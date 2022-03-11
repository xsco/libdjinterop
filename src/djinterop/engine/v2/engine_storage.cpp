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

#include "engine_storage.hpp"

#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/exceptions.hpp>

#include "../../util.hpp"
#include "../schema/schema.hpp"

namespace djinterop::engine::v2
{
namespace
{
sqlite::database make_attached_db(const std::string& directory, bool must_exist)
{
    if (!dir_exists(directory))
    {
        if (must_exist)
        {
            throw database_not_found{directory};
        }
        else
        {
            // Note: only creates leaf directory, not entire tree.
            create_dir(directory);
        }
    }

    return sqlite::database{directory + "/m.db"};
}

sqlite::database make_temporary_db()
{
    sqlite::database db{":memory:"};
    return db;
}

}  // anonymous namespace

engine_storage::engine_storage(
    const std::string& directory, const engine_version& version) :
    engine_storage{directory, version, make_attached_db(directory, true)}
{
}

engine_storage::engine_storage(
    const std::string& directory, const engine_version& version,
    sqlite::database db) :
    directory{directory},
    version{version}, db_{std::move(db)}
{
}

std::shared_ptr<engine_storage> engine_storage::create(
    const std::string& directory, const engine_version& version)
{
    auto db = make_attached_db(directory, false);

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(version);
    schema_creator->create(db);

    return std::shared_ptr<engine_storage>{
        new engine_storage{directory, version, std::move(db)}};
}

std::shared_ptr<engine_storage> engine_storage::create_temporary(
    const engine_version& version)
{
    auto db = make_temporary_db();

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(version);
    schema_creator->create(db);

    return std::shared_ptr<engine_storage>{
        new engine_storage{":memory:", version, std::move(db)}};
}

void engine_storage::verify()
{
    auto validator = schema::make_schema_creator_validator(version);
    validator->verify(db_);
}

}  // namespace djinterop::engine::v2
