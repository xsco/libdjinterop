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

#include <string>

#include <djinterop/semantic_version.hpp>
#include <sqlite_modern_cpp.h>

namespace djinterop
{
namespace enginelibrary
{
class el_storage
{
public:
    /// Returns a boolean indicating whether a given schema version is
    /// supported.
    static bool schema_version_supported(semantic_version schema_version);

    el_storage(std::string directory);

    /// Create and validate schema in an empty EL storage DB.
    void create_and_validate_schema(semantic_version schema_version);

    /// Returns a boolean indicating whether the EL DB has any schema.
    bool schema_created() const;

    std::string directory;

    // TODO - don't expose mutable SQLite connection - allow txn guard to be
    // obtained from el_storage by other EL classes.
    mutable sqlite::database db;
    int64_t last_savepoint = 0;
};

}  // namespace enginelibrary
}  // namespace djinterop
