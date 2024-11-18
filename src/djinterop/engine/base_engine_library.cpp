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

#include <djinterop/engine/base_engine_library.hpp>

#include <sqlite_modern_cpp.h>

#include <djinterop/engine/engine.hpp>
#include <memory>
#include <utility>

#include "engine_library_context.hpp"
#include "engine_library_dir_utils.hpp"
#include "schema/schema.hpp"

namespace djinterop::engine
{
base_engine_library::base_engine_library(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

std::shared_ptr<engine_library_context> base_engine_library::load(
    const std::string& directory)
{
    auto db = load_database2_sqlite_database(directory);

    const auto schema = schema::detect_schema(db);
    return std::make_shared<engine_library_context>(
        directory, true, schema, db);
}

std::shared_ptr<engine_library_context> base_engine_library::create(
    const std::string& directory, const engine_schema& schema)
{
    auto db = create_database2_sqlite_database(directory);

    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return std::make_shared<engine_library_context>(
        directory, true, schema, std::move(db));
}

std::shared_ptr<engine_library_context> base_engine_library::create_temporary(
    const engine_schema& schema)
{
    auto db = create_temporary_database2_sqlite_database();

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return std::make_shared<engine_library_context>(
        ":memory:", true, schema, std::move(db));
}

bool base_engine_library::exists(const std::string& directory)
{
    return database2_database_exists(directory);
}

void base_engine_library::verify() const
{
    auto validator = schema::make_schema_creator_validator(context_->schema);
    validator->verify(context_->db);
}

std::string base_engine_library::directory() const
{
    return context_->directory;
}

engine_schema base_engine_library::schema() const
{
    return context_->schema;
}

}  // namespace djinterop::engine
