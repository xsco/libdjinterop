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

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_TRACK_HPP
#define DJINTEROP_ENGINELIBRARY_TRACK_HPP

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>

#include <djinterop/enginelibrary/musical_key.hpp>
#include <djinterop/enginelibrary/performance_data.hpp>
#include <djinterop/enginelibrary/schema_version.hpp>

namespace djinterop
{
namespace enginelibrary
{
class database;
class crate;

/// The `track_deleted` exception is thrown when an invalid `track` object is
/// used, i.e. one that does not exist in the database anymore.
class track_deleted : public std::invalid_argument
{
public:
    /// Constructs the exception for a given track ID
    explicit track_deleted(int64_t id) noexcept
        : invalid_argument{"Track does not exist in database"}, id_{id}
    {
    }

    /// Returns the track ID that was found to be non-existent
    int64_t id() const noexcept { return id_; }

private:
    int64_t id_;
};

class track_import_info
{
public:
    track_import_info() noexcept;
    track_import_info(
        std::string external_db_uuid, int64_t externl_track_id) noexcept;
    std::string& external_db_uuid();
    const std::string& external_db_uuid() const;
    int64_t& external_track_id();
    const int64_t& external_track_id() const;

private:
    std::string external_db_uuid_;
    int64_t external_track_id_;
};

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
class track
{
public:
    /// Copy constructor
    track(const track& other) noexcept;

    /// Destructor
    ~track();

    /// Copy assignment operator
    track& operator=(const track& other) noexcept;

    std::vector<beatgrid_marker> adjusted_beatgrid() const;

    void set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid) const;

    double adjusted_main_cue() const;

    void set_adjusted_main_cue(double sample_offset) const;

    /// Returns the album name (metadata) of the track
    boost::optional<std::string> album() const;

    /// Sets the album name (metadata) of the track
    void set_album(boost::optional<boost::string_view> album) const;
    void set_album(boost::string_view album) const;

    /// Returns the ID of the `album_art` associated to the track
    ///
    /// If the track doesn't have an associated `album_art`, then `boost::none`
    /// is returned.
    /// TODO (haslersn): Return an `album_art` object instead.
    boost::optional<int64_t> album_art_id() const;

    /// Sets the ID of the `album_art` associated to the track
    ///
    /// If the track doesn't have an associated `album_art`, then `boost::none`
    /// is returned.
    /// TODO (haslersn): Return an `album_art` object instead.
    void set_album_art_id(boost::optional<int64_t> album_art_id) const;
    void set_album_art_id(int64_t album_art_id) const;

    /// Returns the artist (metadata) of the track
    boost::optional<std::string> artist() const;

    /// Returns the artist (metadata) of the track
    void set_artist(boost::optional<boost::string_view> artist) const;
    void set_artist(boost::string_view artist) const;

    boost::optional<double> average_loudness() const;

    void set_average_loudness(boost::optional<double> average_loudness) const;
    void set_average_loudness(double average_loudness) const;

    /// Returns the bitrate (metadata) of the track
    boost::optional<int64_t> bitrate() const;

    /// Sets the bitrate (metadata) of the track
    void set_bitrate(boost::optional<int64_t> bitrate) const;
    void set_bitrate(int64_t bitrate) const;

    /// Returns the BPM (metadata) of the track, rounded to the nearest integer
    boost::optional<double> bpm() const;

    /// Sets the BPM (metadata) of the track, rounded to the nearest integer
    void set_bpm(boost::optional<double> bpm) const;
    void set_bpm(double bpm) const;

    /// Returns the comment associated to the track (metadata)
    boost::optional<std::string> comment() const;

    /// Sets the comment associated to the track (metadata)
    void set_comment(boost::optional<boost::string_view> comment) const;
    void set_comment(boost::string_view comment) const;

    /// Returns the composer (metadata) of the track
    boost::optional<std::string> composer() const;

    /// Sets the composer (metadata) of the track
    void set_composer(boost::optional<boost::string_view> composer) const;
    void set_composer(boost::string_view composer) const;

    /// Returns the crates containing the track
    std::vector<crate> containing_crates() const;

    /// Returns the database containing the track
    database db() const;

    std::vector<beatgrid_marker> default_beatgrid() const;

    void set_default_beatgrid(std::vector<beatgrid_marker> beatgrid) const;

    double default_main_cue() const;

    void set_default_main_cue(double sample_offset) const;

    /// Returns the duration (metadata) of the track
    boost::optional<std::chrono::milliseconds> duration() const;

    /// Returns the file extension part of `track::relative_path()`
    ///
    /// An empty string is returned if the file doesn't have an extension.
    std::string file_extension() const;

    /// Returns the filename part of `track::relative_path()` (including the
    /// file extension)
    std::string filename() const;

    /// Returns the genre (metadata) of the track
    boost::optional<std::string> genre() const;

    /// Sets the genre (metadata) of the track
    void set_genre(boost::optional<boost::string_view> genre) const;
    void set_genre(boost::string_view genre) const;

    boost::optional<hot_cue> hot_cue_at(int32_t index) const;

    void set_hot_cue_at(int32_t index, boost::optional<hot_cue> cue) const;
    void set_hot_cue_at(int32_t index, hot_cue cue) const;

    std::array<boost::optional<hot_cue>, 8> hot_cues() const;

    void set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues) const;

    /// Returns the ID of this track
    ///
    /// The ID is used internally in the database and is unique for tracks
    /// contained in the same database.
    int64_t id() const;

    /// TODO (haslersn): Document this method.
    boost::optional<track_import_info> import_info() const;

    /// TODO (haslersn): Document these methods.
    void set_import_info(
        const boost::optional<track_import_info>& import_info) const;
    void set_import_info(const track_import_info& import_info) const;

    /// Returns `true` iff `*this` is valid as described in the class comment
    bool is_valid() const;

    /// Returns the key (metadata) of the track
    boost::optional<musical_key> key() const;

    /// Sets the key (metadata) of the track
    void set_key(boost::optional<musical_key> key) const;
    void set_key(musical_key key) const;

    /// Get the time at which this track was last accessed
    ///
    /// Note that on VFAT filesystems, the access time is ceiled to just a date,
    /// and loses any time precision.
    boost::optional<std::chrono::system_clock::time_point> last_accessed_at()
        const;

    /// TODO (haslersn): Document these methods.
    void set_last_accessed_at(
        boost::optional<std::chrono::system_clock::time_point> last_accessed_at)
        const;
    void set_last_accessed_at(
        std::chrono::system_clock::time_point last_accessed_at) const;

    /// Get the time of last attribute modification of this track's file
    ///
    /// Note that this is the attribute modification time, not the data
    /// modification time, i.e. ctime not mtime.
    boost::optional<std::chrono::system_clock::time_point> last_modified_at()
        const;

    /// TODO (haslersn): Document these methods.
    void set_last_modified_at(
        boost::optional<std::chrono::system_clock::time_point> last_modified_at)
        const;
    void set_last_modified_at(
        std::chrono::system_clock::time_point last_modified_at) const;

    /// Returns the time at which the track was last played
    boost::optional<std::chrono::system_clock::time_point> last_played_at()
        const;

    /// Sets the time at which the track was last played
    void set_last_played_at(
        boost::optional<std::chrono::system_clock::time_point> time) const;
    void set_last_played_at(std::chrono::system_clock::time_point time) const;

    boost::optional<loop> loop_at(int32_t index) const;

    void set_loop_at(int32_t index, boost::optional<loop> l) const;
    void set_loop_at(int32_t index, loop l) const;

    std::array<boost::optional<loop>, 8> loops() const;

    void set_loops(std::array<boost::optional<loop>, 8> loops) const;

    std::vector<waveform_entry> overview_waveform() const;

    /// Returns the publisher (metadata) of the track
    boost::optional<std::string> publisher() const;

    /// Sets the publisher (metadata) of the track
    void set_publisher(boost::optional<boost::string_view> publisher) const;
    void set_publisher(boost::string_view publisher) const;

    int64_t recommended_waveform_size() const;

    /// Get the path to this track's file on disk, relative to the music
    /// database.
    std::string relative_path() const;

    /// TODO (haslersn): Document this method.
    void set_relative_path(boost::string_view relative_path) const;

    boost::optional<sampling_info> sampling() const;

    void set_sampling(boost::optional<sampling_info> sample_rate) const;
    void set_sampling(sampling_info sample_rate) const;

    /// Returns the title (metadata) of the track
    boost::optional<std::string> title() const;

    /// Sets the title (metadata) of the track
    void set_title(boost::optional<boost::string_view> title) const;
    void set_title(boost::string_view title) const;

    /// Returns the track number (metadata) of the track
    boost::optional<int32_t> track_number() const;

    /// Sets the track number (metadata) of the track
    void set_track_number(boost::optional<int32_t> track_number) const;
    void set_track_number(int32_t track_number) const;

    std::vector<waveform_entry> waveform() const;

    void set_waveform(std::vector<waveform_entry> waveform) const;

    /// Returns the recording year (metadata) of the track
    boost::optional<int32_t> year() const;

    /// Sets the recording year (metadata) of the track
    void set_year(boost::optional<int32_t> year) const;
    void set_year(int32_t year) const;

private:
    track(database database, int64_t id);

    struct impl;
    std::shared_ptr<impl> pimpl_;

    friend class crate;
    friend class database;
};

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_TRACK_HPP
