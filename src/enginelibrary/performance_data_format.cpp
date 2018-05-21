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

#include "performance_data_format.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "encode_decode_utils.hpp"

namespace io = boost::iostreams;

namespace djinterop {
namespace enginelibrary {

// Uncompress a zlib'ed BLOB
static void uncompress(
        int track_id,
        const std::vector<char> &compressed,
        std::vector<char> &uncompressed)
{
    if (compressed.size() == 0)
        // No data, which is a valid situation
        return;

    if (compressed.size() < 4)
        throw corrupt_performance_data(
                track_id,
                "Compressed data is less than the minimum size of 4 bytes");

    auto apparent_size = decode_int32_be(compressed.data());
    if (apparent_size == 0)
        // No data (explicitly so!), which is a valid situation
        return;

    uncompressed.clear();
    uncompressed.reserve(apparent_size);

    io::filtering_istream src;
    src.push(io::zlib_decompressor{});
    src.push(io::basic_array_source<char>{
            &compressed[4], compressed.size() - 4});
    io::copy(src, io::back_inserter(uncompressed));
}

// Compress a byte array using zlib
static std::vector<char> compress(const std::vector<char> &uncompressed)
{
    std::vector<char> compressed;

    // Put a placeholder four bytes to hold the uncompressed buffer size
    compressed.resize(4);
    encode_int32_be(uncompressed.size(), compressed.data());

    // Compress
    io::filtering_istream src;
    src.push(io::zlib_compressor{});
    src.push(io::basic_array_source<char>{
            uncompressed.data(), uncompressed.size()});
    io::copy(src, io::back_inserter(compressed));
    return compressed;
}

// Extract track data from a blob
track_data_blob decode_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_track_data, raw_data);
    auto ptr = raw_data.data();

    if (raw_data.size() == 0)
    {
        // No track data
        return track_data_blob{};
    }

    // track_data should always be 28 bytes long
    if (raw_data.size() != 28)
        throw corrupt_performance_data{
            track_id, "Track data is not expected length of 28 bytes"};

    // Extract values from raw data
    return track_data_blob{
        decode_double_be(ptr     ),
        decode_int64_be (ptr + 8 ),
        decode_double_be(ptr + 16),
        decode_int32_be (ptr + 24)};
}

// Extract beat data from a blob
beat_data_blob decode_beat_data(
        int track_id,
        const std::vector<char> &compressed_beat_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_beat_data, raw_data);
    auto ptr = raw_data.data();

    if (raw_data.size() == 0)
    {
        // No data
        return beat_data_blob{};
    }

    // Minimum length of 129 bytes
    if (raw_data.size() < 129)
        throw corrupt_performance_data{
            track_id,
            "Beat data is less than minimum expected length of 129 bytes"};

    // Extract fixed-location values from raw data
    beat_data_blob beat_data{
        decode_double_be(ptr     ),
        decode_int64_be (ptr + 8 ),
        decode_int8     (ptr + 16)};

    // Default beat grid
    auto default_num_beatgrid_markers = decode_int64_be(ptr + 17);
    if (default_num_beatgrid_markers < 2)
        throw corrupt_performance_data{
            track_id,
            "There are fewer than 2 default beat grid markers, which is "
            "the minimum expected"};
    beat_data.default_markers.resize(default_num_beatgrid_markers);
    auto marker_ptr = ptr + 25;
    for (auto &marker : beat_data.default_markers)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        marker.sample_offset           = decode_double_le(marker_ptr     );
        marker.beat_index              = decode_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = decode_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = decode_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }

    // Adjusted beat grid
    marker_ptr = ptr + 25 + 8 + (default_num_beatgrid_markers * 24);
    auto adjusted_num_beatgrid_markers = decode_int64_be(marker_ptr - 8);
    if (adjusted_num_beatgrid_markers < 2)
        throw corrupt_performance_data{
            track_id,
            "There are fewer than 2 adjusted beat grid markers, which is "
            "the minimum expected"};
    beat_data.adjusted_markers.resize(adjusted_num_beatgrid_markers);
    for (auto &marker : beat_data.adjusted_markers)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        marker.sample_offset           = decode_double_le(marker_ptr     );
        marker.beat_index              = decode_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = decode_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = decode_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }

    return beat_data;
}

// Extract quick cues data from a blob
quick_cues_blob decode_quick_cues(
        int track_id,
        const std::vector<char> &compressed_quick_cues_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_quick_cues_data, raw_data);
    auto ptr = raw_data.data();

    if (raw_data.size() == 0)
    {
        // No data, which is a valid scenario
        return quick_cues_blob{};
    }
    else if (raw_data.size() < 8)
    {
        throw corrupt_performance_data{
            track_id,
            "Quick cue data is not long enough to contain the number of "
            "quick cues, held in the first 8 bytes"};
    }

    // Work out how many quick cues we have, and check minimum data length
    auto num_quick_cues = decode_int64_be(ptr);
    if (raw_data.size() < 25 + (num_quick_cues * 13))
        throw corrupt_performance_data{
            track_id,
            "Quick cue data is less than the absolute minimum expected size of "
            "13 bytes per cue point and 25 bytes additionally"};

    quick_cues_blob quick_cues;
    quick_cues.hot_cues.resize(num_quick_cues);
    auto cue_ptr = ptr + 8;
    for (int i = 0; i < num_quick_cues; ++i)
    {
        // Get label length, and check minimum data length
        auto label_length = decode_int8(cue_ptr);
        if (raw_data.size() < (cue_ptr - ptr) + label_length + 13)
            throw corrupt_performance_data{track_id};
        if (label_length < 0)
            throw corrupt_performance_data{track_id};

        ++cue_ptr;
        if (label_length > 0)
        {
            quick_cues.hot_cues[i].label =
                std::string{cue_ptr, cue_ptr + label_length};
            cue_ptr += label_length;
        }
        quick_cues.hot_cues[i].sample_offset = decode_double_be(cue_ptr);
        cue_ptr += 8;
        quick_cues.hot_cues[i].is_set =
            quick_cues.hot_cues[i].sample_offset != -1;
        quick_cues.hot_cues[i].colour.a = decode_int8(cue_ptr);
        quick_cues.hot_cues[i].colour.r = decode_int8(cue_ptr + 1);
        quick_cues.hot_cues[i].colour.g = decode_int8(cue_ptr + 2);
        quick_cues.hot_cues[i].colour.b = decode_int8(cue_ptr + 3);
        cue_ptr += 4;
    }

    // Check length, and get cue positions
    if (raw_data.size() < (cue_ptr - ptr) + 17)
        throw corrupt_performance_data{track_id};
    quick_cues.adjusted_main_cue_sample_offset = decode_double_be(cue_ptr);
    cue_ptr += 8;
    quick_cues.is_main_cue_adjusted_from_default = decode_int8(cue_ptr);
    ++cue_ptr;
    quick_cues.default_main_cue_sample_offset = decode_double_be(cue_ptr);

    return quick_cues;
}

// Extract loops from a blob
loops_blob decode_loops(
        int track_id,
        const std::vector<char> &loops_data)
{
    // Note that loops are not compressed, unlike all the other fields
    auto &raw_data = loops_data;
    auto ptr = loops_data.data();

    if (raw_data.size() == 0)
    {
        // No data, which is a valid scenario
        return loops_blob{};
    }
    else if (raw_data.size() < 8)
    {
        throw corrupt_performance_data{
            track_id,
            "Loops data is not long enough to contain the number of "
            "loops, held in the first 8 bytes"};
    }

    // Check how many loops there are, and minimum data length
    auto num_loops = decode_int64_le(ptr);
    if (raw_data.size() < 8 + (num_loops * 23))
        throw corrupt_performance_data{
            track_id,
            "Loops data is less than the absolute minimum size of 23 bytes per "
            "loop and 8 bytes additionally"};

    loops_blob loops;
    loops.loops.resize(num_loops);
    auto loop_ptr = ptr + 8;
    for (int i = 0; i < num_loops; ++i)
    {
        // Get label length, and check minimum data length
        auto label_length = decode_int8(loop_ptr);
        if (raw_data.size() < (loop_ptr - ptr) + label_length + 23)
            throw corrupt_performance_data{track_id};
        if (label_length < 0)
            throw corrupt_performance_data{track_id};

        ++loop_ptr;
        if (label_length > 0)
        {
            loops.loops[i].label =
                std::string{loop_ptr, loop_ptr + label_length};
            loop_ptr += label_length;
        }
        loops.loops[i].start_sample_offset = decode_double_le(loop_ptr + 0);
        loops.loops[i].end_sample_offset   = decode_double_le(loop_ptr + 8);
        loops.loops[i].is_start_set        = decode_int8(loop_ptr      + 16);
        loops.loops[i].is_end_set          = decode_int8(loop_ptr      + 17);
        loops.loops[i].colour.a            = decode_int8(loop_ptr      + 18);
        loops.loops[i].colour.r            = decode_int8(loop_ptr      + 19);
        loops.loops[i].colour.g            = decode_int8(loop_ptr      + 20);
        loops.loops[i].colour.b            = decode_int8(loop_ptr      + 21);
        loop_ptr += 22;
    }

    return loops;
}

// Encode track data into a blob
std::vector<char> encode_track_data(const track_data_blob &track_data)
{
    if (track_data.sample_rate == 0 || track_data.total_samples == 0)
        // No meaningful information to encode
        return std::vector<char>{};

    // Track data is always a fixed size
    std::vector<char> uncompressed{};
    uncompressed.resize(28);
    auto ptr = uncompressed.data();

    encode_double_be(track_data.sample_rate,      ptr + 0);
    encode_int64_be (track_data.total_samples,    ptr + 8);
    encode_double_be(track_data.average_loudness, ptr + 16);
    encode_int32_be ((int32_t)track_data.key,     ptr + 24);

    return compress(uncompressed);
}

// Encode beat data into a blob
std::vector<char> encode_beat_data(const beat_data_blob &beat_data)
{
    if (beat_data.sample_rate == 0 || beat_data.total_samples == 0)
        // No meaningful information to encode
        return std::vector<char>{};

    std::vector<char> uncompressed{};
    uncompressed.resize(
            33 +
            24 * (
                beat_data.default_markers.size() +
                beat_data.adjusted_markers.size()));
    auto ptr = uncompressed.data();

    encode_double_be(beat_data.sample_rate,   ptr + 0);
    encode_double_be(beat_data.total_samples, ptr + 8);
    encode_int8     (1,                       ptr + 16);
    ptr += 17;

    encode_int64_be (beat_data.default_markers.size(), ptr + 0);
    ptr += 8;
    for (auto &marker : beat_data.default_markers)
    {
        encode_double_le(marker.sample_offset,           ptr + 0);
        encode_int64_le (marker.beat_index,              ptr + 8);
        encode_int32_le (marker.beats_until_next_marker, ptr + 16);
        encode_int32_le (marker.unknown_field_1,         ptr + 20);
        ptr += 24;
    }

    encode_int64_be (beat_data.adjusted_markers.size(), ptr + 0);
    ptr += 8;
    for (auto &marker : beat_data.adjusted_markers)
    {
        encode_double_le(marker.sample_offset,           ptr + 0);
        encode_int64_le (marker.beat_index,              ptr + 8);
        encode_int32_le (marker.beats_until_next_marker, ptr + 16);
        encode_int32_le (marker.unknown_field_1,         ptr + 20);
        ptr += 24;
    }

    return compress(uncompressed);
}

// Encode quick cues data into a blob
std::vector<char> encode_quick_cues(const quick_cues_blob &quick_cues)
{
    if (quick_cues.hot_cues.size() == 0)
        // No meaningful information to encode
        return std::vector<char>{};

    // Work out total length of all cue labels
    auto total_label_length = 0;
    for (auto &hot_cue : quick_cues.hot_cues)
        if (hot_cue.is_set)
            total_label_length += hot_cue.label.length();
    std::vector<char> uncompressed{};
    uncompressed.resize(
            25 +
            (quick_cues.hot_cues.size() * 13) +
            total_label_length);
    auto ptr = uncompressed.data();

    encode_int64_be(quick_cues.hot_cues.size(), ptr);
    ptr += 8;
    for (auto &hot_cue : quick_cues.hot_cues)
    {
        if (hot_cue.is_set)
        {
            auto label_len = hot_cue.label.length();
            encode_int8(label_len, ptr++);
            for (auto &label_char : hot_cue.label)
                encode_int8(label_char, ptr++);
        }
        else
        {
            encode_int8(0, ptr++);
        }

        encode_double_be(hot_cue.sample_offset, ptr + 0);
        encode_int8     (hot_cue.colour.a,      ptr + 8);
        encode_int8     (hot_cue.colour.r,      ptr + 9);
        encode_int8     (hot_cue.colour.g,      ptr + 10);
        encode_int8     (hot_cue.colour.b,      ptr + 11);
        ptr += 12;
    }

    encode_double_be(quick_cues.adjusted_main_cue_sample_offset,   ptr + 0);
    encode_int8     (quick_cues.is_main_cue_adjusted_from_default, ptr + 8);
    encode_double_be(quick_cues.default_main_cue_sample_offset,   ptr + 9);

    return compress(uncompressed);
}

// Encode loops into a blob
std::vector<char> encode_loops(const loops_blob &loops)
{
    if (loops.loops.size() == 0)
        // No meaningful information to encode
        return std::vector<char>{};

    // Work out total length of all loop labels
    auto total_label_length = 0;
    for (auto &loop : loops.loops)
        if (loop.is_set())
            total_label_length += loop.label.length();
    std::vector<char> uncompressed{};
    uncompressed.resize(
            8 +
            (loops.loops.size() * 23) +
            total_label_length);
    auto ptr = uncompressed.data();

    encode_int64_le(loops.loops.size(), ptr);
    ptr += 8;
    for (auto &loop : loops.loops)
    {
        if (loop.is_set())
        {
            auto label_len = loop.label.length();
            encode_int8(label_len, ptr++);
            for (auto &label_char : loop.label)
                encode_int8(label_char, ptr++);
        }
        else
        {
            encode_int8(0, ptr++);
        }

        encode_double_le(loop.start_sample_offset, ptr + 0);
        encode_double_le(loop.end_sample_offset,   ptr + 8);
        encode_int8     (loop.is_start_set,        ptr + 16);
        encode_int8     (loop.is_end_set,          ptr + 17);
        encode_int8     (loop.colour.a,            ptr + 18);
        encode_int8     (loop.colour.r,            ptr + 19);
        encode_int8     (loop.colour.g,            ptr + 20);
        encode_int8     (loop.colour.b,            ptr + 21);
        ptr += 22;
    }

    // Note that loops is not compressed
    return uncompressed;
}

} // enginelibrary
} // djinterop
