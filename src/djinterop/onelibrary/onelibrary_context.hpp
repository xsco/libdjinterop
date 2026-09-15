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
#include <utility>

#include <sqlite_modern_cpp.h>

#include <djinterop/exceptions.hpp>

namespace djinterop::onelibrary
{
/// State shared by everything belonging to one loaded OneLibrary database.
struct onelibrary_context
{
    onelibrary_context(std::string directory, sqlite::database db) :
        directory{std::move(directory)}, db{std::move(db)}
    {
    }

    /// Root directory of the device.
    ///
    /// Track paths in the database are relative to this, not to the directory
    /// the database file itself sits in.
    const std::string directory;

    /// The database, decrypted from the device into memory.
    sqlite::database db;
};

/// Refuse an operation that would change the database.
[[noreturn]] inline void read_only()
{
    throw unsupported_operation{
        "OneLibrary databases are currently read-only in libdjinterop"};
}

}  // namespace djinterop::onelibrary
