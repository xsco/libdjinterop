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

#include <iomanip>
#include <numeric>
#include <vector>

#include <djinterop/musical_key.hpp>
#include <djinterop/optional.hpp>

#include "../../util/convert.hpp"
#include "djinterop/engine/encode_decode_utils.hpp"
#include "performance_data_format.hpp"

namespace djinterop::engine::v1
{
namespace
{
template <typename T, typename U>
stdx::optional<std::decay_t<T> > prohibit(const U& sentinel, T&& data)
{
    if (data == sentinel)
    {
        return stdx::nullopt;
    }
    return stdx::make_optional(std::forward<T>(data));
}

std::byte* encode_beatgrid(const std::vector<beatgrid_marker>& beatgrid, std::byte* ptr)
{
    typedef std::vector<beatgrid_marker>::size_type vec_size_t;
    ptr = encode_int64_be(beatgrid.size(), ptr);
    for (vec_size_t i = 0; i < beatgrid.size(); ++i)
    {
        ptr = encode_double_le(beatgrid[i].sample_offset, ptr);
        ptr = encode_int64_le(beatgrid[i].index, ptr);
        int32_t diff = 0;
        if (i < beatgrid.size() - 1)
        {
            diff = beatgrid[i + 1].index - beatgrid[i].index;
        }
        ptr = encode_int32_le(diff, ptr);
        ptr = encode_int32_le(0, ptr);  // unknown field
    }
    return ptr;
}

std::pair<std::vector<beatgrid_marker>, const std::byte*> decode_beatgrid(
    const std::byte* ptr, const std::byte* end)
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
            "Beat data grid has unsupportedly many markers"};
    }
    if (end - ptr < 24 * count)
    {
        throw std::invalid_argument{"Beat data grid is missing data"};
    }
    std::vector<beatgrid_marker> result(count);
    int32_t beats_until_next_marker;
    typedef std::vector<beatgrid_marker>::size_type vec_size_t;
    for (vec_size_t i = 0; i < result.size(); ++i)
    {
        std::tie(result[i].sample_offset, ptr) = decode_double_le(ptr);
        std::tie(result[i].index, ptr) = decode_int64_le(ptr);
        if (i != 0)
        {
            if (result[i].index <= result[i - 1].index)
            {
                throw std::invalid_argument{
                    "Beat data grid has unsorted indices"};
            }
            if (result[i].sample_offset <= result[i - 1].sample_offset)
            {
                throw std::invalid_argument{
                    "Beat data grid has unsorted sample offsets"};
            }
            if (result[i].index - result[i - 1].index !=
                beats_until_next_marker)
            {
                throw std::invalid_argument{
                    "Beat data grid has conflicting markers"};
            }
        }
        std::tie(beats_until_next_marker, ptr) = decode_int32_le(ptr);
        std::tie(std::ignore, ptr) = decode_int32_le(ptr);  // unknown field
    }
    if (beats_until_next_marker != 0)
    {
        throw std::invalid_argument{
            "Beat data grid promised non-existent marker"};
    }
    return {std::move(result), ptr};
}

}  // namespace

// Encode beat data into a byte array
std::vector<std::byte> beat_data::encode() const
{
    std::vector<std::byte> uncompressed(
        33 + 24 * (default_beatgrid.size() + adjusted_beatgrid.size()));
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_double_be(sample_rate.value_or(0), ptr);
    ptr = encode_double_be(sample_count.value_or(0), ptr);
    ptr = encode_uint8(1, ptr);
    ptr = encode_beatgrid(default_beatgrid, ptr);
    ptr = encode_beatgrid(adjusted_beatgrid, ptr);

    if (ptr != end)
    {
        throw std::runtime_error{"Internal error in beat_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return zlib_compress(uncompressed);
}

// Extract beat data from a byte array
beat_data beat_data::decode(const std::vector<std::byte>& compressed_data)
{
    const auto raw_data = zlib_uncompress(compressed_data);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 33)
    {
        throw std::invalid_argument{
            "Beat data has less than the minimum length of 33 bytes"};
    }

    beat_data result;

    double sample_rate;
    double sample_count;
    std::tie(sample_rate, ptr) = decode_double_be(ptr);
    std::tie(sample_count, ptr) = decode_double_be(ptr);
    result.sample_rate =
        sample_rate != 0 ? stdx::make_optional(sample_rate) : stdx::nullopt;
    result.sample_count =
        sample_count != 0 ? stdx::make_optional(sample_count) : stdx::nullopt;

    uint8_t is_beat_data_set;
    std::tie(is_beat_data_set, ptr) = decode_uint8(ptr);
    if (is_beat_data_set != 1)
    {
        // TODO (haslersn): print a warning that "Is beat data set" is not 1
    }

    try
    {
        std::vector<beatgrid_marker> default_beatgrid;
        std::vector<beatgrid_marker> adjusted_beatgrid;
        std::tie(default_beatgrid, ptr) = decode_beatgrid(ptr, end);
        std::tie(adjusted_beatgrid, ptr) = decode_beatgrid(ptr, end);
        // If there's an exception, then the following will intentionally not be
        // executed.
        result.default_beatgrid = std::move(default_beatgrid);
        result.adjusted_beatgrid = std::move(adjusted_beatgrid);
    }
    catch (const std::invalid_argument& e)
    {
        // TODO (haslersn): print a warning with e.what().
    }

    // Beat data has known to be encoded with 9 additional zero bytes at the
    // end of the data buffer, across various Engine Library-supporting software
    // and hardware.  The precise reason for this is unknown, but it is
    // tolerated here in accordance with the robustness principle.
    while (ptr != end)
    {
        if (std::to_integer<int>(*ptr) != 0)
        {
            throw std::invalid_argument{"Beat data has trailing non-zero data"};
        }

        ptr++;
    }

    return result;
}

// Encode high-resolution waveform data into a byte array
std::vector<std::byte> high_res_waveform_data::encode() const
{
    std::vector<std::byte> uncompressed(30 + 6 * waveform.size());
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_int64_be(waveform.size(), ptr);
    ptr = encode_int64_be(waveform.size(), ptr);
    ptr = encode_double_be(samples_per_entry, ptr);

    uint8_t max_low = 0;
    uint8_t max_mid = 0;
    uint8_t max_high = 0;
    uint8_t max_low_opc = 0;
    uint8_t max_mid_opc = 0;
    uint8_t max_high_opc = 0;
    for (auto& entry : waveform)
    {
        max_low = std::max(max_low, entry.low.value);
        max_mid = std::max(max_mid, entry.mid.value);
        max_high = std::max(max_high, entry.high.value);
        max_low_opc = std::max(max_low_opc, entry.low.opacity);
        max_mid_opc = std::max(max_mid_opc, entry.mid.opacity);
        max_high_opc = std::max(max_high_opc, entry.high.opacity);
        ptr = encode_uint8(entry.low.value, ptr);
        ptr = encode_uint8(entry.mid.value, ptr);
        ptr = encode_uint8(entry.high.value, ptr);
        ptr = encode_uint8(entry.low.opacity, ptr);
        ptr = encode_uint8(entry.mid.opacity, ptr);
        ptr = encode_uint8(entry.high.opacity, ptr);
    }

    // Encode the maximum values across all entries
    ptr = encode_uint8(max_low, ptr);
    ptr = encode_uint8(max_mid, ptr);
    ptr = encode_uint8(max_high, ptr);
    ptr = encode_uint8(max_low_opc, ptr);
    ptr = encode_uint8(max_mid_opc, ptr);
    ptr = encode_uint8(max_high_opc, ptr);

    if (ptr != end)
    {
        throw std::runtime_error{
            "Internal error in high_res_waveform_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return zlib_compress(uncompressed);
}

// Extract high-resolution waveform from a byte array
high_res_waveform_data high_res_waveform_data::decode(
    const std::vector<std::byte>& compressed_data)
{
    const auto raw_data = zlib_uncompress(compressed_data);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 30)
    {
        throw std::invalid_argument{
            "High-resolution waveform data has less than the minimum length of "
            "30 bytes"};
    }

    // Work out how many entries we have
    high_res_waveform_data result;
    int64_t num_entries_1, num_entries_2;
    std::tie(num_entries_1, ptr) = decode_int64_be(ptr);
    std::tie(num_entries_2, ptr) = decode_int64_be(ptr);
    std::tie(result.samples_per_entry, ptr) = decode_double_be(ptr);

    if (num_entries_1 != num_entries_2)
    {
        throw std::invalid_argument{
            "High-resolution waveform data has conflicting length fields"};
    }

    if (end - ptr != 6 * (num_entries_1 + 1))
    {
        throw std::invalid_argument{
            "High-resolution waveform data has incorrect length"};
    }

    result.waveform.resize(num_entries_1);

    for (auto& entry : result.waveform)
    {
        std::tie(entry.low.value, ptr) = decode_uint8(ptr);
        std::tie(entry.mid.value, ptr) = decode_uint8(ptr);
        std::tie(entry.high.value, ptr) = decode_uint8(ptr);
        std::tie(entry.low.opacity, ptr) = decode_uint8(ptr);
        std::tie(entry.mid.opacity, ptr) = decode_uint8(ptr);
        std::tie(entry.high.opacity, ptr) = decode_uint8(ptr);
    }

    // Ignore additional entry at the end
    ptr += 6;

    if (ptr != end)
    {
        throw std::runtime_error{
            "Internal error in high_res_waveform_data::decode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return result;
}

// Encode loops into a byte array
std::vector<std::byte> loops_data::encode() const
{
    auto total_label_length = std::accumulate(
        loops.begin(), loops.end(), int64_t{0},
        [](int64_t x, const stdx::optional<loop>& loop) {
            return x + (loop ? loop->label.length() : 0);
        });

    std::vector<std::byte> uncompressed(
        8 + (23 * loops.size()) + total_label_length);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_int64_le(loops.size(), ptr);  // 8

    for (auto& loop : loops)
    {
        if (loop)
        {
            if (loop->label.length() == 0)
            {
                throw std::logic_error{"Loop labels must not be empty"};
            }
            ptr = encode_uint8(loop->label.length(), ptr);
            for (auto& chr : loop->label)
            {
                ptr = encode_uint8(static_cast<uint8_t>(chr), ptr);
            }
            ptr = encode_double_le(loop->start_sample_offset, ptr);
            ptr = encode_double_le(loop->end_sample_offset, ptr);
            ptr = encode_uint8(1, ptr);
            ptr = encode_uint8(1, ptr);
            ptr = encode_uint8(loop->color.a, ptr);
            ptr = encode_uint8(loop->color.r, ptr);
            ptr = encode_uint8(loop->color.g, ptr);
            ptr = encode_uint8(loop->color.b, ptr);
        }
        else
        {
            ptr = encode_uint8(0, ptr);
            ptr = encode_double_le(-1, ptr);
            ptr = encode_double_le(-1, ptr);
            for (int i = 0; i < 6; ++i)
            {
                ptr = encode_uint8(0, ptr);
            }
        }
    }

    if (ptr != end)
    {
        throw std::runtime_error{"Internal error in loops_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    // Note that 'loops' is not compressed
    return uncompressed;
}

// Extract loops from a byte array
loops_data loops_data::decode(const std::vector<std::byte>& raw_data)
{
    // Note that loops are not compressed, unlike all the other fields
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 8)
    {
        throw std::invalid_argument{
            "Loops data has less than the minimum length of 8 bytes"};
    }

    int64_t num_loops;
    std::tie(num_loops, ptr) = decode_int64_le(ptr);

    loops_data result;
    result.loops.reserve(num_loops);
    for (auto i = 0; i < num_loops; ++i)
    {
        loop loop;

        uint8_t label_length;
        std::tie(label_length, ptr) = decode_uint8(ptr);
        if (end - ptr < 22 + label_length)
        {
            throw std::invalid_argument{"Loop data has loop with missing data"};
        }

        if (label_length > 0)
        {
            loop.label.assign(reinterpret_cast<const char*>(ptr), label_length);
            ptr += label_length;
        }

        std::tie(loop.start_sample_offset, ptr) = decode_double_le(ptr);
        std::tie(loop.end_sample_offset, ptr) = decode_double_le(ptr);
        uint8_t is_start_set;
        uint8_t is_end_set;
        std::tie(is_start_set, ptr) = decode_uint8(ptr);
        std::tie(is_end_set, ptr) = decode_uint8(ptr);
        std::tie(loop.color.a, ptr) = decode_uint8(ptr);
        std::tie(loop.color.r, ptr) = decode_uint8(ptr);
        std::tie(loop.color.g, ptr) = decode_uint8(ptr);
        std::tie(loop.color.b, ptr) = decode_uint8(ptr);

        if (loop.start_sample_offset != -1)
            result.loops.emplace_back(loop);
        else
            result.loops.emplace_back(stdx::nullopt);
    }

    if (ptr != end)
    {
        throw std::invalid_argument{"Loops data has too much data"};
    }

    return result;
}

// Encode overview waveform data into a byte array
std::vector<std::byte> overview_waveform_data::encode() const
{
    std::vector<std::byte> uncompressed(27 + 3 * waveform.size());
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    // Encode
    ptr = encode_int64_be(waveform.size(), ptr);
    ptr = encode_int64_be(waveform.size(), ptr);
    ptr = encode_double_be(samples_per_entry, ptr);

    uint8_t max_low = 0;
    uint8_t max_mid = 0;
    uint8_t max_high = 0;
    for (auto& entry : waveform)
    {
        max_low = std::max(max_low, entry.low.value);
        max_mid = std::max(max_mid, entry.mid.value);
        max_high = std::max(max_high, entry.high.value);
        ptr = encode_uint8(entry.low.value, ptr);
        ptr = encode_uint8(entry.mid.value, ptr);
        ptr = encode_uint8(entry.high.value, ptr);
    }

    // Encode the maximum values across all entries
    ptr = encode_uint8(max_low, ptr);
    ptr = encode_uint8(max_mid, ptr);
    ptr = encode_uint8(max_high, ptr);

    if (ptr != end)
    {
        throw std::runtime_error{
            "Internal error in overview_waveform_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return zlib_compress(uncompressed);
}

// Extract overview waveform from a byte array
overview_waveform_data overview_waveform_data::decode(
    const std::vector<std::byte>& compressed_data)
{
    const auto raw_data = zlib_uncompress(compressed_data);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 27)
    {
        throw std::invalid_argument{
            "Overview waveform data has less than the minimum length of "
            "27 bytes"};
    }

    // Work out how many entries we have
    overview_waveform_data result;
    int64_t num_entries_1, num_entries_2;
    std::tie(num_entries_1, ptr) = decode_int64_be(ptr);
    std::tie(num_entries_2, ptr) = decode_int64_be(ptr);
    std::tie(result.samples_per_entry, ptr) = decode_double_be(ptr);

    if (num_entries_1 != num_entries_2)
    {
        throw std::invalid_argument{
            "High-resolution waveform data has conflicting length fields"};
    }

    if (end - ptr != 3 * (num_entries_1 + 1))
    {
        throw std::invalid_argument{
            "High-resolution waveform data has incorrect length"};
    }

    result.waveform.resize(num_entries_1);

    for (auto& entry : result.waveform)
    {
        std::tie(entry.low.value, ptr) = decode_uint8(ptr);
        std::tie(entry.mid.value, ptr) = decode_uint8(ptr);
        std::tie(entry.high.value, ptr) = decode_uint8(ptr);
    }

    // Ignore additional entry at the end
    ptr += 3;

    if (ptr != end)
    {
        throw std::runtime_error{
            "Internal error in overview_waveform_data::decode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return result;
}

// Encode quick cues data into a byte array
std::vector<std::byte> quick_cues_data::encode() const
{
    auto total_label_length = std::accumulate(
        hot_cues.begin(), hot_cues.end(), int64_t{0},
        [](int64_t x, const stdx::optional<hot_cue>& hot_cue) {
            return x + (hot_cue ? hot_cue->label.length() : 0);
        });

    // Work out total length of all cue labels
    std::vector<std::byte> uncompressed(129 + total_label_length);
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_int64_be(hot_cues.size(), ptr);

    for (auto& hot_cue : hot_cues)
    {
        if (hot_cue)
        {
            if (hot_cue->label.length() == 0)
            {
                throw std::invalid_argument{"Hot cue labels must not be empty"};
            }
            ptr = encode_uint8(hot_cue->label.length(), ptr);
            for (auto& chr : hot_cue->label)
            {
                ptr = encode_uint8(static_cast<uint8_t>(chr), ptr);
            }
            ptr = encode_double_be(hot_cue->sample_offset, ptr);
            ptr = encode_uint8(hot_cue->color.a, ptr);
            ptr = encode_uint8(hot_cue->color.r, ptr);
            ptr = encode_uint8(hot_cue->color.g, ptr);
            ptr = encode_uint8(hot_cue->color.b, ptr);
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
    ptr = encode_uint8(adjusted_main_cue == default_main_cue ? 0 : 1, ptr);
    ptr = encode_double_be(default_main_cue, ptr);

    if (ptr != end)
    {
        throw std::runtime_error{"Internal error in quick_cues_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return zlib_compress(uncompressed);
}

// Extract quick cues data from a byte array
quick_cues_data quick_cues_data::decode(
    const std::vector<std::byte>& compressed_data)
{
    const auto raw_data = zlib_uncompress(compressed_data);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() < 25)
    {
        throw std::invalid_argument{
            "Quick cues data has less than the minimum length of 25 bytes"};
    }

    int64_t num_hot_cues;
    std::tie(num_hot_cues, ptr) = decode_int64_be(ptr);

    quick_cues_data result;
    result.hot_cues.reserve(num_hot_cues);
    for (auto i = 0; i < num_hot_cues; ++i)
    {
        hot_cue quick_cue;

        uint8_t label_length;
        std::tie(label_length, ptr) = decode_uint8(ptr);
        if (end - ptr < 29 + label_length)  // 12 (here) + 17 (after the loop)
        {
            throw std::invalid_argument{
                "Quick cues data has quick cue with missing data"};
        }

        if (label_length > 0)
        {
            quick_cue.label.assign(reinterpret_cast<const char*>(ptr), label_length);
            ptr += label_length;
        }

        std::tie(quick_cue.sample_offset, ptr) = decode_double_be(ptr);
        std::tie(quick_cue.color.a, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.r, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.g, ptr) = decode_uint8(ptr);
        std::tie(quick_cue.color.b, ptr) = decode_uint8(ptr);

        if (quick_cue.sample_offset != -1)
            result.hot_cues.emplace_back(quick_cue);
        else
            result.hot_cues.emplace_back(stdx::nullopt);
    }

    std::tie(result.adjusted_main_cue, ptr) = decode_double_be(ptr);
    uint8_t is_adjusted;
    std::tie(is_adjusted, ptr) = decode_uint8(ptr);
    std::tie(result.default_main_cue, ptr) = decode_double_be(ptr);

    if (is_adjusted > 1 || (is_adjusted == 0 && result.adjusted_main_cue !=
                                                    result.default_main_cue))
    {
        // TODO (haslersn): This is not fatal. Make this a warning.
        throw std::invalid_argument{
            "Quick cues data has invalid main cue data"};
    }

    if (ptr != end)
    {
        throw std::invalid_argument{"Quick cues data has too much data"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return result;
}

// Encode track data into a byte array
std::vector<std::byte> track_data::encode() const
{
    std::vector<std::byte> uncompressed(28);  // Track data has fixed size
    auto ptr = uncompressed.data();
    const auto end = ptr + uncompressed.size();

    ptr = encode_double_be(sample_rate.value_or(0), ptr);
    ptr = encode_int64_be(sample_count.value_or(0), ptr);
    ptr = encode_double_be(average_loudness.value_or(0), ptr);
    ptr = encode_int32_be(key ? static_cast<int32_t>(*key) : 0, ptr);

    if (ptr != end)
    {
        throw std::runtime_error{"Internal error in track_data::encode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return zlib_compress(uncompressed);
}

// Extract track data from a byte array
track_data track_data::decode(const std::vector<std::byte>& compressed_track_data)
{
    const auto raw_data = zlib_uncompress(compressed_track_data);
    auto ptr = raw_data.data();
    const auto end = ptr + raw_data.size();

    if (raw_data.size() != 28)
    {
        throw std::invalid_argument{
            "Track data doesn't have expected length of 28 bytes"};
    }

    track_data result;

    double sample_rate;
    int64_t sample_count;
    std::tie(sample_rate, ptr) = decode_double_be(ptr);
    std::tie(sample_count, ptr) = decode_int64_be(ptr);
    result.sample_rate =
        sample_rate != 0 ? stdx::make_optional(sample_rate) : stdx::nullopt;
    result.sample_count =
        sample_count != 0 ? stdx::make_optional(sample_count) : stdx::nullopt;

    double raw_average_loudness;
    std::tie(raw_average_loudness, ptr) = decode_double_be(ptr);
    result.average_loudness = prohibit(0, raw_average_loudness);

    int32_t raw_key;
    std::tie(raw_key, ptr) = decode_int32_be(ptr);
    result.key = util::optional_static_cast<musical_key>(prohibit(0, raw_key));

    if (ptr != end)
    {
        throw std::runtime_error{"Internal error in track_data::decode()"};
        // TODO (haslersn): This shouldn't be possible to happen. How to handle?
    }

    return result;
}

}  // namespace djinterop::engine::v1
