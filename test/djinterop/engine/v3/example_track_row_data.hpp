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
#include <djinterop/engine/v3/track_table.hpp>

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
    example_track_row_type row_type, djinterop::engine::v3::track_row& r,
    const djinterop::engine::engine_schema& schema)
{
    using namespace std::string_literals;
    namespace ev = djinterop::engine;
    namespace ev3 = djinterop::engine::v3;
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
            r.last_edit_time = ev3::LAST_EDIT_TIME_NONE;
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
            r.title = "Title"s;
            r.artist = "Artist"s;
            r.album = "Album"s;
            r.genre = "Genre"s;
            r.comment = "Comment"s;
            r.label = "Label"s;
            r.composer = "Composer"s;
            r.remixer = "Remixer"s;
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
            r.last_edit_time = ev3::LAST_EDIT_TIME_NONE;

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
            r.title = "Title"s;
            r.artist = "Artist"s;
            r.album = "Album"s;
            r.genre = "Genre"s;
            r.comment = "Comment"s;
            r.label = "Label"s;
            r.composer = "Composer"s;
            r.remixer = "Remixer"s;
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
            r.streaming_flags = 0;
            r.explicit_lyrics = false;
            r.last_edit_time = ev3::LAST_EDIT_TIME_NONE;

            break;
        }

        default: assertm("Unrecognised track row type", false);
    }
}