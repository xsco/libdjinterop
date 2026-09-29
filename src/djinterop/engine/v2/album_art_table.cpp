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

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
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

/// Encode up to three bytes as base64url characters, without padding.
///
/// Base64 works on 24-bit groups: the bytes are packed into one integer, most
/// significant byte first, and read back out six bits at a time.  A short
/// group yields one character more than it has bytes.
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

/// Classify a `hash` column by the storage class SQLite reports for it.
album_art_hash hash_from_column(
    std::variant<std::monostate, std::string, std::vector<std::byte>> value)
{
    if (std::holds_alternative<std::monostate>(value))
        return std::monostate{};

    if (const auto* bytes = std::get_if<std::vector<std::byte>>(&value))
    {
        album_art_binary_hash hash{};
        if (bytes->size() != hash.value.size())
        {
            throw std::runtime_error{
                "Album art hash is a blob of " + std::to_string(bytes->size()) +
                " bytes, not " + std::to_string(hash.value.size())};
        }

        std::copy(bytes->begin(), bytes->end(), hash.value.begin());
        return hash;
    }

    auto text = std::get<std::string>(std::move(value));
    if (text.rfind(ALBUM_ART_URI_PREFIX, 0) == 0)
        return album_art_uri{std::move(text)};

    return album_art_text_hash{std::move(text)};
}
}  // anonymous namespace

std::string album_art_file_name(
    const album_art_binary_hash& hash, const std::string& extension)
{
    const auto& bytes = hash.value;
    std::string result;
    // Four characters per started group of three bytes, which over-reserves
    // by one for the final, unpadded group.
    result.reserve((bytes.size() + 2) / 3 * 4 + extension.length());

    std::size_t offset = 0;
    for (; offset + 3 <= bytes.size(); offset += 3)
        encode_base64url_group(bytes.data() + offset, 3, result);

    if (offset < bytes.size())
        encode_base64url_group(
            bytes.data() + offset, bytes.size() - offset, result);

    result += extension;
    return result;
}

album_art_storage storage_of(const album_art_row& row)
{
    if (!row.album_art.empty())
        return album_art_storage::blob_in_database;

    if (std::holds_alternative<album_art_binary_hash>(row.hash))
        return album_art_storage::file_in_artwork_dir;

    if (std::holds_alternative<album_art_uri>(row.hash))
        return album_art_storage::uri;

    return album_art_storage::none;
}

namespace
{
/// Bind a hash with the storage class it is read back by: a binary hash as a
/// BLOB, a text hash or URI as TEXT, and a null hash as NULL.
void bind_hash(sqlite::database_binder& query, const album_art_hash& hash)
{
    std::visit(
        [&query](const auto& held)
        {
            using T = std::decay_t<decltype(held)>;
            if constexpr (std::is_same_v<T, std::monostate>)
                query << held;
            else if constexpr (std::is_same_v<T, album_art_binary_hash>)
                query << std::vector<std::byte>{
                    held.value.begin(), held.value.end()};
            else
                query << held.value;
        },
        hash);
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

    // `hash = NULL` is never true, so a null hash needs `IS NULL`.
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
    context_->db << "SELECT id, hash, albumArt FROM AlbumArt WHERE id = ?"
                 << id >>
        [&](int64_t row_id,
            std::variant<std::monostate, std::string, std::vector<std::byte>>
                hash,
            std::optional<std::vector<std::byte>> album_art)
    {
        result = album_art_row{
            row_id, hash_from_column(std::move(hash)),
            album_art.value_or(std::vector<std::byte>{})};
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

    auto query = context_->db
                 << "UPDATE AlbumArt SET hash = ?, albumArt = ? WHERE id = ?";
    bind_hash(query, row.hash);
    query << row.album_art << row.id;
    query.execute();
}

}  // namespace djinterop::engine::v2
