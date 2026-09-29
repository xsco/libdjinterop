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

#include <array>
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

/// Where the image for an album art row is stored.
enum class album_art_storage
{
    /// There is no image stored.
    none,

    /// The image is in the `albumArt` column of the row.
    blob_in_database,

    /// The image is a file in the `Artwork` directory, named after the hash.
    file_in_artwork_dir,

    /// The image is wherever the URI in the `hash` column points.
    uri,
};

/// A hash in text form, as found in rows that store the image in the
/// `albumArt` column.
///
/// Observed as lowercase hexadecimal without leading zeros, so it may be
/// shorter than 40 characters.
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

/// A hash in binary form, as found in rows whose image is a file in the
/// `Artwork` directory.
struct DJINTEROP_PUBLIC album_art_binary_hash
{
    std::array<std::byte, 20> value;

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
/// If the path is absolute, as it is when Engine imports a library from
/// rekordbox and points the URI directly at the rekordbox artwork, there is
/// no guarantee that it can be resolved when the library is loaded
/// elsewhere.
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

/// What the `hash` column of an album art row holds.
///
/// `std::monostate` means the column is null.  How Engine derives either
/// kind of hash from an image is not currently known.
using album_art_hash = std::variant<
    std::monostate, album_art_text_hash, album_art_binary_hash, album_art_uri>;

/// Prefix of a `hash` value that is a URI rather than a hash.
constexpr const char* ALBUM_ART_URI_PREFIX = "image://";

/// Get the name of the file in the `Artwork` directory that holds the image
/// for a given binary hash.
///
/// \param hash Binary hash of the image.
/// \param extension File extension to append, including the leading dot.
/// \return Returns the file name.
DJINTEROP_PUBLIC std::string album_art_file_name(
    const album_art_binary_hash& hash,
    const std::string& extension = ALBUM_ART_DEFAULT_EXTENSION);

/// Represents a row in the `AlbumArt` table.
struct DJINTEROP_PUBLIC album_art_row
{
    /// Auto-generated id column.
    int64_t id;

    /// `hash` column, identifying the image.
    album_art_hash hash;

    /// `albumArt` column, holding the image data.
    ///
    /// Schema versions 3.0.1 and earlier always store image data in this
    /// column.  Schema versions 3.0.2 and later have the option to store the
    /// image data externally in a file on disk, in which case this field
    /// will be empty.
    ///
    /// The storage approach can be determined with the `storage_of()`
    /// function.
    std::vector<std::byte> album_art;

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
                    os << "binary:" << album_art_file_name(hash, "");
                else if constexpr (std::is_same_v<T, album_art_uri>)
                    os << "uri:" << hash.value;
                else
                    os << "text:" << hash.value;
            },
            row.hash);
        os << ", album_art=" << row.album_art.size() << " bytes}";
        return os;
    }
};

/// Determine where the image for an album art row is stored.
///
/// \param row Album art row.
/// \return Returns where the image is stored, or `none` if there is none.
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
    /// A hash only matches a hash of the same kind: a text hash never
    /// matches a binary hash of the same bytes.  If several rows match, the
    /// id of one of them is returned.
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
    void update(const album_art_row& row);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
