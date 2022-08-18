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

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/engine/v2/beat_data_blob.hpp>
#include <djinterop/engine/v2/loops_blob.hpp>
#include <djinterop/engine/v2/overview_waveform_data_blob.hpp>
#include <djinterop/engine/v2/quick_cues_blob.hpp>
#include <djinterop/engine/v2/track_data_blob.hpp>
#include <djinterop/optional.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

/// Thrown when the id on a track row is in an erroneous state for a given
/// operation.
struct DJINTEROP_PUBLIC track_row_id_error : public std::runtime_error
{
public:
    explicit track_row_id_error(const std::string& what_arg) noexcept
        : runtime_error{what_arg}
    {
    }
};

/// Represents a row in the `Track` table.
struct DJINTEROP_PUBLIC track_row
{
    /// Auto-generated id column.
    ///
    /// A value of `0` can be used to indicate track row that is not yet
    /// persisted in the table, e.g. when adding a new row.
    int64_t id;

    /// Play order.
    int64_t play_order;

    /// Approximate length of track, in seconds.
    int64_t length;

    /// Approximate BPM of track.
    int64_t bpm;

    /// Track year.
    djinterop::stdx::optional<int64_t> year;

    /// Relative path from the location of the Engine library to the music file
    /// underlying this track.
    std::string path;

    /// Name of the file underlying this track.
    std::string filename;

    /// Approximate bitrate.
    int64_t bitrate;

    /// BPM, as determined from track analysis.
    double bpm_analyzed;

    /// Id of album art, or 0 if none.
    int64_t album_art_id;

    /// Size of the file on disk, in bytes.
    int64_t file_bytes;

    /// Track title.
    djinterop::stdx::optional<std::string> title;

    /// Track artist.
    djinterop::stdx::optional<std::string> artist;

    /// Track album.
    djinterop::stdx::optional<std::string> album;

    /// Track genre.
    djinterop::stdx::optional<std::string> genre;

    /// Track comment.
    djinterop::stdx::optional<std::string> comment;

    /// Track label.
    djinterop::stdx::optional<std::string> label;

    /// Track composer.
    djinterop::stdx::optional<std::string> composer;

    /// Track remixer.
    djinterop::stdx::optional<std::string> remixer;

    /// Musical key.
    int64_t key;

    /// Rating (or 0 if none).
    int64_t rating;

    /// Album art.
    djinterop::stdx::optional<std::string> album_art;

    /// Time at which the track was last played.
    djinterop::stdx::optional<std::chrono::system_clock::time_point>
        time_last_played;

    /// Flag indicating whether the track has ever been played.
    bool is_played;

    /// Type of file.
    std::string file_type;

    /// Flag indicating whether the track has been analysed.
    bool is_analyzed;

    /// Time at which the track was created.
    djinterop::stdx::optional<std::chrono::system_clock::time_point>
        date_created;

    /// Time at which the track was added to the database.
    djinterop::stdx::optional<std::chrono::system_clock::time_point> date_added;

    /// Flag indicating if the file underpinning this track entry is available.
    bool is_available;

    /// Flag indicating if meta-data has changed, in the event of a packed
    /// track.
    bool is_metadata_of_packed_track_changed;

    /// Flag indicating if performance data has changed, in the event of a
    /// packed track.
    bool is_performance_data_of_packed_track_changed;

    /// Random number matching the equivalent field in the `Information` table,
    /// if this track is the most recent to be played.
    djinterop::stdx::optional<int64_t> played_indicator;

    /// Flag indicating whether meta-data has been imported.
    bool is_metadata_imported;

    /// PDB import key.
    int64_t pdb_import_key;

    /// Streaming source.
    djinterop::stdx::optional<std::string> streaming_source;

    /// URI of track.
    djinterop::stdx::optional<std::string> uri;

    /// Flag indicating whether the beat grid has been locked for this track.
    bool is_beat_grid_locked;

    /// UUID of database from which this track was originally imported.
    ///
    /// If the track has not been imported from another database, this field is
    /// set to the UUID of the current database.
    std::string origin_database_uuid;

    /// ID of track in the original database from which it was imported.
    ///
    /// If the track has not been imported from another database, this field is
    /// set to the id of the track in the current database.
    int64_t origin_track_id;

    /// Analyzed track data.
    track_data_blob track_data;

    /// Waveform data.
    overview_waveform_data_blob overview_waveform_data;

    /// Beat data.
    beat_data_blob beat_data;

    /// Quick cue data.
    quick_cues_blob quick_cues;

    /// Loop data.
    loops_blob loops;

    /// Third-party source id.
    djinterop::stdx::optional<int64_t> third_party_source_id;

    /// Streaming flags.
    int64_t streaming_flags;

    /// Flag indicating whether this track has explicit lyrics.
    bool explicit_lyrics;
};

/// Represents the `Track` table in an Engine v2 database.
class DJINTEROP_PUBLIC track_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit track_table(std::shared_ptr<engine_library_context> context);

    /// Add a track row to the table.
    ///
    /// \param row Track row to add.
    /// \return Returns the id of the newly-added track row.
    int64_t add(const track_row& row);

    /// Get an entire track row.
    ///
    /// \param id Id of track.
    /// \return Returns a track row, or none if not found.
    djinterop::stdx::optional<track_row> get(int64_t id);

    /// Update an existing track row in the table.
    ///
    /// \param row Track row to update.
    void update(const track_row& row);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
