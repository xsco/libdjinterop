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

#include <djinterop/engine/v2/album_art_table.hpp>

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include "../engine_library_context.hpp"

namespace djinterop::engine::v2
{
namespace
{
constexpr char base64url_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/// Encode up to three bytes as base64url characters.
///
/// Where the numbers come from: base64 works on 24-bit groups, so the three
/// bytes are packed into one integer, most significant byte first, and read
/// back out six bits at a time, giving four characters, at bit offsets 18,
/// 12, 6 and 0.  A group that is short is padded with zero bits rather
/// than with '=' characters, and yields one character more than it has bytes:
/// two characters for one byte (8 bits needs two 6-bit digits), three for two
/// (16 bits needs three).  That is the unpadded base64url Engine uses for
/// its artwork file names.
void encode_base64url_group(
    const std::byte* group, std::size_t length, std::string& result)
{
    constexpr std::size_t bytes_per_group = 3;
    constexpr std::size_t bits_per_character = 6;
    constexpr uint_fast32_t character_mask = 0x3F;  // six bits

    uint_fast32_t bits = 0;
    for (std::size_t i = 0; i < bytes_per_group; ++i)
    {
        bits <<= 8;
        if (i < length)
            bits |= std::to_integer<uint_fast32_t>(group[i]);
    }

    for (std::size_t i = 0; i < length + 1; ++i)
    {
        const auto shift = static_cast<uint_fast32_t>(
            (bytes_per_group * 8) - bits_per_character * (i + 1));
        result += base64url_alphabet[(bits >> shift) & character_mask];
    }
}

/// What the column holds, told apart by how SQLite stored the value: a
/// null column is a null hash, a BLOB is a binary hash, and TEXT is a URI
/// if it begins with `image://` and a text hash otherwise.
///
/// The column is nullable in every schema, so the null case is not
/// hypothetical: without it, reading a library that has one null hash
/// throws out of `get()` and takes the whole enumeration with it.
album_art_hash hash_from_column(
    std::variant<std::monostate, std::string, std::vector<std::byte>> value)
{
    if (std::holds_alternative<std::monostate>(value))
        return std::monostate{};

    if (std::holds_alternative<std::vector<std::byte>>(value))
        return album_art_binary_hash{
            std::get<std::vector<std::byte>>(std::move(value))};

    auto text = std::get<std::string>(std::move(value));
    if (text.rfind(ALBUM_ART_URI_PREFIX, 0) == 0)
        return album_art_uri{std::move(text)};

    return album_art_text_hash{std::move(text)};
}
}  // anonymous namespace

std::string album_art_file_name(
    const std::vector<std::byte>& hash, const std::string& extension)
{
    std::string result;
    result.reserve((hash.size() + 2) / 3 * 4 + extension.length());

    std::size_t offset = 0;
    for (; offset + 3 <= hash.size(); offset += 3)
        encode_base64url_group(hash.data() + offset, 3, result);

    if (offset < hash.size())
        encode_base64url_group(
            hash.data() + offset, hash.size() - offset, result);

    result += extension;
    return result;
}

album_art_storage storage_of(const album_art_row& row)
{
    // An image in the row is the image to read, whatever the hash says.
    if (row.album_art.has_value())
        return album_art_storage::in_database;

    if (std::holds_alternative<album_art_binary_hash>(row.hash))
        return album_art_storage::as_file;

    if (std::holds_alternative<album_art_uri>(row.hash))
        return album_art_storage::elsewhere;

    // A text hash or a null one, with no image beside it: the row at id 1
    // that tracks without art point at, or a row whose image has been
    // converted away.  There is nowhere to look.
    return album_art_storage::none;
}

namespace
{
/// Bind a hash the way it is stored: a binary hash as a BLOB, a text hash
/// or a URI as TEXT, a null hash as null.  It is the type of the member
/// that picks the binding, not a branch here: `std::vector<std::byte>`
/// binds a BLOB and `std::string` binds TEXT.  SQLite keeps the two apart
/// and so does Engine, which is the whole reason the column's shapes are
/// separate types here.
void bind_hash(sqlite::database_binder& query, const album_art_hash& hash)
{
    std::visit(
        [&query](const auto& held)
        {
            using T = std::decay_t<decltype(held)>;
            if constexpr (std::is_same_v<T, std::monostate>)
                query << held;
            else
                query << held.value;
        },
        hash);
}

/// An empty binary hash cannot be stored and cannot be read back.
///
/// SQLite binds a zero-length blob from a null data pointer, which is what
/// an empty `std::vector` gives, and treats that as a null column: the row
/// would come back with a null hash instead of the empty one written, and
/// `find_id()` would never match it again.  It also names no file, only a
/// bare extension.  Refuse it rather than silently storing something else.
void reject_unstorable_hash(const album_art_hash& hash)
{
    const auto* binary = std::get_if<album_art_binary_hash>(&hash);
    if (binary != nullptr && binary->value.empty())
    {
        throw std::invalid_argument{
            "An empty binary album art hash cannot be stored, because SQLite "
            "cannot tell a zero-length blob from a null column"};
    }
}
}  // anonymous namespace

album_art_table::album_art_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

int64_t album_art_table::add(const album_art_row& row)
{
    if (row.id != ALBUM_ART_ROW_ID_NONE)
    {
        throw album_art_row_id_error{
            "The provided album art row already pertains to a persisted album "
            "art entry, and so it cannot be created again"};
    }

    reject_unstorable_hash(row.hash);

    auto query = context_->db
                 << "INSERT INTO AlbumArt (hash, albumArt) VALUES (?, ?)";
    bind_hash(query, row.hash);
    query << row.album_art;
    query.execute();

    return context_->db.last_insert_rowid();
}

std::vector<int64_t> album_art_table::all_ids() const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM AlbumArt" >> [&](int64_t id)
    { results.push_back(id); };
    return results;
}

bool album_art_table::exists(int64_t id) const
{
    bool result = false;
    context_->db << "SELECT COUNT(*) FROM AlbumArt WHERE id = ?" << id >>
        [&](int64_t count) { result = count > 0; };
    return result;
}

std::optional<int64_t> album_art_table::find_id(
    const album_art_hash& hash) const
{
    std::optional<int64_t> result;

    // `hash = NULL` is never true, not even of a null column, so a null
    // hash has to be asked for by its own spelling.
    if (std::holds_alternative<std::monostate>(hash))
    {
        context_->db << "SELECT id FROM AlbumArt WHERE hash IS NULL LIMIT 1" >>
            [&](int64_t id) { result = id; };
        return result;
    }

    auto query = context_->db
                 << "SELECT id FROM AlbumArt WHERE hash = ? LIMIT 1";
    bind_hash(query, hash);
    query >> [&](int64_t id) { result = id; };
    return result;
}

std::optional<album_art_row> album_art_table::get(int64_t id) const
{
    std::optional<album_art_row> result;
    // The hash comes back as whichever SQLite storage class it was written
    // with, which is what tells a binary hash from a text one.
    context_->db << "SELECT id, hash, albumArt FROM AlbumArt WHERE id = ?"
                 << id >>
        [&](int64_t row_id,
            std::variant<std::monostate, std::string, std::vector<std::byte>>
                hash,
            std::optional<std::vector<std::byte>> album_art)
    {
        result = album_art_row{
            row_id, hash_from_column(std::move(hash)), std::move(album_art)};
    };
    return result;
}

void album_art_table::remove(int64_t id)
{
    context_->db << "DELETE FROM AlbumArt WHERE id = ?" << id;
}

void album_art_table::update(const album_art_row& row)
{
    if (row.id == ALBUM_ART_ROW_ID_NONE)
    {
        throw album_art_row_id_error{
            "The provided album art row does not pertain to a persisted album "
            "art entry, and so it cannot be updated"};
    }

    reject_unstorable_hash(row.hash);

    auto query = context_->db
                 << "UPDATE AlbumArt SET hash = ?, albumArt = ? WHERE id = ?";
    bind_hash(query, row.hash);
    query << row.album_art << row.id;
    query.execute();
}

}  // namespace djinterop::engine::v2
