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

#include <djinterop/onelibrary/onelibrary.hpp>

#include <cstring>
#include <memory>
#include <new>
#include <string>

#include <sqlite3.h>

#include <djinterop/exceptions.hpp>

#include "../util/crypto/sqlcipher_codec.hpp"
#include "../util/crypto/sqlcipher_wal.hpp"
#include "../util/filesystem.hpp"
#include "database_impl.hpp"
#include "onelibrary_context.hpp"

namespace djinterop::onelibrary
{
namespace
{
struct resolved_location
{
    /// Root directory of the device, to which track paths are relative.
    std::string directory;

    /// The database file itself.
    std::string database_path;
};

/// Work out where the database is, given either a device or a database file.
resolved_location resolve(const std::string& path)
{
    // A path that names the database directly implies its device root, which
    // is three levels up: `<root>/PIONEER/rekordbox/exportLibrary.db`.
    if (!util::path_is_directory(path))
    {
        // Walking up by index, rather than by assigning a piece of a string
        // back to itself three times over, which is a shape GCC's -Wrestrict
        // cannot see the safety of.
        auto end = path.size();
        for (int level = 0; level < 3; ++level)
        {
            const auto separator =
                end == 0 ? std::string::npos
                         : path.find_last_of("/\\", end - 1);

            // A relative path with nothing above it sits in the working
            // directory, which is then the root of the device.
            if (separator == std::string::npos)
                return resolved_location{".", path};

            end = separator;
        }

        return resolved_location{path.substr(0, end), path};
    }

    return resolved_location{path, path + "/" + database_relative_path};
}

/// Open the database, folding in its write-ahead log.
///
/// The database is decrypted into a plain SQLite image, which is handed back
/// as an in-memory database.  A device is written in WAL mode and checkpointed
/// on eject, which leaves the header declaring the database
/// write-ahead-logged, and SQLite will not open one of those read-only without
/// the log that is no longer there -- so the log has to be folded in here, and
/// the header rewritten, before SQLite ever sees the bytes.
sqlite::database open_database(
    const std::string& database_path, const std::string& passphrase)
{
#if defined(SQLITE_OMIT_DESERIALIZE) || SQLITE_VERSION_NUMBER < 3036000
    throw unsupported_database{
        "The database `" + database_path +
        "` needs SQLite 3.36 or newer, built without SQLITE_OMIT_DESERIALIZE, "
        "to read"};
#else
    // Key derivation is deliberately expensive, so it is done once here and
    // the codec is handed to everything that reads a page.
    const auto codec = util::crypto::make_codec_for(database_path, passphrase);
    if (!codec)
        throw unsupported_database{
            "The file `" + database_path + "` is too small to be a database"};

    const auto image =
        util::crypto::decrypt_database_to_image(database_path, *codec);

    sqlite::database db{":memory:"};

    // SQLite takes ownership of the buffer and frees it with the connection,
    // so it must come from SQLite's own allocator.
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
        throw unsupported_database{
            "The database `" + database_path +
            "` could not be read once it had been decrypted"};

    return db;
#endif
}

/// Read one column of a track's row, if the row is there and the column set.
///
/// The columns behind `library` are each read on their own, rather than
/// through `content_table`, whose query joins six lookup tables to build a
/// whole row that none of them needs.
template <typename T>
std::optional<T> track_column(
    onelibrary_context& context, const char* sql, int64_t track_id)
{
    std::optional<T> result;
    context.db << sql << track_id >>
        [&](std::optional<T> value) { result = std::move(value); };

    return result;
}

/// Decrypt the database on a device and check that it is one.
std::shared_ptr<onelibrary_context> load_context(
    const std::string& path, const std::string& passphrase)
{
    const auto location = resolve(path);

    if (!util::path_exists(location.database_path))
        throw database_not_found{location.database_path};

    // Key derivation is deliberately expensive, so the passphrase is not
    // tested separately: a wrong one shows up as the database failing to open,
    // and is reported as such.
    std::shared_ptr<onelibrary_context> context;
    try
    {
        context = std::make_shared<onelibrary_context>(
            location.directory,
            open_database(location.database_path, passphrase));

        // Opening a database reads nothing, so touch it here: a wrong
        // passphrase would otherwise not be noticed until the first query.
        context->db << "SELECT COUNT(*) FROM sqlite_master" >> [](int64_t) {};
    }
    catch (const sqlite::sqlite_exception&)
    {
        throw unsupported_database{
            "The file `" + location.database_path +
            "` is not a SQLCipher database that the given passphrase opens"};
    }
    catch (const util::crypto::sqlcipher_error&)
    {
        throw unsupported_database{
            "The file `" + location.database_path +
            "` is not a SQLCipher database that the given passphrase opens"};
    }

    // Fail here, while the caller still has the path in hand, rather than at
    // the first query.
    database_impl{context}.verify();

    return context;
}

}  // anonymous namespace

bool database_exists(const std::string& path)
{
    const auto location = resolve(path);
    return util::path_exists(location.database_path);
}

database load_database(const std::string& path, const std::string& passphrase)
{
    return database{std::make_shared<database_impl>(
        load_context(path, passphrase))};
}

library::library(const std::string& path, const std::string& passphrase) :
    context_{load_context(path, passphrase)}
{
}

database library::db() const
{
    return database{std::make_shared<database_impl>(context_)};
}

const std::string& library::directory() const
{
    return context_->directory;
}

std::optional<std::string> library::analysis_path(int64_t track_id) const
{
    const auto path = track_column<std::string>(
        *context_, "SELECT analysisDataFilePath FROM content WHERE content_id = ?",
        track_id);
    if (!path || path->empty())
        return std::nullopt;

    // Paths are absolute within the device, whereas every path this library
    // hands out is relative to its root.
    return path->front() == '/' ? path->substr(1) : *path;
}

std::optional<std::string> library::key_name(int64_t track_id) const
{
    const auto name = track_column<std::string>(
        *context_,
        "SELECT \"key\".name FROM content AS c "
        // `key` is quoted throughout, as it is also a SQL keyword.
        "LEFT JOIN \"key\" ON \"key\".key_id = c.key_id "
        "WHERE c.content_id = ?",
        track_id);
    if (!name || name->empty())
        return std::nullopt;

    return name;
}

std::optional<int> library::color_id(int64_t track_id) const
{
    const auto id = track_column<int64_t>(
        *context_, "SELECT color_id FROM content WHERE content_id = ?",
        track_id);
    if (!id || *id == 0)
        return std::nullopt;

    return static_cast<int>(*id);
}

}  // namespace djinterop::onelibrary
