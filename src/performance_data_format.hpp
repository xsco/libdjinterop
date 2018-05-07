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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_PERFORMANCE_DATA_FORMAT_HPP
#define ENGINEPRIME_PERFORMANCE_DATA_FORMAT_HPP

#include <vector>
#include "engineprime/performance_data.hpp"

namespace engineprime {

struct track_data_blob
{
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
    double sample_rate;
    int64_t total_samples;
    int8_t is_beat_data_set;
    int64_t default_num_beatgrid_markers;
    std::vector<beat_data_marker_blob> default_markers;
    int64_t adjusted_num_beatgrid_markers;
    std::vector<beat_data_marker_blob> adjusted_markers;
};

struct quick_cues_blob
{
    std::vector<track_hot_cue_point> hot_cues;
    double adjusted_main_cue_sample_offset;
    bool is_main_cue_adjusted_from_default;
    double default_main_cue_sample_offset;
};

struct loops_blob
{
    std::vector<track_loop> loops;
};


// Extract track data from a blob
void extract_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data,
        track_data_blob &track_data);

// Extract beat data from a blob
void extract_beat_data(
        int track_id,
        const std::vector<char> &compressed_beat_data,
        beat_data_blob &beat_data);

// Extract quick cues data from a blob
void extract_quick_cues(
        int track_id,
        const std::vector<char> &compressed_quick_cues_data,
        quick_cues_blob &quick_cues);

// Extract loops from a blob
void extract_loops(
        int track_id,
        const std::vector<char> &loops_data,
        loops_blob &loops);


} // namespace engineprime

#endif // ENGINEPRIME_PERFORMANCE_DATA_HPP

