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
#ifndef DJINTEROP_TRACK_HPP
#define DJINTEROP_TRACK_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/musical_key.hpp>
#include <djinterop/performance_data.hpp>
#include <djinterop/semantic_version.hpp>

namespace djinterop
{
class database;
class crate;
class track_impl;
struct track_snapshot;

/// A `track` object is a handle to a track stored in a database. As long as it
/// lives, the corresponding database connection is kept open.
///
/// `track` objects can be copied and assigned cheaply, resulting in multiple
/// handles to the same actual track.
///
/// The read/write operations provided by this class directly access the
/// database.
///
/// A `track` object becomes invalid if the track gets deleted by
/// `database::remove_track()`. After that, you must not call any methods on the
/// `track` object, except for destructing it, or assigning to it.
class DJINTEROP_PUBLIC track
{
public:
    /// Copy constructor
    track(const track& other) noexcept;

    /// Destructor
    ~track();

    /// Copy assignment operator
    track& operator=(const track& other) noexcept;

    /// Obtain a snapshot of the track's current state.
    track_snapshot snapshot() const;

    /// Update the track with the contents of the provided snapshot.
    void update(const track_snapshot& snapshot);

    /// Get the beatgrid of the track.
    std::vector<beatgrid_marker> beatgrid() const;

    /// Set the beatgrid of the track.
    void set_beatgrid(std::vector<beatgrid_marker> beatgrid) const;

    /// Returns the album name (metadata) of the track
    std::optional<std::string> album() const;

    /// Sets the album name (metadata) of the track
    void set_album(std::optional<std::string> album) const;
    void set_album(std::string album) const;

    /// Returns the artist (metadata) of the track
    std::optional<std::string> artist() const;

    /// Sets the artist (metadata) of the track
    void set_artist(std::optional<std::string> artist) const;
    void set_artist(std::string artist) const;

    std::optional<double> average_loudness() const;

    void set_average_loudness(std::optional<double> average_loudness) const;
    void set_average_loudness(double average_loudness) const;

    /// Returns the bitrate (metadata) of the track
    std::optional<int> bitrate() const;

    /// Sets the bitrate (metadata) of the track
    void set_bitrate(std::optional<int> bitrate) const;
    void set_bitrate(int bitrate) const;

    /// Returns the BPM (metadata) of the track, rounded to the nearest integer
    std::optional<double> bpm() const;

    /// Sets the BPM (metadata) of the track, rounded to the nearest integer
    void set_bpm(std::optional<double> bpm) const;
    void set_bpm(double bpm) const;

    /// Returns the comment associated to the track (metadata)
    std::optional<std::string> comment() const;

    /// Sets the comment associated to the track (metadata)
    void set_comment(std::optional<std::string> comment) const;
    void set_comment(std::string comment) const;

    /// Returns the composer (metadata) of the track
    std::optional<std::string> composer() const;

    /// Sets the composer (metadata) of the track
    void set_composer(std::optional<std::string> composer) const;
    void set_composer(std::string composer) const;

    /// Returns the crates containing the track
    std::vector<crate> containing_crates() const;

    /// Returns the database containing the track
    database db() const;

    /// Returns the duration (metadata) of the track
    std::optional<std::chrono::milliseconds> duration() const;

    /// Sets the duration (metadata) of the track.
    void set_duration(std::optional<std::chrono::milliseconds> duration);
    void set_duration(std::chrono::milliseconds duration);

    // TODO (mr-smidge): Add `file_bytes()` and `set_file_bytes()` methods.

    /// Returns the file extension part of `track::relative_path()`
    ///
    /// An empty string is returned if the file doesn't have an extension.
    std::string file_extension() const;

    /// Returns the filename part of `track::relative_path()` (including the
    /// file extension)
    std::string filename() const;

    /// Returns the genre (metadata) of the track
    std::optional<std::string> genre() const;

    /// Sets the genre (metadata) of the track
    void set_genre(std::optional<std::string> genre) const;
    void set_genre(std::string genre) const;

    std::optional<hot_cue> hot_cue_at(int index) const;

    void set_hot_cue_at(int index, std::optional<hot_cue> cue) const;
    void set_hot_cue_at(int index, hot_cue cue) const;

    std::vector<std::optional<hot_cue>> hot_cues() const;

    void set_hot_cues(std::vector<std::optional<hot_cue>> cues) const;

    /// Returns the ID of this track
    ///
    /// The ID is used internally in the database and is unique for tracks
    /// contained in the same database.
    int64_t id() const;

    /// Returns `true` iff `*this` is valid as described in the class comment
    bool is_valid() const;

    /// Returns the key (metadata) of the track
    std::optional<musical_key> key() const;

    /// Sets the key (metadata) of the track
    void set_key(std::optional<musical_key> key) const;
    void set_key(musical_key key) const;

    /// Returns the time at which the track was last played
    std::optional<std::chrono::system_clock::time_point> last_played_at()
        const;

    /// Sets the time at which the track was last played
    void set_last_played_at(
        std::optional<std::chrono::system_clock::time_point> time) const;
    void set_last_played_at(std::chrono::system_clock::time_point time) const;

    std::optional<loop> loop_at(int index) const;

    void set_loop_at(int index, std::optional<loop> l) const;
    void set_loop_at(int index, loop l) const;

    std::vector<std::optional<loop>> loops() const;

    void set_loops(std::vector<std::optional<loop>> loops) const;

    /// Get the track's main cue point.
    std::optional<double> main_cue() const;

    /// Set the track's main cue point.
    void set_main_cue(std::optional<double> sample_offset) const;

    /// Returns the publisher (metadata) of the track
    std::optional<std::string> publisher() const;

    /// Sets the publisher (metadata) of the track
    void set_publisher(std::optional<std::string> publisher) const;
    void set_publisher(std::string publisher) const;

    /// Gets the track rating, from 0-100.
    std::optional<int> rating() const;

    /// Sets the track rating, from 0-100.  Any rating provided outside this
    /// range will be clamped.
    void set_rating(std::optional<int> rating);
    void set_rating(int rating);

    /// Get the path to this track's file on disk, relative to the music
    /// database.
    std::string relative_path() const;

    /// Set the path to this track's file on disk, relative to the directory of
    /// the database.
    void set_relative_path(std::string relative_path) const;

    /// Get the sample count of the track.
    std::optional<unsigned long long> sample_count() const;

    /// Set the sample count of the track.
    void set_sample_count(std::optional<unsigned long long> sample_count);
    void set_sample_count(unsigned long long sample_count);

    /// Get the sample rate of the track.
    std::optional<double> sample_rate() const;

    /// Set the sample rate of the track.
    void set_sample_rate(std::optional<double> sample_rate);
    void set_sample_rate(double sample_rate);

    /// Returns the title (metadata) of the track
    std::optional<std::string> title() const;

    /// Sets the title (metadata) of the track
    void set_title(std::optional<std::string> title) const;
    void set_title(std::string title) const;

    /// Returns the track number (metadata) of the track
    std::optional<int> track_number() const;

    /// Sets the track number (metadata) of the track
    void set_track_number(std::optional<int> track_number) const;
    void set_track_number(int track_number) const;

    // TODO (mr-smidge): Add `uri()` and `set_uri()` methods.

    std::vector<waveform_entry> waveform() const;

    void set_waveform(std::vector<waveform_entry> waveform) const;

    /// Returns the recording year (metadata) of the track
    std::optional<int> year() const;

    /// Sets the recording year (metadata) of the track
    void set_year(std::optional<int> year) const;
    void set_year(int year) const;

    // TODO (haslersn): non public?
    track(std::shared_ptr<track_impl> pimpl);

private:
    std::shared_ptr<track_impl> pimpl_;
};

}  // namespace djinterop

#endif  // DJINTEROP_TRACK_HPP
