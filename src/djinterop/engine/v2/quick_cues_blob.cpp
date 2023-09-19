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

#include <djinterop/engine/v2/quick_cues_blob.hpp>

#include <cassert>
#include <numeric>
#include <stdexcept>

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
std::vector<std::byte> quick_cues_blob::to_blob() const
{
    // Work out total length of all cue labels in order to size the buffer.
    auto total_label_length = std::accumulate(
        quick_cues.begin(), quick_cues.end(), int64_t{0},
        [](int64_t x, const quick_cue_blob& quick_cue)
        { return x + quick_cue.label.length(); });

    std::vector<std::byte> uncompressed(
        25 + (13 * quick_cues.size()) + total_label_length + extra_data.size());
    auto ptr = uncompressed.data();
    [[maybe_unused]] const auto end = ptr + uncompressed.size();

    ptr = encode_int64_be(static_cast<int64_t>(quick_cues.size()), ptr);

    for (auto& quick_cue : quick_cues)
    {
        ptr = encode_uint8(quick_cue.label.length(), ptr);
        for (auto& chr : quick_cue.label)
        {
            ptr = encode_uint8(static_cast<uint8_t>(chr), ptr);
        }
        ptr = encode_double_be(quick_cue.sample_offset, ptr);
        ptr = encode_uint8(quick_cue.color.a, ptr);
        ptr = encode_uint8(quick_cue.color.r, ptr);
        ptr = encode_uint8(quick_cue.color.g, ptr);
        ptr = encode_uint8(quick_cue.color.b, ptr);
    }

    ptr = encode_double_be(adjusted_main_cue, ptr);
    ptr = encode_uint8(is_main_cue_adjusted, ptr);
    ptr = encode_double_be(default_main_cue, ptr);
    ptr = encode_extra(extra_data, ptr);
    assert(ptr == end);

    return zlib_compress(uncompressed);
}

quick_cues_blob quick_cues_blob::from_blob(const std::vector<std::byte>& blob)
{
    const auto uncompressed = zlib_uncompress(blob);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    if (uncompressed.size() < 25)
    {
        throw std::invalid_argument{
            "Quick cues data has less than the minimum length of 25 bytes"};
    }

    int64_t num_hot_cues;
    std::tie(num_hot_cues, ptr) = decode_int64_be(ptr);

    quick_cues_blob result;
    result.quick_cues.reserve(num_hot_cues);
    for (auto i = 0; i < num_hot_cues; ++i)
    {
        quick_cue_blob quick_cue;

        uint8_t label_length;
        std::tie(label_length, ptr) = decode_uint8(ptr);
        if (end - ptr < 29 + label_length)  // 12 (here) + 17 (after the loop)
        {
            throw std::invalid_argument{
                "Quick cues data has quick cue with missing data"};
        }

        if (label_length > 0)
        {
            quick_cue.label.assign(
                reinterpret_cast<const char*>(ptr), label_length);
            ptr += label_length;
        }

        std::tie(quick_cue.sample_offset, ptr) = decode_double_be(ptr);
        std::tie(quick_cue.color.a, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.r, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.g, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.b, ptr) = decode_uint8(ptr);

        result.quick_cues.push_back(quick_cue);
    }

    std::tie(result.adjusted_main_cue, ptr) = decode_double_be(ptr);
    std::tie(result.is_main_cue_adjusted, ptr) = decode_uint8(ptr);
    std::tie(result.default_main_cue, ptr) = decode_double_be(ptr);
    std::tie(result.extra_data, ptr) = decode_extra(ptr, end);
    assert(ptr == end);

    return result;
}

}  // namespace djinterop::engine::v2
