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

// The implementation of `encrypted_database.hpp` that decrypts SQLCipher
// pages itself, rather than by linking a copy of SQLCipher.

#include "encrypted_database.hpp"

#include <cstdint>
#include <cstring>
#include <new>
#include <vector>

#include <sqlite3.h>

#include "sqlcipher_codec.hpp"
#include "sqlcipher_wal.hpp"

// The decrypted image is handed to SQLite through `sqlite3_deserialize`, from
// SQLite 3.36, which can also be compiled out.
#if defined(SQLITE_OMIT_DESERIALIZE) || SQLITE_VERSION_NUMBER < 3036000
#define DJINTEROP_HAVE_DESERIALIZE 0
#else
#define DJINTEROP_HAVE_DESERIALIZE 1
#endif

namespace djinterop::util::crypto
{
bool encrypted_databases_supported() noexcept
{
    return DJINTEROP_HAVE_DESERIALIZE;
}

sqlite::database open_encrypted_database(
    const std::string& path, const std::string& passphrase)
{
#if !DJINTEROP_HAVE_DESERIALIZE
    (void)passphrase;
    throw encryption_unsupported{
        "The database `" + path +
        "` needs SQLite 3.36 or newer, built without SQLITE_OMIT_DESERIALIZE, "
        "to read"};
#else
    // Key derivation is expensive, so it is done once here.
    const auto codec = make_codec_for(path, passphrase);
    if (!codec)
        throw encrypted_database_error{
            "The file `" + path + "` is too small to be a database"};

    std::vector<uint8_t> image;
    try
    {
        // A device is checkpointed on eject but still declares itself
        // write-ahead-logged, which SQLite will not open read-only without
        // the log.  Fold it in and rewrite the header first.
        image = decrypt_database_to_image(path, *codec);
    }
    catch (const sqlcipher_error&)
    {
        throw encrypted_database_error{
            "The file `" + path +
            "` is not a SQLCipher database that the given passphrase opens"};
    }

    sqlite::database db{":memory:"};

    // SQLite frees the buffer with the connection, so it must come from
    // SQLite's own allocator.
    auto* buffer = static_cast<uint8_t*>(sqlite3_malloc64(image.size()));
    if (buffer == nullptr)
        throw std::bad_alloc{};

    std::memcpy(buffer, image.data(), image.size());

    const auto rc = sqlite3_deserialize(
        db.connection().get(), "main", buffer,
        static_cast<sqlite3_int64>(image.size()),
        static_cast<sqlite3_int64>(image.size()),
        SQLITE_DESERIALIZE_FREEONCLOSE | SQLITE_DESERIALIZE_READONLY);
    if (rc != SQLITE_OK)
        throw encrypted_database_error{
            "The database `" + path +
            "` could not be read once it had been decrypted"};

    return db;
#endif
}

}  // namespace djinterop::util::crypto

#undef DJINTEROP_HAVE_DESERIALIZE
