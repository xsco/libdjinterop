/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <performance_data_format.hpp>

#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

namespace io = boost::iostreams;

namespace engineprime {

// Uncompress a zlib'ed BLOB
static void uncompress(
        int track_id,
        const std::vector<char> &compressed,
        std::vector<char> &uncompressed)
{
    if (compressed.size() < 4)
        throw corrupt_performance_data(track_id);

    auto apparent_size = *reinterpret_cast<const uint32_t *>(compressed.data());
    if (apparent_size == 0)
        throw corrupt_performance_data(track_id);

    uncompressed.clear();
    uncompressed.reserve(apparent_size);

    io::filtering_ostream out;
    out.push(io::zlib_decompressor{});
    out.push(io::back_inserter(uncompressed));
    io::write(out, &compressed[4], compressed.size() - 4);
}

// Extract an int8_t from a raw value at ptr address
static int32_t extract_int8(const char *ptr)
{
    return static_cast<const unsigned char>(*ptr);
}

// Extract an int32_t from a little-endian encoded raw value at ptr address
static int32_t extract_int32_le(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 4; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*(ptr + 3 - i));
    }
    return val;
}

// Extract an int32_t from a big-endian encoded raw value at ptr address
static int32_t extract_int32_be(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 4; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*ptr++);
    }
    return val;
}

// Extract an int64_t from a little-endian encoded raw value at ptr address
static int64_t extract_int64_le(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 8; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*(ptr + 7 - i));
    }
    return val;
}

// Extract an int64_t from a big-endian encoded raw value at ptr address
static int64_t extract_int64_be(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 8; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*ptr++);
    }
    return val;
}

// Extract a double from a little-endian encoded raw value at ptr address
static double extract_double_le(const char *ptr)
{
    auto val = extract_int64_le(ptr);
    return *reinterpret_cast<double *>(&val);
}

// Extract a double from a big-endian encoded raw value at ptr address
static double extract_double_be(const char *ptr)
{
    auto val = extract_int64_be(ptr);
    return *reinterpret_cast<double *>(&val);
}

// Extract track data from a blob
void extract_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data,
        track_data_blob &track_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_track_data, raw_data);
    auto ptr = raw_data.data();

    // track_data should always be 28 bytes long
    if (raw_data.size() != 28)
        throw corrupt_performance_data{track_id};

    // Extract values from raw data
    track_data.sample_rate      = extract_double_be(ptr     );
    track_data.total_samples    = extract_int64_be (ptr + 8 );
    track_data.average_loudness = extract_double_be(ptr + 16);
    track_data.key              = extract_int32_be (ptr + 24);
}

// Extract beat data from a blob
void extract_beat_data(
        int track_id,
        const std::vector<char> &compressed_beat_data,
        beat_data_blob &beat_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_beat_data, raw_data);
    auto ptr = raw_data.data();

    // Minimum length of 129 bytes
    if (raw_data.size() < 129)
        throw corrupt_performance_data{track_id};

    // Extract fixed-location values from raw data
    beat_data.sample_rate      = extract_double_be(ptr     );
    beat_data.total_samples    = extract_int64_be (ptr + 8 );
    beat_data.is_beat_data_set = extract_int8     (ptr + 16);

    // Default beat grid
    beat_data.default_num_beatgrid_markers = extract_int64_be(ptr + 17);
    if (beat_data.default_num_beatgrid_markers < 2)
        throw corrupt_performance_data{track_id};
    beat_data.default_markers.resize(beat_data.default_num_beatgrid_markers);
    auto marker_ptr = ptr + 25;
    for (int i = 0; i < beat_data.default_num_beatgrid_markers; ++i)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        auto &marker = beat_data.default_markers[i];
        marker.sample_offset           = extract_double_le(marker_ptr     );
        marker.beat_index              = extract_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = extract_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = extract_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }

    // Adjusted beat grid
    marker_ptr = ptr + 25 + 8 + (beat_data.default_num_beatgrid_markers * 24);
    beat_data.adjusted_num_beatgrid_markers = extract_int64_be(marker_ptr - 8);
    if (beat_data.adjusted_num_beatgrid_markers < 2)
        throw corrupt_performance_data{track_id};
    beat_data.adjusted_markers.resize(beat_data.adjusted_num_beatgrid_markers);
    for (int i = 0; i < beat_data.adjusted_num_beatgrid_markers; ++i)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        auto &marker = beat_data.adjusted_markers[i];
        marker.sample_offset           = extract_double_le(marker_ptr     );
        marker.beat_index              = extract_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = extract_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = extract_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }
}

// Extract quick cues data from a blob
void extract_quick_cues(
        int track_id,
        const std::vector<char> &compressed_quick_cues_data,
        quick_cues_blob &quick_cues)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_quick_cues_data, raw_data);
    auto ptr = raw_data.data();

    // Work out how many quick cues we have, and check minimum data length
    auto num_quick_cues = extract_int64_be(ptr);
    if (raw_data.size() < 25 + (num_quick_cues * 13))
        throw corrupt_performance_data{track_id};

    quick_cues.hot_cues.resize(num_quick_cues);
    auto cue_ptr = ptr + 8;
    for (int i = 0; i < num_quick_cues; ++i)
    {
        // Get label length, and check minimum data length
        auto label_length = extract_int8(cue_ptr);
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
        quick_cues.hot_cues[i].sample_offset = extract_double_be(cue_ptr);
        cue_ptr += 8;
        quick_cues.hot_cues[i].is_set =
            quick_cues.hot_cues[i].sample_offset != -1;
        quick_cues.hot_cues[i].colour.a = extract_int8(cue_ptr);
        quick_cues.hot_cues[i].colour.r = extract_int8(cue_ptr + 1);
        quick_cues.hot_cues[i].colour.g = extract_int8(cue_ptr + 2);
        quick_cues.hot_cues[i].colour.b = extract_int8(cue_ptr + 3);
        cue_ptr += 4;
    }

    // Check length, and get cue positions
    if (raw_data.size() < (cue_ptr - ptr) + 17)
        throw corrupt_performance_data{track_id};
    quick_cues.adjusted_main_cue_sample_offset = extract_double_be(cue_ptr);
    cue_ptr += 8;
    quick_cues.is_main_cue_adjusted_from_default = extract_int8(cue_ptr);
    ++cue_ptr;
    quick_cues.default_main_cue_sample_offset = extract_double_be(cue_ptr);
}

// Extract loops from a blob
void extract_loops(
        int track_id,
        const std::vector<char> &loops_data,
        loops_blob &loops)
{
    // Note that loops are not compressed, unlike all the other fields
    auto &raw_data = loops_data;
    auto ptr = loops_data.data();

    // Check how many loops there are, and minimum data length
    auto num_loops = extract_int64_le(ptr);
    if (raw_data.size() < 8 + (num_loops * 23))
        throw corrupt_performance_data{track_id};

    loops.loops.resize(num_loops);
    auto loop_ptr = ptr + 8;
    for (int i = 0; i < num_loops; ++i)
    {
        // Get label length, and check minimum data length
        auto label_length = extract_int8(loop_ptr);
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
        loops.loops[i].start_sample_offset = extract_double_le(loop_ptr + 0);
        loops.loops[i].end_sample_offset   = extract_double_le(loop_ptr + 8);
        loops.loops[i].is_start_set        = extract_int8(loop_ptr      + 16);
        loops.loops[i].is_end_set          = extract_int8(loop_ptr      + 17);
        loops.loops[i].colour.a            = extract_int8(loop_ptr      + 18);
        loops.loops[i].colour.r            = extract_int8(loop_ptr      + 19);
        loops.loops[i].colour.g            = extract_int8(loop_ptr      + 20);
        loops.loops[i].colour.b            = extract_int8(loop_ptr      + 21);
        loop_ptr += 22;
    }
}

} // namespace engineprime
