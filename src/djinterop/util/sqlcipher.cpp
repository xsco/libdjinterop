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

// `sqlite3_key` is declared only when `SQLITE_HAS_CODEC` is defined before
// `sqlite3.h` is first included.
#ifndef SQLITE_HAS_CODEC
#define SQLITE_HAS_CODEC
#endif
#include <sqlite3.h>

#include "sqlcipher.hpp"

#include <djinterop/exceptions.hpp>

namespace djinterop::util
{
namespace
{
// The wrapper's own `sqlite_modern_cpp/sqlcipher.h` would do this, but the
// bundled copy of it no longer compiles against the bundled wrapper, so the
// key is set through the C API directly.
sqlite::database open(
    const std::string& path, const std::string& passphrase,
    sqlite::OpenFlags flags)
{
    sqlite::sqlite_config config;
    config.flags = flags;

    try
    {
        sqlite::database db{path, config};
        if (const auto rc = sqlite3_key(
                db.connection().get(), passphrase.data(),
                static_cast<int>(passphrase.size()));
            rc != SQLITE_OK)
            sqlite::errors::throw_sqlite_error(rc);

        return db;
    }
    catch (const sqlite::sqlite_exception& e)
    {
        throw unsupported_database{
            "The database `" + path + "` could not be opened: " + e.what()};
    }
}

}  // anonymous namespace

sqlite::database open_encrypted_database(
    const std::string& path, const std::string& passphrase)
{
    // A write-ahead-logged database cannot be read without the shared-memory
    // index beside it, so even a read-only connection creates one if it is
    // missing.
    auto db = open(path, passphrase, sqlite::OpenFlags::READONLY);

    // Setting the key reads nothing, so read something: a wrong passphrase
    // would otherwise not be noticed until the first query.
    try
    {
        db << "SELECT COUNT(*) FROM sqlite_master" >> [](int64_t) {};
    }
    catch (const sqlite::sqlite_exception&)
    {
        throw unsupported_database{
            "The file `" + path +
            "` is not a SQLCipher database that the given passphrase opens"};
    }

    return db;
}

sqlite::database create_encrypted_database(
    const std::string& path, const std::string& passphrase)
{
    return open(
        path, passphrase,
        sqlite::OpenFlags::READWRITE | sqlite::OpenFlags::CREATE);
}

}  // namespace djinterop::util
