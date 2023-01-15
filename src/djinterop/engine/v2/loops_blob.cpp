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

#include <djinterop/engine/v2/loops_blob.hpp>

#include <cassert>
#include <numeric>
#include <stdexcept>

#include "../encode_decode_utils.hpp"

namespace djinterop::engine::v2
{
std::vector<char> loops_blob::to_blob() const
{
    auto total_label_length = std::accumulate(
        loops.begin(), loops.end(), int64_t{0},
        [](int64_t x, const stdx::optional<loop_blob>& loop)
        { return x + (loop ? loop->label.length() : 0); });

    std::vector<char> uncompressed(
        8 + (23 * loops.size()) + total_label_length);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_int64_le(static_cast<int64_t>(loops.size()), ptr);  // 8

    for (auto& loop : loops)
    {
        ptr = encode_uint8(loop.label.length(), ptr);
        for (auto& chr : loop.label)
        {
            ptr = encode_uint8(static_cast<uint8_t>(chr), ptr);
        }
        ptr = encode_double_le(loop.start_sample_offset, ptr);
        ptr = encode_double_le(loop.end_sample_offset, ptr);
        ptr = encode_uint8(loop.is_start_set, ptr);
        ptr = encode_uint8(loop.is_end_set, ptr);
        ptr = encode_uint8(loop.color.a, ptr);
        ptr = encode_uint8(loop.color.r, ptr);
        ptr = encode_uint8(loop.color.g, ptr);
        ptr = encode_uint8(loop.color.b, ptr);
    }

    assert(ptr == end);

    // Note that the loops blob is not compressed.
    return uncompressed;
}

loops_blob loops_blob::from_blob(const std::vector<char>& blob)
{
    // Note that loops are not compressed, unlike all the other fields.
    auto ptr = blob.data();
    const auto end = ptr + blob.size();

    if (blob.size() < 8)
    {
        throw std::invalid_argument{
            "Loops data has less than the minimum length of 8 bytes"};
    }

    int64_t num_loops;
    std::tie(num_loops, ptr) = decode_int64_le(ptr);

    loops_blob result;
    for (auto i = 0; i < num_loops; ++i)
    {
        loop_blob loop;

        uint8_t label_length;
        std::tie(label_length, ptr) = decode_uint8(ptr);
        if (end - ptr < 22 + label_length)
        {
            throw std::invalid_argument{"Loop data has loop with missing data"};
        }

        if (label_length > 0)
        {
            loop.label.assign(ptr, label_length);
            ptr += label_length;
        }

        std::tie(loop.start_sample_offset, ptr) = decode_double_le(ptr);
        std::tie(loop.end_sample_offset, ptr) = decode_double_le(ptr);
        std::tie(loop.is_start_set, ptr) = decode_uint8(ptr);
        std::tie(loop.is_end_set, ptr) = decode_uint8(ptr);
        std::tie(loop.color.a, ptr) = decode_uint8(ptr);
        std::tie(loop.color.r, ptr) = decode_uint8(ptr);
        std::tie(loop.color.g, ptr) = decode_uint8(ptr);
        std::tie(loop.color.b, ptr) = decode_uint8(ptr);

        result.loops.push_back(loop);
    }

    assert(ptr == end);

    return result;
}

}  // namespace djinterop::engine::v2
