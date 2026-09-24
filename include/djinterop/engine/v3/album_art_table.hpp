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

#include <djinterop/config.hpp>
#include <djinterop/engine/v2/album_art_table.hpp>

namespace djinterop::engine::v3
{
/// Thrown when the id on an album art row is in an erroneous state for a given
/// operation.
using album_art_row_id_error = djinterop::engine::v2::album_art_row_id_error;

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr int64_t ALBUM_ART_ROW_ID_NONE =
    djinterop::engine::v2::ALBUM_ART_ROW_ID_NONE;

/// Name of the directory, directly beneath the Engine library directory, in
/// which album art images are stored.
constexpr const char* ALBUM_ART_DIRECTORY =
    djinterop::engine::v2::ALBUM_ART_DIRECTORY;

/// Default file extension for album art images.
constexpr const char* ALBUM_ART_DEFAULT_EXTENSION =
    djinterop::engine::v2::ALBUM_ART_DEFAULT_EXTENSION;

/// Get the name of the file in which the image for a given hash is stored.
using djinterop::engine::v2::album_art_file_name;

/// Where a library keeps its album art.
using album_art_storage = djinterop::engine::v2::album_art_storage;

/// A hash as written when the image is stored in the `albumArt` column.
using album_art_text_hash = djinterop::engine::v2::album_art_text_hash;

/// A hash as written when the image is a file beneath `Artwork`.
using album_art_binary_hash = djinterop::engine::v2::album_art_binary_hash;

/// A URI in the `hash` column, written by Engine's rekordbox import.
using album_art_uri = djinterop::engine::v2::album_art_uri;

/// What the `hash` column holds.
using album_art_hash = djinterop::engine::v2::album_art_hash;

/// Prefix of a `hash` value that is a URI rather than a hash.
constexpr const char* ALBUM_ART_URI_PREFIX =
    djinterop::engine::v2::ALBUM_ART_URI_PREFIX;

/// Where the image for a given row lives.
using djinterop::engine::v2::storage_of;

/// Represents a row in the `AlbumArt` table.
using album_art_row = djinterop::engine::v2::album_art_row;

/// Represents the `AlbumArt` table in an Engine v2 database.
using album_art_table = djinterop::engine::v2::album_art_table;

}  // namespace djinterop::engine::v3
