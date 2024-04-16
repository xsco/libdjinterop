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
#ifndef DJINTEROP_TRACK_SNAPSHOT_HPP
#define DJINTEROP_TRACK_SNAPSHOT_HPP

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include <djinterop/musical_key.hpp>
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
///
/// Note that the snapshot does not contain any reference to a track identifier:
/// the association between a snapshot and any real track is left to the user
/// to define and maintain.
struct track_snapshot
{
    /// The album name metadata.
    std::optional<std::string> album;

    /// The artist name metadata.
    std::optional<std::string> artist;

    /// The average loudness metadata.
    std::optional<double> average_loudness;

    /// The beatgrid.
    std::vector<beatgrid_marker> beatgrid;

    /// The bitrate metadata.
    std::optional<int> bitrate;

    /// The BPM metadata.
    std::optional<double> bpm;

    /// The comment metadata.
    std::optional<std::string> comment;

    /// The composer metadata.
    std::optional<std::string> composer;

    /// The duration metadata.
    std::optional<std::chrono::milliseconds> duration;

    /// The size of the file, in bytes, metadata.
    std::optional<unsigned long long> file_bytes;

    /// The genre metadata.
    std::optional<std::string> genre;

    /// The hot cues.
    std::vector<std::optional<hot_cue>> hot_cues;

    /// The key.
    std::optional<musical_key> key;

    /// The time at which the track was last played.
    std::optional<std::chrono::system_clock::time_point> last_played_at;

    /// The loops.
    std::vector<std::optional<loop>> loops;

    /// The main cue sample offset.
    std::optional<double> main_cue;

    /// The publisher metadata.
    std::optional<std::string> publisher;

    /// The track rating, from 0-100.
    /// Any rating provided outside this range will be clamped.
    std::optional<int> rating;

    /// The path to this track's file on disk, relative to the directory of
    /// the database.
    std::optional<std::string> relative_path;

    /// Number of audio samples within the track.
    std::optional<unsigned long long> sample_count;

    /// Sample rate, i.e. number of samples per second.
    std::optional<double> sample_rate;

    /// The title metadata.
    std::optional<std::string> title;

    /// The track number metadata.
    std::optional<int> track_number;

    // TODO (mr-smidge): Add `std::optional<std::string> uri` field.

    /// The waveform.
    std::vector<waveform_entry> waveform;

    /// The recording year metadata.
    std::optional<int> year;

    friend bool operator==(
        const track_snapshot& lhs, const track_snapshot& rhs) noexcept
    {
        return std::tie(
                   lhs.album, lhs.artist, lhs.average_loudness, lhs.beatgrid,
                   lhs.bitrate, lhs.bpm, lhs.comment, lhs.composer,
                   lhs.duration, lhs.file_bytes, lhs.genre, lhs.hot_cues,
                   lhs.key, lhs.last_played_at, lhs.loops, lhs.main_cue,
                   lhs.publisher, lhs.rating, lhs.relative_path,
                   lhs.sample_count, lhs.sample_rate, lhs.title,
                   lhs.track_number, lhs.waveform, lhs.year) ==
               std::tie(
                   rhs.album, rhs.artist, rhs.average_loudness, rhs.beatgrid,
                   rhs.bitrate, rhs.bpm, rhs.comment, rhs.composer,
                   rhs.duration, rhs.file_bytes, rhs.genre, rhs.hot_cues,
                   rhs.key, rhs.last_played_at, rhs.loops, rhs.main_cue,
                   rhs.publisher, rhs.rating, rhs.relative_path,
                   rhs.sample_count, rhs.sample_rate, rhs.title,
                   rhs.track_number, rhs.waveform, rhs.year);
    }

    friend bool operator!=(
        const track_snapshot& lhs, const track_snapshot& rhs) noexcept
    {
        return !(rhs == lhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const track_snapshot& obj) noexcept
    {
#define PRINT_FIELD(field) \
    os << ", " #field "="; \
    stream_helper::print(os, obj.field)

        os << "track_snapshot{album=";
        stream_helper::print(os, obj.album);

        PRINT_FIELD(artist);
        PRINT_FIELD(average_loudness);
        PRINT_FIELD(beatgrid);
        PRINT_FIELD(bitrate);
        PRINT_FIELD(bpm);
        PRINT_FIELD(comment);
        PRINT_FIELD(composer);
        PRINT_FIELD(duration);
        PRINT_FIELD(file_bytes);
        PRINT_FIELD(genre);
        PRINT_FIELD(hot_cues);
        PRINT_FIELD(key);
        PRINT_FIELD(last_played_at);
        PRINT_FIELD(loops);
        PRINT_FIELD(main_cue);
        PRINT_FIELD(publisher);
        PRINT_FIELD(rating);
        PRINT_FIELD(relative_path);
        PRINT_FIELD(sample_count);
        PRINT_FIELD(sample_rate);
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

#endif  // DJINTEROP_TRACK_SNAPSHOT_HPP
