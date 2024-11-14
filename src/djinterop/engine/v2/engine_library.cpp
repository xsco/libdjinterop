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

    const auto schema = schema::detect_schema(db);
    return std::make_shared<engine_library_context>(directory, schema, db);
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
    const std::string& directory, const engine_schema& schema)
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

    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return engine_library{std::make_shared<engine_library_context>(
        directory, schema, std::move(db))};
}

engine_library engine_library::create_temporary(const engine_schema& schema)
{
    auto db = sqlite::database(":memory:");

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return engine_library{std::make_shared<engine_library_context>(
        ":memory:", schema, std::move(db))};
}

bool engine_library::exists(const std::string& directory)
{
    return djinterop::util::path_exists(make_db_path(directory));
}

void engine_library::verify() const
{
    auto validator =
        schema::make_schema_creator_validator(context_->schema);
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

engine_schema engine_library::schema() const
{
    return context_->schema;
}

}  // namespace djinterop::engine::v2
