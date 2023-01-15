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
#include <djinterop/engine/v2/track_table.hpp>

#include "../../boost_test_utils.hpp"

enum class example_track_row_type
{
    minimal_1,
    basic_metadata_only_1,
    fully_analysed_1,
};

inline std::ostream& operator<<(std::ostream& o, example_track_row_type v)
{
    switch (v)
    {
        case example_track_row_type::minimal_1: o << "minimal_1"; break;
        case example_track_row_type::basic_metadata_only_1:
            o << "basic_metadata_only_1";
            break;
        case example_track_row_type::fully_analysed_1:
            o << "fully_analysed_1";
            break;
        default: o << "unknown"; break;
    }

    return o;
}

inline void populate_track_row(
    example_track_row_type row_type, djinterop::engine::v2::track_row& r)
{
    namespace ev = djinterop::engine;
    namespace ev2 = djinterop::engine::v2;
    switch (row_type)
    {
        case example_track_row_type::minimal_1:
            r.path = "../path/to/minimal_file.mp3";
            r.filename = "minimal_file.mp3";
            r.album_art_id = 0;
            r.file_bytes = 1024;
            r.is_played = false;
            r.is_analyzed = false;
            r.is_available = true;
            break;

        case example_track_row_type::basic_metadata_only_1:
        {
            r.play_order = 1;
            r.length = 123;
            r.bpm = 135;
            r.year = 2022;
            r.path = "../path/to/file.mp3";
            r.filename = "file.mp3";
            r.bitrate = 320;
            r.bpm_analyzed = 135.25;
            r.album_art_id = 0;
            r.file_bytes = 1048576;
            r.title = "Title";
            r.artist = "Artist";
            r.album = "Album";
            r.genre = "Genre";
            r.comment = "Comment";
            r.label = "Label";
            r.composer = "Composer";
            r.remixer = "Remixer";
            r.key = 1;
            r.rating = 5;
            r.time_last_played = std::chrono::system_clock::time_point{
                std::chrono::seconds{1660860954}};
            r.is_played = true;
            r.file_type = "mp3";
            r.is_analyzed = false;
            r.date_created = std::chrono::system_clock::time_point{
                std::chrono::seconds{1460860954}};
            r.date_added = std::chrono::system_clock::time_point{
                std::chrono::seconds{1560860954}};
            r.is_available = true;
            r.is_metadata_of_packed_track_changed = false;
            r.is_performance_data_of_packed_track_changed = false;
            r.played_indicator = 3857238926;
            r.is_metadata_imported = true;
            r.pdb_import_key = 0;
            r.is_beat_grid_locked = false;
            r.origin_database_uuid = "e423f29c-fb50-4ddc-b730-fd1d6534b022";
            r.origin_track_id = 12345;
            r.streaming_flags = 0;
            r.explicit_lyrics = false;
            break;
        }

        case example_track_row_type::fully_analysed_1:
        {
            r.play_order = 1;
            r.length = 123;
            r.bpm = 135;
            r.year = 2022;
            r.path = "../path/to/file.mp3";
            r.filename = "file.mp3";
            r.bitrate = 320;
            r.bpm_analyzed = 135.25;
            r.album_art_id = 0;
            r.file_bytes = 1048576;
            r.title = "Title";
            r.artist = "Artist";
            r.album = "Album";
            r.genre = "Genre";
            r.comment = "Comment";
            r.label = "Label";
            r.composer = "Composer";
            r.remixer = "Remixer";
            r.key = 1;
            r.rating = 5;
            r.time_last_played = std::chrono::system_clock::time_point{
                std::chrono::seconds{1660860954}};
            r.is_played = true;
            r.file_type = "mp3";
            r.is_analyzed = true;
            r.date_created = std::chrono::system_clock::time_point{
                std::chrono::seconds{1460860954}};
            r.date_added = std::chrono::system_clock::time_point{
                std::chrono::seconds{1560860954}};
            r.is_available = true;
            r.is_metadata_of_packed_track_changed = false;
            r.is_performance_data_of_packed_track_changed = false;
            r.played_indicator = 3857238926;
            r.is_metadata_imported = true;
            r.pdb_import_key = 0;
            r.is_beat_grid_locked = false;
            r.origin_database_uuid = "e423f29c-fb50-4ddc-b730-fd1d6534b022";
            r.origin_track_id = 12345;
            r.track_data = ev2::track_data_blob{41000, 5424300, 0.5, 1};
            r.overview_waveform_data = ev2::overview_waveform_data_blob{};
            auto beatgrid = std::vector<ev2::beat_grid_marker_blob>{};
            beatgrid.push_back(ev2::beat_grid_marker_blob{0, 1, 16, 0});
            beatgrid.push_back(ev2::beat_grid_marker_blob{313600, 16, 0, 0});
            r.beat_data =
                ev2::beat_data_blob{41000, 5424300, 1, beatgrid, beatgrid};
            std::vector<ev2::quick_cue_blob> quick_cues_list{};
            quick_cues_list.push_back(ev2::quick_cue_blob{"First cue", 12345, ev::standard_pad_colors::pad_1});
            r.quick_cues = ev2::quick_cues_blob{quick_cues_list, 11111, true, 22222};
            r.loops = ev2::loops_blob{};
            r.streaming_flags = 0;
            r.explicit_lyrics = false;
            break;
        }

        default: assertm("Unrecognised track row type", false);
    }
}