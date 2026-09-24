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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::engine
{
struct engine_library_context;
}

namespace djinterop::engine::v2
{
/// Thrown when the id on an album art row is in an erroneous state for a given
/// operation.
struct DJINTEROP_PUBLIC album_art_row_id_error : public std::runtime_error
{
    explicit album_art_row_id_error(const std::string& what_arg) noexcept :
        runtime_error{what_arg}
    {
    }
};

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr int64_t ALBUM_ART_ROW_ID_NONE = 0;

/// Name of the directory, directly beneath the Engine library directory, in
/// which album art images are stored.
constexpr const char* ALBUM_ART_DIRECTORY = "Artwork";

/// Default file extension for album art images.
///
/// Every image written by Engine that has been observed so far is a JPEG.
constexpr const char* ALBUM_ART_DEFAULT_EXTENSION = ".jpg";

/// Get the name of the file in which the image for a given hash is stored.
///
/// The file resides in the `Artwork` directory of the Engine library, and is
/// named after the hash, encoded as unpadded base64url.
///
/// Only a binary hash names a file: a library that keeps its art in the
/// `albumArt` column has no file to name, and a URI already says where its
/// image is.  See `album_art_hash`.
///
/// An empty hash encodes to nothing, so the result is the extension alone.
/// No row can name such a file: `album_art_table::add()` and `update()`
/// reject an empty binary hash.
///
/// \param hash Binary hash of the image, as found in an album art row.
/// \param extension File extension to append, including the leading dot.
/// \return Returns the file name.
DJINTEROP_PUBLIC std::string album_art_file_name(
    const std::vector<std::byte>& hash,
    const std::string& extension = ALBUM_ART_DEFAULT_EXTENSION);

/// Where a library keeps its album art, which changed with Engine 4.5.0.
///
/// Firmware 4.3.4 and earlier store the image in the `albumArt` column.
/// Engine Desktop 4.5.0 and later store it as a file beneath the `Artwork`
/// directory by default, and can still read a library that keeps it in the
/// column: a library migrated by 4.5.0 retains the image data in the
/// database until the user chooses to convert it.  Drives written in the
/// new layout may show no artwork on Engine OS 4.3.4 and older, which do
/// not know about it.  See the release notes:
/// https://support.inmusicstore.com/en/support/solutions/articles/69000875932-engine-dj-album-art-changes-with-4-5-0
///
/// Both layouts are therefore live, and a row says which one it is by what
/// its `hash` column holds: see `album_art_hash`.
enum class album_art_storage
{
    /// There is no image to be found for this row.
    ///
    /// The row at id 1, seeded by the schema, is of this kind: an empty
    /// hash and a null `albumArt`, which every track without art points
    /// at.  So is a row whose hash is null, and a text hash with no image
    /// beside it, which names no file and points nowhere.
    none,

    /// The image is in the `albumArt` column of this row.
    in_database,

    /// The image is a file beneath `Artwork`, named after the hash.
    as_file,

    /// The hash is a URI pointing at another library's artwork.
    elsewhere,
};

/// A hash as written when the image is stored in the `albumArt` column.
///
/// Text, as the column's declared type says.
struct DJINTEROP_PUBLIC album_art_text_hash
{
    std::string value;

    friend bool operator==(
        const album_art_text_hash& lhs, const album_art_text_hash& rhs)
    {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(
        const album_art_text_hash& lhs, const album_art_text_hash& rhs)
    {
        return !(lhs == rhs);
    }
};

/// A hash as written when the image is a file beneath `Artwork`.
///
/// A 20-byte binary value, in a column declared `TEXT`.  The precise
/// derivation is not known, and does not appear to be checked by hardware
/// players when the art is displayed: it acts as a key, and any stable,
/// collision-resistant value of that width has been observed to work.  The
/// file is named after it; see `album_art_file_name()`.
struct DJINTEROP_PUBLIC album_art_binary_hash
{
    std::vector<std::byte> value;

    friend bool operator==(
        const album_art_binary_hash& lhs, const album_art_binary_hash& rhs)
    {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(
        const album_art_binary_hash& lhs, const album_art_binary_hash& rhs)
    {
        return !(lhs == rhs);
    }
};

/// A URI in the `hash` column, of the form `image://fileart/<path>`.
///
/// Written by Engine's own import of a rekordbox library, and pointing at
/// the artwork of the imported library rather than at a file beneath
/// `Artwork`.  The path is absolute, and so does not survive the medium
/// being mounted elsewhere.
struct DJINTEROP_PUBLIC album_art_uri
{
    std::string value;

    friend bool operator==(const album_art_uri& lhs, const album_art_uri& rhs)
    {
        return lhs.value == rhs.value;
    }

    friend bool operator!=(const album_art_uri& lhs, const album_art_uri& rhs)
    {
        return !(lhs == rhs);
    }
};

/// What the `hash` column holds.
///
/// Which one a given row holds is decided on reading: a value stored as a
/// SQLite BLOB is a binary hash, and a value stored as TEXT is a URI if it
/// begins with `image://` and a text hash otherwise.
///
/// `std::monostate` is a null column, which the schema permits on every
/// version.  Those three storage classes and null are all that can occur:
/// the column has TEXT affinity, so SQLite converts a number written to it
/// into text.
using album_art_hash = std::variant<
    std::monostate, album_art_text_hash, album_art_binary_hash, album_art_uri>;

/// Prefix of a `hash` value that is a URI rather than a hash.
constexpr const char* ALBUM_ART_URI_PREFIX = "image://";

/// Represents a row in the `AlbumArt` table.
///
/// Whether the image is in this row or in a file beneath `Artwork` depends
/// on the Engine version that wrote the library; see `album_art_storage`.
struct DJINTEROP_PUBLIC album_art_row
{
    /// Auto-generated id column.
    ///
    /// Note that the row at id 1 is created as part of the schema, holds no
    /// art, and is the row that tracks without album art refer to, by way of
    /// `ALBUM_ART_ID_NONE`.
    int64_t id;

    /// `hash` column, identifying the image.
    ///
    /// One of four things, distinguished on reading: see `album_art_hash`.
    album_art_hash hash;

    /// `albumArt` column, holding the image itself.
    ///
    /// Populated in a library whose art is stored in the database, which is
    /// what Engine wrote before 4.5.0 and what a library migrated by 4.5.0
    /// keeps until it is converted.  Null where the image is a file beneath
    /// `Artwork`.
    std::optional<std::vector<std::byte>> album_art;

    friend bool operator==(const album_art_row& lhs, const album_art_row& rhs)
    {
        return lhs.id == rhs.id && lhs.hash == rhs.hash &&
               lhs.album_art == rhs.album_art;
    }

    friend bool operator!=(const album_art_row& lhs, const album_art_row& rhs)
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const album_art_row& row)
    {
        os << "album_art_row{id=" << row.id << ", hash=";
        std::visit(
            [&os](const auto& hash)
            {
                using T = std::decay_t<decltype(hash)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                    os << "null";
                else if constexpr (std::is_same_v<T, album_art_binary_hash>)
                    os << "binary/base64url:"
                       << album_art_file_name(hash.value, "");
                else if constexpr (std::is_same_v<T, album_art_uri>)
                    os << "uri:" << hash.value;
                else
                    os << "text:" << hash.value;
            },
            row.hash);
        os << ", album_art=";
        if (row.album_art)
            os << row.album_art->size() << " bytes";
        else
            os << "none";
        os << "}";
        return os;
    }
};

/// Where the image for a given row lives.
///
/// The question needs the whole row, not just its hash: a text hash says
/// the image is in the `albumArt` column, and only the column itself says
/// whether one is actually there.  A row carrying an image is reported as
/// `in_database` whatever its hash, because that image is the one to read.
///
/// \param row Album art row to classify.
/// \return Returns where this row's image lives, if anywhere.
DJINTEROP_PUBLIC album_art_storage storage_of(const album_art_row& row);

/// Represents the `AlbumArt` table in an Engine v2 database.
class DJINTEROP_PUBLIC album_art_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit album_art_table(std::shared_ptr<engine_library_context> context);

    /// Add an album art row to the table.
    ///
    /// \param row Album art row to add.
    /// \return Returns the id of the newly-added album art row.
    /// \throws album_art_row_id_error If the row already has an id.
    /// \throws std::invalid_argument If the row holds an empty binary hash,
    ///         which SQLite cannot tell from a null column and which names
    ///         no file.
    int64_t add(const album_art_row& row);

    /// Get all album art ids.
    ///
    /// The results are not sorted in any particular order.
    ///
    /// \return Returns a vector of album art ids.
    [[nodiscard]] std::vector<int64_t> all_ids() const;

    /// Test whether a given album art id exists.
    ///
    /// \param id Id of album art to test.
    /// \return Returns `true` if the album art exists, or `false` if not.
    [[nodiscard]] bool exists(int64_t id) const;

    /// Find the id of the album art row with a given hash.
    ///
    /// The hash is matched as it is stored: a binary hash against a BLOB, a
    /// text hash or URI against TEXT, and a null hash against a null column.
    /// SQLite does not equate a BLOB with TEXT of the same bytes, and
    /// neither does Engine.
    ///
    /// Nothing stops a library from holding the same hash twice, so if
    /// several rows match, the id of one of them is returned.
    ///
    /// \param hash Hash to find.
    /// \return Returns the id of a row with that hash, or none if not
    ///         found.
    [[nodiscard]] std::optional<int64_t> find_id(
        const album_art_hash& hash) const;

    /// Get an album art row by id.
    ///
    /// \param id Id of album art.
    /// \return Returns an optional album art row.
    [[nodiscard]] std::optional<album_art_row> get(int64_t id) const;

    /// Remove an entry from the table.
    ///
    /// Note that this does not remove the image file, if any, and does not
    /// change any track that refers to the removed row.
    ///
    /// \param id Id of album art to remove.
    void remove(int64_t id);

    /// Update an existing album art row in the table.
    ///
    /// \param row Album art row to update.
    /// \throws album_art_row_id_error If the row has no id.
    /// \throws std::invalid_argument If the row holds an empty binary hash,
    ///         which SQLite cannot tell from a null column and which names
    ///         no file.
    void update(const album_art_row& row);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
