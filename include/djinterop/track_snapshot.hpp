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
#ifndef DJINTEROP_TRACK_BUILDER_HPP
#define DJINTEROP_TRACK_BUILDER_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <array>
#include <chrono>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

#include <djinterop/musical_key.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>
#include <djinterop/stream_helper.hpp>

namespace djinterop
{
class database;

/// The `track_snapshot` struct represents a snapshot of the data for a given
/// track.
///
/// This class facilitates an efficient way to make multiple changes to a track
/// at once, or to create a new track from scratch, by allowing a series of
/// changes to the snapshot that can be committed to a database at a later time.
/// This means that only minimal interactions with the database are required to
/// persist the changes.
struct track_snapshot
{
    /// The track id, if persisted.
    stdx::optional<int64_t> id;

    /// The adjusted beatgrid.
    std::vector<beatgrid_marker> adjusted_beatgrid;

    /// The adjusted main cue sample offset.
    stdx::optional<double> adjusted_main_cue;

    /// The album name metadata.
    stdx::optional<std::string> album;

    /// The artist name metadata.
    stdx::optional<std::string> artist;

    /// The average loudness.
    stdx::optional<double> average_loudness;

    /// The bitrate metadata.
    stdx::optional<int64_t> bitrate;

    /// The BPM metadata.
    stdx::optional<double> bpm;

    /// The comment metadata.
    stdx::optional<std::string> comment;

    /// The composer metadata.
    stdx::optional<std::string> composer;

    /// The default beatgrid.
    std::vector<beatgrid_marker> default_beatgrid;

    /// The default main cue sample offset.
    stdx::optional<double> default_main_cue;

    /// The duration metadata.
    stdx::optional<std::chrono::milliseconds> duration;

    /// The size of the file, in bytes.
    stdx::optional<int64_t> file_bytes;

    /// The genre metadata.
    stdx::optional<std::string> genre;

    /// The hot cues.
    std::array<stdx::optional<hot_cue>, 8> hot_cues;

    /// The key.
    stdx::optional<musical_key> key;

    /// The time at which this track was last accessed.
    stdx::optional<std::chrono::system_clock::time_point> last_accessed_at;

    /// The time of last attribute modification of this track's file.
    stdx::optional<std::chrono::system_clock::time_point> last_modified_at;

    /// The time at which the track was last played.
    stdx::optional<std::chrono::system_clock::time_point> last_played_at;

    /// The loops.
    std::array<stdx::optional<loop>, 8> loops;

    /// The publisher metadata.
    stdx::optional<std::string> publisher;

    /// The track rating, from 0-100.
    /// Any rating provided outside this range will be clamped.
    stdx::optional<int32_t> rating;

    /// The path to this track's file on disk, relative to the directory of
    /// the database.
    stdx::optional<std::string> relative_path;

    /// The sampling info.
    stdx::optional<sampling_info> sampling;

    /// The title metadata.
    stdx::optional<std::string> title;

    /// The track number metadata.
    stdx::optional<int32_t> track_number;

    /// The waveform.
    std::vector<waveform_entry> waveform;

    /// The recording year metadata.
    stdx::optional<int32_t> year;

    friend bool operator==(
        const track_snapshot& lhs, const track_snapshot& rhs) noexcept
    {
        return std::tie(
                   lhs.id, lhs.adjusted_beatgrid, lhs.adjusted_main_cue,
                   lhs.album, lhs.artist, lhs.average_loudness, lhs.bitrate,
                   lhs.bpm, lhs.comment, lhs.composer, lhs.default_beatgrid,
                   lhs.default_main_cue, lhs.duration, lhs.file_bytes,
                   lhs.genre, lhs.hot_cues, lhs.key, lhs.last_accessed_at,
                   lhs.last_modified_at, lhs.last_played_at, lhs.loops,
                   lhs.publisher, lhs.rating, lhs.relative_path, lhs.sampling,
                   lhs.title, lhs.track_number, lhs.waveform, lhs.year) ==
               std::tie(
                   rhs.id, rhs.adjusted_beatgrid, rhs.adjusted_main_cue,
                   rhs.album, rhs.artist, rhs.average_loudness, rhs.bitrate,
                   rhs.bpm, rhs.comment, rhs.composer, rhs.default_beatgrid,
                   rhs.default_main_cue, rhs.duration, rhs.file_bytes,
                   rhs.genre, rhs.hot_cues, rhs.key, rhs.last_accessed_at,
                   rhs.last_modified_at, rhs.last_played_at, rhs.loops,
                   rhs.publisher, rhs.rating, rhs.relative_path, rhs.sampling,
                   rhs.title, rhs.track_number, rhs.waveform, rhs.year);
    }

    friend bool operator!=(
        const track_snapshot& lhs, const track_snapshot& rhs) noexcept
    {
        return !(rhs == lhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const track_snapshot& obj) noexcept
    {
#define PRINT_FIELD(field) \
        os << ", " #field "="; \
        stream_helper::print(os, obj.field)

        os << "track_snapshot{id=";
        stream_helper::print(os, obj.id);

        PRINT_FIELD(adjusted_beatgrid);
        PRINT_FIELD(adjusted_main_cue);
        PRINT_FIELD(album);
        PRINT_FIELD(artist);
        PRINT_FIELD(average_loudness);
        PRINT_FIELD(bitrate);
        PRINT_FIELD(bpm);
        PRINT_FIELD(comment);
        PRINT_FIELD(composer);
        PRINT_FIELD(default_beatgrid);
        PRINT_FIELD(default_main_cue);
        PRINT_FIELD(duration);
        PRINT_FIELD(file_bytes);
        PRINT_FIELD(genre);
        PRINT_FIELD(hot_cues);
        PRINT_FIELD(key);
        PRINT_FIELD(last_accessed_at);
        PRINT_FIELD(last_modified_at);
        PRINT_FIELD(last_played_at);
        PRINT_FIELD(loops);
        PRINT_FIELD(publisher);
        PRINT_FIELD(rating);
        PRINT_FIELD(relative_path);
        PRINT_FIELD(sampling);
        PRINT_FIELD(title);
        PRINT_FIELD(track_number);
        os << ", waveform=[#" << obj.waveform.size() << "]";
        PRINT_FIELD(year);

        os << "}";
        return os;
#undef PRINT_FIELD
    }
};

}  // namespace djinterop

#endif  // DJINTEROP_TRACK_BUILDER_HPP
