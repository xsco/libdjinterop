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

#include <filesystem>
#include <memory>
#include <string>
#include <utility>

#include <sqlite_modern_cpp.h>

#include <djinterop/exceptions.hpp>

namespace djinterop::onelibrary
{
/// State shared by everything belonging to one loaded OneLibrary database.
struct onelibrary_context
{
    onelibrary_context(std::filesystem::path directory, sqlite::database db) :
        directory{std::move(directory)}, db{std::move(db)}
    {
    }

    /// Root directory of the device.
    ///
    /// Track paths in the database are relative to this, not to the directory
    /// the database file itself sits in.
    const std::filesystem::path directory;

    /// The database, opened with the key that decrypts it.
    sqlite::database db;
};

/// Open the database on a device and check that it is one.
///
/// \param path Either the root directory of a device, or the database file.
std::shared_ptr<onelibrary_context> load_context(
    const std::filesystem::path& path, const std::string& passphrase);

/// Create a database on a device from a script, and open it.
///
/// \param directory Root directory of the device.
/// \param script_directory Directory holding `exportLibrary.db.sql`.
std::shared_ptr<onelibrary_context> create_context_from_scripts(
    const std::filesystem::path& directory,
    const std::filesystem::path& script_directory,
    const std::string& passphrase);

/// Check that a database holds the tables, indexes and versioned `property` row of an export.
///
/// \throws database_inconsistency If a table, an index or the `property` row
///                                is missing.
void verify_schema(onelibrary_context& context);

/// Refuse an operation that would change the database.
[[noreturn]] inline void read_only()
{
    throw unsupported_operation{
        "OneLibrary databases are currently read-only in libdjinterop"};
}

}  // namespace djinterop::onelibrary
