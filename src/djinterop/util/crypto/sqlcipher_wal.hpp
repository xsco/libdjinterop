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

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "sqlcipher_codec.hpp"

namespace djinterop::util::crypto
{
/// Decrypt a SQLCipher database, merging its write-ahead log, into a plain
/// SQLite image.
///
/// rekordbox leaves most of a fresh export in the log rather than in the
/// database file, so a reader that ignores it reports a nearly empty library
/// with no error at all.
///
/// The returned image is a standard, unencrypted SQLite file, marked as using
/// a rollback journal: everything the log held has already been folded in.
///
/// Only the pages the finished image is made of are decrypted, and so only
/// those are authenticated: a page the log replaces, or one past the size the
/// log truncates the database to, is never read from the database file and a
/// fault in it goes unreported.  SQLite would not have looked at it either.
///
/// \param database_path Path of the encrypted database.
/// \param codec Codec built for the database, as by `make_codec_for`.
/// \return Returns a plain SQLite image.
/// \throws sqlcipher_error If the database cannot be read or decrypted.
std::vector<uint8_t> decrypt_database_to_image(
    const std::string& database_path, const sqlcipher_codec& codec);

}  // namespace djinterop::util::crypto
