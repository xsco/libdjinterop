/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <djinterop/engine/v2/engine_library.hpp>

#include <sqlite_modern_cpp.h>

#include <djinterop/engine/engine.hpp>
#include <djinterop/exceptions.hpp>
#include <memory>
#include <utility>

#include "../../util/filesystem.hpp"
#include "../schema/schema.hpp"
#include "database_impl.hpp"
#include "engine_library_context.hpp"

namespace djinterop::engine::v2
{
namespace
{
inline std::string make_db_dir_path(const std::string& directory)
{
    return directory + "/Database2";
}

inline std::string make_db_path(const std::string& directory)
{
    return directory + "/Database2/m.db";
}

std::shared_ptr<engine_library_context> load_existing(
    const std::string& directory)
{
    auto db_path = make_db_path(directory);
    if (!djinterop::util::path_exists(db_path))
    {
        throw database_not_found{directory};
    }

    sqlite::database db{db_path};

    // Check that the `Information` table exists.
    std::string sql =
        "SELECT COUNT(*) AS rows "
        "FROM sqlite_master "
        "WHERE name = 'Information'";
    int32_t table_count;
    db << sql >> table_count;
    if (table_count != 1)
    {
        throw database_inconsistency{
            "Did not find an `Information` table in the database"};
    }

    // Detect version.
    semantic_version schema_version{};
    db << "SELECT schemaVersionMajor, schemaVersionMinor, "
          "schemaVersionPatch FROM Information" >>
        std::tie(schema_version.maj, schema_version.min, schema_version.pat);

    for (auto&& candidate_version : all_v2_versions)
    {
        if (schema_version == candidate_version.schema_version)
        {
            return std::make_shared<engine_library_context>(
                directory, candidate_version, db);
        }
    }

    throw unsupported_engine_database{schema_version};
}

}  // anonymous namespace

engine_library::engine_library(const std::string& directory) :
    engine_library{load_existing(directory)}
{
}

engine_library::engine_library(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

engine_library engine_library::create(
    const std::string& directory, const engine_version& version)
{
    // Ensure the target directory exists.
    if (!djinterop::util::path_exists(directory))
    {
        djinterop::util::create_dir(directory);
    }

    auto db_dir_path = make_db_dir_path(directory);
    if (!djinterop::util::path_exists(db_dir_path))
    {
        djinterop::util::create_dir(db_dir_path);
    }

    // Target database must not exist.
    auto db_path = make_db_path(directory);
    if (djinterop::util::path_exists(db_path))
    {
        throw database_inconsistency{
            "Cannot create new Engine library, as the database file already "
            "exists"};
    }

    auto db = sqlite::database{db_path};

    auto schema_creator = schema::make_schema_creator_validator(version);
    schema_creator->create(db);

    return engine_library{std::make_shared<engine_library_context>(
        directory, version, std::move(db))};
}

engine_library engine_library::create_temporary(const engine_version& version)
{
    auto db = sqlite::database(":memory:");

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(version);
    schema_creator->create(db);

    return engine_library{std::make_shared<engine_library_context>(
        ":memory:", version, std::move(db))};
}

bool engine_library::exists(const std::string& directory)
{
    return djinterop::util::path_exists(make_db_path(directory));
}

void engine_library::verify() const
{
    auto validator = schema::make_schema_creator_validator(context_->version);
    validator->verify(context_->db);
}

database engine_library::database() const
{
    auto library = std::make_shared<engine_library>(*this);
    auto pimpl = std::make_shared<database_impl>(library);
    return djinterop::database{pimpl};
}

std::string engine_library::directory() const
{
    return context_->directory;
}

engine_version engine_library::version() const
{
    return context_->version;
}

}  // namespace djinterop::engine::v2
