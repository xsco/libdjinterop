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
#include <vector>

#include <djinterop/musical_key.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

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
    /// Create a new instance of `track_snapshot` to assemble a new track.
    track_snapshot() : id{stdx::nullopt} {}

    /// Create new instance of `track_snapshot` to edit an existing track.
    explicit track_snapshot(int64_t id) : id{id} {}

    const stdx::optional<int64_t> id;

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
};

}  // namespace djinterop

#endif  // DJINTEROP_TRACK_BUILDER_HPP
