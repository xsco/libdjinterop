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

#include <sqlite_modern_cpp.h>

#include <djinterop/semantic_version.hpp>

#include "schema/schema.hpp"

namespace djinterop::enginelibrary
{
class el_storage
{
public:
    /// Construct by loading from an existing DB directory.
    el_storage(const std::string& directory);

    /// Construct by making a new, empty DB of a given version.
    el_storage(const std::string& directory, semantic_version version);

    const std::string directory;
    // TODO - don't expose mutable SQLite connection - allow txn guard to be
    // obtained from el_storage by other EL classes.
    sqlite::database db;

    const semantic_version version;
    const std::unique_ptr<schema::schema_creator_validator> schema_creator_validator;

    int64_t last_savepoint = 0;
};

}  // namespace djinterop::enginelibrary
