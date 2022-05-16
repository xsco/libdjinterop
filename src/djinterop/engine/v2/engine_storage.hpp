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

#pragma once

#include <cstdint>
#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/engine_version.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/optional.hpp>

#include "../schema/schema.hpp"

namespace djinterop::engine::v2
{
/// The `engine_storage` class provides access to persistent storage for Engine
/// data.
class engine_storage
{
public:
    /// Construct by loading from an existing DB directory.
    engine_storage(const std::string& directory, const engine_version& version);

    /// Make a new, empty DB of a given version.
    static std::shared_ptr<engine_storage> create(
        const std::string& directory, const engine_version& version);

    /// Make a new, empty, in-memory DB of a given version.
    ///
    /// Any changes made to the database will not persist beyond destruction
    /// of the class instance.
    static std::shared_ptr<engine_storage> create_temporary(
        const engine_version& version);

    /// Verify the correctness of the schema.
    void verify();

    /// The directory in which the Engine DB files reside.
    const std::string directory;

    /// The version of the Engine database.
    const engine_version version;

    /// Pointer to the schema creator/validator.
    const std::unique_ptr<schema::schema_creator_validator>
        schema_creator_validator;
private:
    engine_storage(
        const std::string& directory, const engine_version& version,
        sqlite::database db);

    sqlite::database db_;
    int64_t last_savepoint_ = 0;
};

}  // namespace djinterop::engine::v1
