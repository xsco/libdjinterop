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

#include <djinterop/engine/v2/beat_data_blob.hpp>

#include <cassert>

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
namespace
{
char* encode_beatgrid(
    const std::vector<beat_grid_marker_blob>& beatgrid, char* ptr)
{
    typedef std::vector<beat_grid_marker_blob>::size_type vec_size_t;
    ptr = encode_int64_be(static_cast<int64_t>(beatgrid.size()), ptr);
    for (vec_size_t i = 0; i < beatgrid.size(); ++i)
    {
        ptr = encode_double_le(beatgrid[i].sample_offset, ptr);
        ptr = encode_int64_le(beatgrid[i].beat_number, ptr);
        int32_t diff = 0;
        if (i < beatgrid.size() - 1)
        {
            diff = static_cast<int32_t>(
                beatgrid[i + 1].beat_number - beatgrid[i].beat_number);
        }
        ptr = encode_int32_le(diff, ptr);
        ptr = encode_int32_le(0, ptr);  // unknown field
    }
    return ptr;
}

std::pair<std::vector<beat_grid_marker_blob>, const char*> decode_beatgrid(
    const char* ptr, const char* end)
{
    int64_t count;
    std::tie(count, ptr) = decode_int64_be(ptr);

    if (count == 0)
    {
        return {{}, ptr};
    }
    if (count < 2)
    {
        throw std::invalid_argument{
            "Beat data grid has an invalid number of markers"};
    }
    if (count > 32768)
    {
        throw std::invalid_argument{
            "Beat data grid has more markers than is supported"};
    }
    if (end - ptr < 24 * count)
    {
        throw std::invalid_argument{"Beat data grid is missing data"};
    }

    std::vector<beat_grid_marker_blob> result(count);
    int32_t beats_until_next_marker;
    typedef std::vector<beat_grid_marker_blob>::size_type vec_size_t;
    for (vec_size_t i = 0; i < result.size(); ++i)
    {
        std::tie(result[i].sample_offset, ptr) = decode_double_le(ptr);
        std::tie(result[i].beat_number, ptr) = decode_int64_le(ptr);
        if (i != 0)
        {
            if (result[i].beat_number <= result[i - 1].beat_number)
            {
                throw std::invalid_argument{
                    "Beat data grid has unsorted indices"};
            }
            if (result[i].sample_offset <= result[i - 1].sample_offset)
            {
                throw std::invalid_argument{
                    "Beat data grid has unsorted sample offsets"};
            }
            if (result[i].beat_number - result[i - 1].beat_number !=
                beats_until_next_marker)
            {
                throw std::invalid_argument{
                    "Beat data grid has conflicting markers"};
            }
        }
        std::tie(result[i].number_of_beats, ptr) = decode_int32_le(ptr);
        std::tie(result[i].unknown_value_1, ptr) = decode_int32_le(ptr);

        beats_until_next_marker = result[i].number_of_beats;
    }

    if (beats_until_next_marker != 0)
    {
        throw std::invalid_argument{
            "Beat data grid promised non-existent marker"};
    }

    return {std::move(result), ptr};
}

}  // anonymous namespace

std::vector<char> beat_data_blob::to_blob() const
{
    std::vector<char> uncompressed(
        33 + 24 * (default_beat_grid.size() + adjusted_beat_grid.size()));
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_double_be(sample_rate, ptr);
    ptr = encode_double_be(samples, ptr);
    ptr = encode_uint8(is_beatgrid_set, ptr);
    ptr = encode_beatgrid(default_beat_grid, ptr);
    ptr = encode_beatgrid(adjusted_beat_grid, ptr);
    assert(ptr == end);

    return zlib_compress(uncompressed);
}

beat_data_blob beat_data_blob::from_blob(const std::vector<char>& blob)
{
    const auto raw_data = zlib_uncompress(blob);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 33)
    {
        throw std::invalid_argument{
            "Beat data has less than the minimum length of 33 bytes"};
    }

    beat_data_blob result;

    std::tie(result.sample_rate, ptr) = decode_double_be(ptr);
    std::tie(result.samples, ptr) = decode_double_be(ptr);
    std::tie(result.is_beatgrid_set, ptr) = decode_uint8(ptr);

    std::vector<beat_grid_marker_blob> default_beatgrid;
    std::vector<beat_grid_marker_blob> adjusted_beatgrid;
    std::tie(result.default_beat_grid, ptr) = decode_beatgrid(ptr, end);
    std::tie(result.adjusted_beat_grid, ptr) = decode_beatgrid(ptr, end);

    // Beat data has known to be encoded with 9 additional zero bytes at the
    // end of the data buffer, across various Engine Library-supporting software
    // and hardware.  The precise reason for this is unknown, but it is
    // tolerated here in accordance with the robustness principle.
    while (ptr != end)
    {
        if (*ptr != 0)
        {
            throw std::invalid_argument{"Beat data has trailing non-zero data"};
        }

        ptr++;
    }

    assert(ptr == end);

    return result;
}

}  // namespace djinterop::engine::v2
