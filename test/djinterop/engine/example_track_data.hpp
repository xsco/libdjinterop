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
#include <string>

#include <djinterop/engine/engine.hpp>
#include <djinterop/track_snapshot.hpp>

#include "../boost_test_utils.hpp"

enum class example_track_data_variation
{
    empty,
    minimal_1,
    basic_metadata_only_1,
    fully_analysed_1,
};

inline std::ostream& operator<<(std::ostream& o, example_track_data_variation v)
{
    switch (v)
    {
        case example_track_data_variation::empty: o << "empty"; break;
        case example_track_data_variation::minimal_1: o << "minimal_1"; break;
        case example_track_data_variation::basic_metadata_only_1:
            o << "basic_metadata_only_1";
            break;
        case example_track_data_variation::fully_analysed_1:
            o << "fully_analysed_1";
            break;
        default: o << "unknown"; break;
    }

    return o;
}

enum class example_track_data_usage
{
    create,
    update,
    fetch,
};

inline std::ostream& operator<<(std::ostream& o, example_track_data_usage v)
{
    switch (v)
    {
        case example_track_data_usage::create: o << "create"; break;
        case example_track_data_usage::update: o << "update"; break;
        case example_track_data_usage::fetch: o << "fetch"; break;
        default: o << "unknown"; break;
    }

    return o;
}

inline void populate_track_snapshot(
    djinterop::track_snapshot& s, example_track_data_variation variation,
    example_track_data_usage usage, djinterop::engine::engine_schema schema)
{
    using namespace std::string_literals;
    switch (variation)
    {
        case example_track_data_variation::empty:
            s.album = std::nullopt;
            s.artist = std::nullopt;
            s.average_loudness = std::nullopt;
            s.beatgrid.clear();
            s.bitrate = std::nullopt;
            s.bpm = std::nullopt;
            s.comment = std::nullopt;
            s.composer = std::nullopt;
            s.duration = std::nullopt;
            s.file_bytes = std::nullopt;
            s.genre = std::nullopt;
            s.hot_cues.clear();
            if (usage == example_track_data_usage::fetch)
                s.hot_cues.resize(8);

            s.key = std::nullopt;
            s.last_played_at = std::nullopt;
            s.loops.clear();
            if (usage == example_track_data_usage::fetch)
                s.loops.resize(8);

            s.main_cue = std::nullopt;
            s.publisher = std::nullopt;
            s.relative_path = std::nullopt;
            s.sample_count = std::nullopt;
            s.sample_rate = std::nullopt;
            s.title = std::nullopt;
            s.track_number = std::nullopt;
            s.waveform.clear();
            s.year = std::nullopt;
            break;

        case example_track_data_variation::minimal_1:
            s.album = std::nullopt;
            s.artist = std::nullopt;
            s.average_loudness = std::nullopt;
            s.beatgrid.clear();
            s.bitrate = std::nullopt;
            s.bpm = std::nullopt;
            s.comment = std::nullopt;
            s.composer = std::nullopt;
            s.duration = std::nullopt;
            s.file_bytes = std::nullopt;
            s.genre = std::nullopt;
            s.hot_cues.clear();
            if (usage == example_track_data_usage::fetch)
                s.hot_cues.resize(8);

            s.key = std::nullopt;
            s.last_played_at = std::nullopt;
            s.loops.clear();
            if (usage == example_track_data_usage::fetch)
                s.loops.resize(8);

            s.main_cue = std::nullopt;
            s.publisher = std::nullopt;
            s.relative_path = "filename.ext"s;
            s.sample_count = std::nullopt;
            s.sample_rate = std::nullopt;
            s.title = std::nullopt;
            s.track_number = std::nullopt;
            s.waveform.clear();
            s.year = std::nullopt;
            break;

        case example_track_data_variation::basic_metadata_only_1:
            s.album = "Some Album"s;
            s.artist = "Some Artist"s;
            s.average_loudness = std::nullopt;
            s.beatgrid.clear();
            s.bitrate = 320;
            s.bpm = 123;
            s.comment = "Comment"s;
            s.composer = "Composer"s;
            s.duration = std::chrono::milliseconds{210000};
            s.file_bytes = std::nullopt;
            s.genre = "Genre"s;
            s.hot_cues.clear();
            if (usage == example_track_data_usage::fetch)
                s.hot_cues.resize(8);

            s.key = djinterop::musical_key::a_minor;
            s.last_played_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1509321800}};
            s.loops.clear();
            if (usage == example_track_data_usage::fetch)
                s.loops.resize(8);

            s.main_cue = std::nullopt;
            s.publisher = "Publisher"s;
            s.relative_path = "../01 - Some Artist - Some Track.mp3"s;
            s.sample_count = std::nullopt;
            s.sample_rate = std::nullopt;
            s.title = "Some Title"s;
            s.track_number = 1;
            s.waveform.clear();
            s.year = 2017;
            break;

        case example_track_data_variation::fully_analysed_1:
        {
            s.beatgrid.clear();
            s.beatgrid.push_back({0, 20000});
            s.beatgrid.push_back({776, 16061375});
            s.album = "Other Album"s;
            s.artist = "Other Artist"s;
            s.average_loudness = 0.555;
            s.bitrate = 1536;
            s.bpm = 128;
            s.comment = "Other Comment"s;
            s.composer = "Other Composer"s;
            s.duration = std::chrono::milliseconds{365000};
            if (schema >= djinterop::engine::engine_schema::schema_1_15_0)
            {
                s.file_bytes = 1048576;
            }

            s.genre = "Other Genre"s;
            s.hot_cues.clear();
            if (usage == example_track_data_usage::fetch)
                s.hot_cues.resize(8);
            else
                s.hot_cues.resize(6);

            s.hot_cues[1] = djinterop::hot_cue{
                "Example cue", 102687.5,
                djinterop::engine::standard_pad_colors::pad_2};
            s.hot_cues[5] = djinterop::hot_cue{
                "Example other cue", 185375,
                djinterop::engine::standard_pad_colors::pad_6};
            s.key = djinterop::musical_key::d_minor;
            s.last_played_at = std::chrono::system_clock::time_point{
                std::chrono::seconds{1616548524}};
            s.loops.clear();
            s.loops.resize(8);
            s.loops[7] = djinterop::loop{
                "Example loop", 102687.5, 185375,
                djinterop::engine::standard_pad_colors::pad_8};
            s.main_cue = 88200;
            s.publisher = "Other Publisher"s;
            s.relative_path = "../02 - Other Artist - Other Track.flac"s;
            s.sample_count = 16096500;
            s.sample_rate = 44100;
            s.title = "Some Title"s;
            s.track_number = 2;
            s.waveform.clear();
            auto waveform_extents =
                schema >= djinterop::engine::engine_schema::schema_2_18_0
                    ? djinterop::engine::calculate_overview_waveform_extents(
                          *s.sample_count, *s.sample_rate)
                    : djinterop::engine::
                          calculate_high_resolution_waveform_extents(
                              *s.sample_count, *s.sample_rate);
            s.waveform.reserve(waveform_extents.size);
            for (unsigned long long i = 0; i < waveform_extents.size; ++i)
            {
                if (schema < djinterop::engine::engine_schema::schema_2_18_0)
                {
                    s.waveform.push_back(
                        {{(uint8_t)(i * 255 / waveform_extents.size),
                          (uint8_t)(i * 255 / waveform_extents.size)},
                         {(uint8_t)(i * 127 / waveform_extents.size),
                          (uint8_t)(i * 127 / waveform_extents.size)},
                         {(uint8_t)(i * 63 / waveform_extents.size),
                          (uint8_t)(i * 63 / waveform_extents.size)}});
                }
                else
                {
                    // Opacity not supported in later versions.
                    s.waveform.push_back(
                        {{(uint8_t)(i * 255 / waveform_extents.size)},
                         {(uint8_t)(i * 127 / waveform_extents.size)},
                         {(uint8_t)(i * 63 / waveform_extents.size)}});
                }
            }

            s.year = 2021;
            break;
        }

        default: assertm("Unrecognised snapshot type", false);
    }
}