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
#include <utility>

#include "../engine_library_context.hpp"

namespace djinterop::engine::v2
{
namespace
{
constexpr char base64url_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

void encode_base64url_group(
    const std::byte* group, std::size_t length, std::string& result)
{
    uint_fast32_t bits = 0;
    for (std::size_t i = 0; i < 3; ++i)
    {
        bits <<= 8;
        if (i < length)
            bits |= std::to_integer<uint_fast32_t>(group[i]);
    }

    // Three bytes make four characters; a partial group makes one character
    // fewer than it has bytes, and is not padded.
    for (std::size_t i = 0; i < length + 1; ++i)
        result += base64url_alphabet[(bits >> (18 - 6 * i)) & 0x3F];
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

    context_->db << "INSERT INTO AlbumArt (hash, albumArt) VALUES (?, ?)"
                 << row.hash << row.album_art;

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
    const std::vector<std::byte>& hash) const
{
    std::optional<int64_t> result;
    context_->db << "SELECT id FROM AlbumArt WHERE hash = ?" << hash >>
        [&](int64_t id) { result = id; };
    return result;
}

std::optional<album_art_row> album_art_table::get(int64_t id) const
{
    std::optional<album_art_row> result;
    context_->db << "SELECT id, hash, albumArt FROM AlbumArt WHERE id = ?"
                 << id >>
        [&](int64_t row_id, std::vector<std::byte> hash,
            std::optional<std::vector<std::byte>> album_art)
    { result = album_art_row{row_id, std::move(hash), std::move(album_art)}; };
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

    context_->db << "UPDATE AlbumArt SET hash = ?, albumArt = ? WHERE id = ?"
                 << row.hash << row.album_art << row.id;
}

}  // namespace djinterop::engine::v2
