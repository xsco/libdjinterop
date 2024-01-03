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

#include <djinterop/engine/engine_version.hpp>
#include <utility>

namespace djinterop::engine::v2
{
struct engine_library_context
{
    engine_library_context(
        std::string directory, engine_version version, sqlite::database db) :
        directory{std::move(directory)}, version{version}, db{std::move(db)}
    {
    }

    /// The directory in which the Engine DB files reside.
    const std::string directory;

    /// The version of the Engine database.
    const engine_version version;

    /// The main SQLite database holding Engine data.
    sqlite::database db;
};

}  // namespace djinterop::engine::v2
