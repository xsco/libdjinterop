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

#include "sqlcipher.hpp"

#include <memory>

#include <sqlite3.h>

#include <djinterop/exceptions.hpp>

namespace djinterop::util
{
sqlite::database open_encrypted_database(
    const std::string& path, const std::string& passphrase)
{
    // A write-ahead-logged database cannot be read without the shared-memory
    // index beside it, so even a read-only connection creates one if it is
    // missing.
    sqlite3* raw = nullptr;
    const auto open_rc =
        sqlite3_open_v2(path.c_str(), &raw, SQLITE_OPEN_READONLY, nullptr);
    std::shared_ptr<sqlite3> connection{raw, sqlite3_close_v2};
    if (open_rc != SQLITE_OK)
        throw unsupported_database{
            "The database `" + path + "` could not be opened"};

    // Setting the key reads nothing, so read something: a wrong passphrase
    // would otherwise not be noticed until the first query.
    const std::unique_ptr<char, decltype(&sqlite3_free)> sql{
        sqlite3_mprintf(
            "PRAGMA key = %Q; SELECT COUNT(*) FROM sqlite_master",
            passphrase.c_str()),
        sqlite3_free};
    if (!sql ||
        sqlite3_exec(raw, sql.get(), nullptr, nullptr, nullptr) != SQLITE_OK)
        throw unsupported_database{
            "The file `" + path +
            "` is not a SQLCipher database that the given passphrase opens"};

    return sqlite::database{connection};
}

}  // namespace djinterop::util
