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

#include <djinterop/engine/engine_schema.hpp>

namespace djinterop::engine
{
struct engine_library_context
{
    engine_library_context(
        std::string directory, bool is_database2, engine_schema schema,
        sqlite::database db) :
        directory{std::move(directory)}, is_database2{is_database2},
        schema{schema}, db{std::move(db)}
    {
    }

    /// The directory in which the Engine DB files reside.
    const std::string directory;

    /// Flag indicating whether the directory is of the 'Database2' structure.
    ///
    /// A 'Database2' library has the SQLite database files under a
    /// subdirectory named `Database2`.  A legacy-type library has the SQLite
    /// database files immediately under the main directory.
    const bool is_database2;

    // TODO(mr-smidge): Add `uuid`, a sufficiently important identifier that
    //  it should be stored for all Engine-wide contexts.

    /// The schema version of the Engine database.
    const engine_schema schema;

    /// The main SQLite database holding Engine data.
    sqlite::database db;
};

}  // namespace djinterop::engine
