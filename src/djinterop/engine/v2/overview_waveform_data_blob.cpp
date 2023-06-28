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

#include <djinterop/engine/v2/overview_waveform_data_blob.hpp>

#include <cassert>
#include <stdexcept>

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
std::vector<std::byte> overview_waveform_data_blob::to_blob() const
{
    std::vector<std::byte> uncompressed(
        27 + (3 * waveform_points.size()) + extra_data.size());
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    // Encode
    auto waveform_size = static_cast<int64_t>(waveform_points.size());
    ptr = encode_int64_be(waveform_size, ptr);
    ptr = encode_int64_be(waveform_size, ptr);
    ptr = encode_double_be(samples_per_waveform_point, ptr);

    for (auto& entry : waveform_points)
    {
        ptr = encode_uint8(entry.low_value, ptr);
        ptr = encode_uint8(entry.mid_value, ptr);
        ptr = encode_uint8(entry.high_value, ptr);
    }

    ptr = encode_uint8(maximum_point.low_value, ptr);
    ptr = encode_uint8(maximum_point.mid_value, ptr);
    ptr = encode_uint8(maximum_point.high_value, ptr);
    ptr = encode_extra(extra_data, ptr);
    assert(ptr == end);

    return zlib_compress(uncompressed);
}

overview_waveform_data_blob overview_waveform_data_blob::from_blob(
    const std::vector<std::byte>& blob)
{
    const auto raw_data = zlib_uncompress(blob);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 27)
    {
        throw std::invalid_argument{
            "Overview waveform data has less than the minimum length of "
            "27 bytes"};
    }

    // Work out how many entries we have
    overview_waveform_data_blob result;
    int64_t num_entries_1, num_entries_2;
    std::tie(num_entries_1, ptr) = decode_int64_be(ptr);
    std::tie(num_entries_2, ptr) = decode_int64_be(ptr);
    std::tie(result.samples_per_waveform_point, ptr) = decode_double_be(ptr);

    if (num_entries_1 != num_entries_2)
    {
        throw std::invalid_argument{
            "Overview waveform data has conflicting length fields"};
    }

    if (end - ptr != 3 * (num_entries_1 + 1))
    {
        throw std::invalid_argument{
            "Overview waveform data has incorrect length"};
    }

    result.waveform_points.resize(num_entries_1);

    for (auto& entry : result.waveform_points)
    {
        std::tie(entry.low_value, ptr) = decode_uint8(ptr);
        std::tie(entry.mid_value, ptr) = decode_uint8(ptr);
        std::tie(entry.high_value, ptr) = decode_uint8(ptr);
    }

    std::tie(result.maximum_point.low_value, ptr) = decode_uint8(ptr);
    std::tie(result.maximum_point.mid_value, ptr) = decode_uint8(ptr);
    std::tie(result.maximum_point.high_value, ptr) = decode_uint8(ptr);
    std::tie(result.extra_data, ptr) = decode_extra(ptr, end);
    assert(ptr == end);

    return result;
}

}  // namespace djinterop::engine::v2
