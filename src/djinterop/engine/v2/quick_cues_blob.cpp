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

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
std::vector<char> quick_cues_blob::to_blob() const
{
    auto total_label_length = std::accumulate(
        quick_cues.begin(), quick_cues.end(), int64_t{0},
        [](int64_t x, const stdx::optional<quick_cue_blob>& quick_cue)
        { return x + (quick_cue ? quick_cue->label.length() : 0); });

    // Work out total length of all cue labels.
    std::vector<char> uncompressed(
        25 + (13 * quick_cues.size()) + total_label_length);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_int64_be(static_cast<int64_t>(quick_cues.size()), ptr);

    for (auto& quick_cue : quick_cues)
    {
        if (quick_cue)
        {
            if (quick_cue->label.length() == 0)
            {
                throw std::invalid_argument{"Hot cue labels must not be empty"};
            }
            ptr = encode_uint8(quick_cue->label.length(), ptr);
            for (auto& chr : quick_cue->label)
            {
                ptr = encode_uint8(static_cast<uint8_t>(chr), ptr);
            }
            ptr = encode_double_be(quick_cue->sample_offset, ptr);
            ptr = encode_uint8(quick_cue->color.a, ptr);
            ptr = encode_uint8(quick_cue->color.r, ptr);
            ptr = encode_uint8(quick_cue->color.g, ptr);
            ptr = encode_uint8(quick_cue->color.b, ptr);
        }
        else
        {
            ptr = encode_uint8(0, ptr);
            ptr = encode_double_be(-1, ptr);
            for (int i = 0; i < 4; ++i)
            {
                ptr = encode_uint8(0, ptr);
            }
        }
    }

    ptr = encode_double_be(adjusted_main_cue, ptr);
    ptr = encode_uint8(is_main_cue_adjusted, ptr);
    ptr = encode_double_be(default_main_cue, ptr);
    assert(ptr == end);

    return zlib_compress(uncompressed);
}

quick_cues_blob quick_cues_blob::from_blob(const std::vector<char>& blob)
{
    const auto uncompressed = zlib_uncompress(blob);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    if (uncompressed.size() < 129)
    {
        throw std::invalid_argument{
            "Quick cues data has less than the minimum length of 129 bytes"};
    }

    {
        // Check that there are exactly 8 loops
        int64_t num_hot_cues;
        std::tie(num_hot_cues, ptr) = decode_int64_be(ptr);
        if (num_hot_cues != 8)
        {
            throw std::invalid_argument{
                "Quick cues data has an unsupported number of cues"};
        }
    }

    quick_cues_blob result;
    for (auto& quick_cue : result.quick_cues)
    {
        uint8_t label_length;
        std::tie(label_length, ptr) = decode_uint8(ptr);
        if (end - ptr < 29 + label_length)  // 12 (here) + 17 (after the loop)
        {
            throw std::invalid_argument{
                "Quick cues data has quick cue with missing data"};
        }
        if (label_length > 0)
        {
            quick_cue.emplace();
            quick_cue->label.assign(ptr, label_length);
            ptr += label_length;
            std::tie(quick_cue->sample_offset, ptr) = decode_double_be(ptr);
            std::tie(quick_cue->color.a, ptr) = decode_uint8(ptr);
            std::tie(quick_cue->color.r, ptr) = decode_uint8(ptr);
            std::tie(quick_cue->color.g, ptr) = decode_uint8(ptr);
            std::tie(quick_cue->color.b, ptr) = decode_uint8(ptr);
        }
        else
        {
            ptr += 12;
        }
    }

    std::tie(result.adjusted_main_cue, ptr) = decode_double_be(ptr);
    std::tie(result.is_main_cue_adjusted, ptr) = decode_uint8(ptr);
    std::tie(result.default_main_cue, ptr) = decode_double_be(ptr);
    assert(ptr == end);

    return result;
}

}  // namespace djinterop::engine::v2
