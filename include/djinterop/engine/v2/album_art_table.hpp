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
/// \param hash Hash of the image, as found in an album art row.
/// \param extension File extension to append, including the leading dot.
/// \return Returns the file name.
DJINTEROP_PUBLIC std::string album_art_file_name(
    const std::vector<std::byte>& hash,
    const std::string& extension = ALBUM_ART_DEFAULT_EXTENSION);

/// Represents a row in the `AlbumArt` table.
///
/// Note that Engine does not store the image itself in the database.  The
/// `albumArt` column is always null, and the image lives in a file beneath the
/// `Artwork` directory of the library, named after the row's hash.  See
/// `album_art_file_name()`.
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
    /// The column is declared as `TEXT`, but Engine writes a 20-byte binary
    /// hash of the image into it.  The precise derivation of the hash is not
    /// known, and does not appear to be checked by hardware players when the
    /// art is displayed: it acts as a key, and any stable, collision-resistant
    /// value of that width has been observed to work.
    ///
    /// The column is also used to hold a URI of the form
    /// `image://fileart/<path>`, written by Engine's own import of a
    /// rekordbox library, which points at the artwork of the imported library
    /// rather than at a file beneath `Artwork`.  Such a URI is an absolute
    /// path, and so does not survive the medium being mounted elsewhere.
    std::vector<std::byte> hash;

    /// `albumArt` column, holding the image itself.
    ///
    /// Always null in libraries written by Engine.
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
        os << "album_art_row{id=" << row.id
           << ", hash(base64url)=" << album_art_file_name(row.hash, "")
           << ", album_art=";
        if (row.album_art)
            os << row.album_art->size() << " bytes";
        else
            os << "none";
        os << "}";
        return os;
    }
};

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
    /// \param hash Hash to find.
    /// \return Returns the id of the row, or none if not found.
    [[nodiscard]] std::optional<int64_t> find_id(
        const std::vector<std::byte>& hash) const;

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
