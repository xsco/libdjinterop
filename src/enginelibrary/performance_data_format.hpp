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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP
#define DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP

#include <cstdint>
#include <vector>
#include "djinterop/enginelibrary/performance_data.hpp"

namespace djinterop {
namespace enginelibrary {

struct track_data_blob
{
    track_data_blob() :
        sample_rate{0},
        total_samples{0},
        average_loudness{0.5},
        key{0}
    {}

    track_data_blob(
            double sample_rate, int64_t total_samples,
            double average_loudness, int32_t key) :
        sample_rate{sample_rate},
        total_samples{total_samples},
        average_loudness{average_loudness},
        key{key}
    {}

    double sample_rate;
    int64_t total_samples;
    double average_loudness;
    int32_t key;
};

struct beat_data_marker_blob
{
    double sample_offset;
    int64_t beat_index;
    int32_t beats_until_next_marker;
    int32_t unknown_field_1;
};

struct beat_data_blob
{
    beat_data_blob() :
        sample_rate{0},
        total_samples{0},
        is_beat_data_set{0}
    {}

    beat_data_blob(
            double sample_rate, int64_t total_samples,
            uint8_t is_beat_data_set) :
        sample_rate{sample_rate},
        total_samples{total_samples},
        is_beat_data_set{is_beat_data_set}
    {}

    double sample_rate;
    int64_t total_samples;
    uint8_t is_beat_data_set;
    std::vector<beat_data_marker_blob> default_markers;
    std::vector<beat_data_marker_blob> adjusted_markers;
};

struct quick_cues_blob
{
    quick_cues_blob() :
        adjusted_main_cue_sample_offset{0},
        is_main_cue_adjusted_from_default{false},
        default_main_cue_sample_offset{0}
    {
        hot_cues.resize(8);
    }

    std::vector<track_hot_cue_point> hot_cues;
    double adjusted_main_cue_sample_offset;
    bool is_main_cue_adjusted_from_default;
    double default_main_cue_sample_offset;
};

struct loops_blob
{
    loops_blob()
    {
        loops.resize(8);
    }

    std::vector<track_loop> loops;
};

struct overview_waveform_blob
{
    overview_waveform_blob() :
        num_entries{0},
        samples_per_entry{0}
    {}

    int64_t num_entries;
    double samples_per_entry;
    std::vector<overview_waveform_entry> entry_data;
};

struct high_res_waveform_blob
{
    high_res_waveform_blob() :
        num_entries{0},
        samples_per_entry{0}
    {}

    int64_t num_entries;
    double samples_per_entry;
    std::vector<high_res_waveform_entry> entry_data;
};

// Extract track data from a byte array
track_data_blob decode_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data);

// Extract beat data from a byte array
beat_data_blob decode_beat_data(
        int track_id,
        const std::vector<char> &compressed_beat_data);

// Extract quick cues data from a byte array
quick_cues_blob decode_quick_cues(
        int track_id,
        const std::vector<char> &compressed_quick_cues_data);

// Extract loops from a byte array
loops_blob decode_loops(
        int track_id,
        const std::vector<char> &loops_data);

// Extract overview waveform from a byte array
overview_waveform_blob decode_overview_waveform_data(
        int track_id,
        const std::vector<char> &waveform_data);

// Extract high-resolution waveform from a byte array
high_res_waveform_blob decode_high_res_waveform_data(
        int track_id,
        const std::vector<char> &waveform_data);

// Encode track data into a byte array
std::vector<char> encode_track_data(const track_data_blob &track_data);

// Encode beat data into a byte array
std::vector<char> encode_beat_data(const beat_data_blob &beat_data);

// Encode quick cues data into a byte array
std::vector<char> encode_quick_cues(const quick_cues_blob &quick_cues);

// Encode loops into a byte array
std::vector<char> encode_loops(const loops_blob &loops);

// Encode overview waveform data into a byte array
std::vector<char> encode_overview_waveform_data(
        const overview_waveform_blob &waveform_data);

// Encode high-resolution waveform data into a byte array
std::vector<char> encode_high_res_waveform_data(
        const high_res_waveform_blob &waveform_data);

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP

