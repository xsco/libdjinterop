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
#include <stdexcept>

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
namespace
{
std::byte* encode_beatgrid(
    const std::vector<beat_grid_marker_blob>& beat_grid, std::byte* ptr)
{
    ptr = encode_int64_be(static_cast<int64_t>(beat_grid.size()), ptr);
    for (auto&& marker : beat_grid)
    {
        ptr = encode_double_le(marker.sample_offset, ptr);
        ptr = encode_int64_le(marker.beat_number, ptr);
        ptr = encode_int32_le(marker.number_of_beats, ptr);
        ptr = encode_int32_le(marker.unknown_value_1, ptr);
    }

    return ptr;
}

std::pair<std::vector<beat_grid_marker_blob>, const std::byte*> decode_beatgrid(
    const std::byte* ptr, const std::byte* end)
{
    int64_t count;
    std::tie(count, ptr) = decode_int64_be(ptr);
    std::vector<beat_grid_marker_blob> result;

    if (end - ptr < 24 * count)
    {
        throw std::invalid_argument{"Beat data grid is missing data"};
    }

    for (auto i = 0; i < count; ++i)
    {
        beat_grid_marker_blob marker;
        std::tie(marker.sample_offset, ptr) = decode_double_le(ptr);
        std::tie(marker.beat_number, ptr) = decode_int64_le(ptr);
        std::tie(marker.number_of_beats, ptr) = decode_int32_le(ptr);
        std::tie(marker.unknown_value_1, ptr) = decode_int32_le(ptr);
        result.push_back(marker);
    }

    return {std::move(result), ptr};
}
}  // anonymous namespace

std::vector<std::byte> beat_data_blob::to_blob() const
{
    std::vector<std::byte> uncompressed(
        33 + 24 * (default_beat_grid.size() + adjusted_beat_grid.size()) +
        extra_data.size());
    auto ptr = uncompressed.data();
    [[maybe_unused]] const auto end = ptr + uncompressed.size();

    ptr = encode_double_be(sample_rate, ptr);
    ptr = encode_double_be(samples, ptr);
    ptr = encode_uint8(is_beatgrid_set, ptr);
    ptr = encode_beatgrid(default_beat_grid, ptr);
    ptr = encode_beatgrid(adjusted_beat_grid, ptr);
    ptr = encode_extra(extra_data, ptr);
    assert(ptr == end);

    return zlib_compress(uncompressed);
}

beat_data_blob beat_data_blob::from_blob(const std::vector<std::byte>& blob)
{
    if (blob.empty())
    {
        return beat_data_blob{
            .sample_rate = 0,
            .samples = 0,
            .is_beatgrid_set = false,
            .default_beat_grid = {},
            .adjusted_beat_grid = {},
            .extra_data = {},
        };
    }

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
    std::tie(result.default_beat_grid, ptr) = decode_beatgrid(ptr, end);
    std::tie(result.adjusted_beat_grid, ptr) = decode_beatgrid(ptr, end);

    std::tie(result.extra_data, ptr) = decode_extra(ptr, end);

    assert(ptr == end);

    return result;
}
}  // namespace djinterop::engine::v2
