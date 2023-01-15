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

#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>

#include <djinterop/engine/engine.hpp>
#include <djinterop/track_snapshot.hpp>

#include "../../boost_test_utils.hpp"

enum class example_track_type
{
    empty,
    minimal_1,
    basic_metadata_only_1,
    fully_analysed_1,
};

inline std::ostream& operator<<(std::ostream& o, example_track_type v)
{
    switch (v)
    {
        case example_track_type::empty: o << "empty"; break;
        case example_track_type::minimal_1: o << "minimal_1"; break;
        case example_track_type::basic_metadata_only_1:
            o << "basic_metadata_only_1";
            break;
        case example_track_type::fully_analysed_1:
            o << "fully_analysed_1";
            break;
        default: o << "unknown"; break;
    }

    return o;
}

inline void populate_track_snapshot(
    example_track_type snapshot_type,
    djinterop::engine::engine_version version, djinterop::track_snapshot& s)
{
    switch (snapshot_type)
    {
        case example_track_type::empty:
            s.adjusted_beatgrid.clear();
            s.adjusted_main_cue = djinterop::stdx::nullopt;
            s.album = djinterop::stdx::nullopt;
            s.artist = djinterop::stdx::nullopt;
            s.average_loudness = djinterop::stdx::nullopt;
            s.bitrate = djinterop::stdx::nullopt;
            s.bpm = djinterop::stdx::nullopt;
            s.comment = djinterop::stdx::nullopt;
            s.composer = djinterop::stdx::nullopt;
            s.default_beatgrid.clear();
            s.default_main_cue = djinterop::stdx::nullopt;
            s.duration = djinterop::stdx::nullopt;
            s.file_bytes = djinterop::stdx::nullopt;
            s.genre = djinterop::stdx::nullopt;
            s.hot_cues.fill(djinterop::stdx::nullopt);
            s.key = djinterop::stdx::nullopt;
            s.last_accessed_at = djinterop::stdx::nullopt;
            s.last_modified_at = djinterop::stdx::nullopt;
            s.last_played_at = djinterop::stdx::nullopt;
            s.loops.fill(djinterop::stdx::nullopt);
            s.publisher = djinterop::stdx::nullopt;
            s.relative_path = djinterop::stdx::nullopt;
            s.sampling = djinterop::stdx::nullopt;
            s.title = djinterop::stdx::nullopt;
            s.track_number = djinterop::stdx::nullopt;
            s.waveform.clear();
            s.year = djinterop::stdx::nullopt;
            break;

        case example_track_type::minimal_1:
            s.adjusted_beatgrid.clear();
            s.adjusted_main_cue = djinterop::stdx::nullopt;
            s.album = djinterop::stdx::nullopt;
            s.artist = djinterop::stdx::nullopt;
            s.average_loudness = djinterop::stdx::nullopt;
            s.bitrate = djinterop::stdx::nullopt;
            s.bpm = djinterop::stdx::nullopt;
            s.comment = djinterop::stdx::nullopt;
            s.composer = djinterop::stdx::nullopt;
            s.default_beatgrid.clear();
            s.default_main_cue = djinterop::stdx::nullopt;
            s.duration = djinterop::stdx::nullopt;
            s.file_bytes = djinterop::stdx::nullopt;
            s.genre = djinterop::stdx::nullopt;
            s.hot_cues.fill(djinterop::stdx::nullopt);
            s.key = djinterop::stdx::nullopt;
            s.last_accessed_at = djinterop::stdx::nullopt;
            s.last_modified_at = djinterop::stdx::nullopt;
            s.last_played_at = djinterop::stdx::nullopt;
            s.loops.fill(djinterop::stdx::nullopt);
            s.publisher = djinterop::stdx::nullopt;
            s.relative_path = "filename.ext";
            s.sampling = djinterop::stdx::nullopt;
            s.title = djinterop::stdx::nullopt;
            s.track_number = djinterop::stdx::nullopt;
            s.waveform.clear();
            s.year = djinterop::stdx::nullopt;
            break;

        case example_track_type::basic_metadata_only_1:
            s.adjusted_beatgrid.clear();
            s.adjusted_main_cue = djinterop::stdx::nullopt;
            s.album = "Some Album";
            s.artist = "Some Artist";
            s.average_loudness = djinterop::stdx::nullopt;
            s.bitrate = 320;
            s.bpm = 123;
            s.comment = "Comment";
            s.composer = "Composer";
            s.default_beatgrid.clear();
            s.default_main_cue = djinterop::stdx::nullopt;
            s.duration = std::chrono::milliseconds{210000};
            s.file_bytes = djinterop::stdx::nullopt;
            s.genre = "Genre";
            s.hot_cues.fill(djinterop::stdx::nullopt);
            s.key = djinterop::musical_key::a_minor;
            s.last_modified_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1509371790}};
            s.last_played_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1509321800}};
            s.last_accessed_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1509321600}};
            s.loops.fill(djinterop::stdx::nullopt);
            s.publisher = "Publisher";
            s.relative_path = "../01 - Some Artist - Some Track.mp3";
            s.sampling = djinterop::stdx::nullopt;
            s.title = "Some Title";
            s.track_number = 1;
            s.waveform.clear();
            s.year = 2017;
            break;

        case example_track_type::fully_analysed_1:
        {
            s.adjusted_beatgrid = {
                djinterop::beatgrid_marker{0, 20000},
                djinterop::beatgrid_marker{776, 16061375},
            };
            s.adjusted_main_cue = 88200;
            s.album = "Other Album";
            s.artist = "Other Artist";
            s.average_loudness = 0.555;
            s.bitrate = 1536;
            s.bpm = 128;
            s.comment = "Other Comment";
            s.composer = "Other Composer";
            s.default_beatgrid = s.adjusted_beatgrid;
            s.default_main_cue = s.adjusted_main_cue;
            s.duration = std::chrono::milliseconds{365000};
            if (version.schema_version >=
                djinterop::engine::os_1_4_0.schema_version)
            {
                s.file_bytes = 1048576;
            }

            s.genre = "Other Genre";
            s.hot_cues.fill(djinterop::stdx::nullopt);
            s.hot_cues[1] = djinterop::hot_cue{
                "Example cue", 102687.5,
                djinterop::engine::standard_pad_colors::pad_2};
            s.hot_cues[5] = djinterop::hot_cue{
                "Example other cue", 185375,
                djinterop::engine::standard_pad_colors::pad_6};
            s.key = djinterop::musical_key::d_minor;
            s.last_modified_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1617659524}};
            s.last_played_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1616548524}};
            s.last_accessed_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1615420800}};
            s.loops.fill(djinterop::stdx::nullopt);
            s.loops[7] = djinterop::loop{
                "Example loop", 102687.5, 185375,
                djinterop::engine::standard_pad_colors::pad_8};
            s.publisher = "Other Publisher";
            s.relative_path = "../02 - Other Artist - Other Track.flac";
            s.sampling = djinterop::sampling_info{44100, 16096500};
            s.title = "Some Title";
            s.track_number = 2;
            s.waveform.clear();
            int64_t samples_per_entry =
                djinterop::engine::required_waveform_samples_per_entry(
                    s.sampling->sample_rate);
            int64_t waveform_size =
                (s.sampling->sample_count + samples_per_entry - 1) /
                samples_per_entry;
            s.waveform.reserve(waveform_size);
            for (int64_t i = 0; i < waveform_size; ++i)
            {
                s.waveform.push_back(
                    {{(uint8_t)(i * 255 / waveform_size),
                      (uint8_t)(i * 255 / waveform_size)},
                     {(uint8_t)(i * 127 / waveform_size),
                      (uint8_t)(i * 127 / waveform_size)},
                     {(uint8_t)(i * 63 / waveform_size),
                      (uint8_t)(i * 63 / waveform_size)}});
            }

            s.year = 2021;
            break;
        }

        default: assertm("Unrecognised snapshot type", false);
    }
}