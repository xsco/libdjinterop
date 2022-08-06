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

#include <djinterop/engine/v2/track_table.hpp>

#include <cassert>
#include <utility>

#include <djinterop/engine/v2/engine_library_context.hpp>

#include "../../util.hpp"

namespace djinterop::engine::v2
{

track_table::track_table(std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

djinterop::stdx::optional<track_row> track_table::get(int64_t id)
{
    djinterop::stdx::optional<track_row> result;

    context_->db << "SELECT id, playOrder, length, bpm, year, path, filename, "
                    "bitrate, bpmAnalyzed, albumArtId, fileBytes, title, "
                    "artist, album, genre, comment, label, composer, remixer, "
                    "key, rating, albumArt, timeLastPlayed, isPlayed, "
                    "fileType, isAnalyzed, dateCreated, dateAdded, "
                    "isAvailable, isMetadataOfPackedTrackChanged, "
                    "playedIndicator, isMetadataImported, pdbImportKey, "
                    "streamingSource, uri, isBeatGridLocked, "
                    "originDatabaseUuid, originTrackId, trackData, "
                    "overviewWaveFormData, beatData, quickCues, loops, "
                    "thirdPartySourceId, streamingFlags, explicitLyrics "
                    "FROM Track WHERE id = ?"
                 << id >>
        [&](int64_t id, int64_t play_order, int64_t length, int64_t bpm,
            djinterop::stdx::optional<int64_t> year, std::string path,
            std::string filename, int64_t bitrate, double bpm_analyzed,
            int64_t album_art_id, int64_t file_bytes,
            djinterop::stdx::optional<std::string> title,
            djinterop::stdx::optional<std::string> artist,
            djinterop::stdx::optional<std::string> album,
            djinterop::stdx::optional<std::string> genre,
            djinterop::stdx::optional<std::string> comment,
            djinterop::stdx::optional<std::string> label,
            djinterop::stdx::optional<std::string> composer,
            djinterop::stdx::optional<std::string> remixer, int64_t key,
            int64_t rating, djinterop::stdx::optional<std::string> album_art,
            djinterop::stdx::optional<int64_t> time_last_played, bool is_played,
            std::string file_type, bool is_analyzed,
            djinterop::stdx::optional<int64_t> date_created,
            djinterop::stdx::optional<int64_t> date_added, bool is_available,
            bool is_metadata_of_packed_track_changed,
            bool is_performance_data_of_packed_track_changed,
            djinterop::stdx::optional<int64_t> played_indicator,
            bool is_metadata_imported, int64_t pdb_import_key,
            djinterop::stdx::optional<std::string> streaming_source,
            djinterop::stdx::optional<std::string> uri,
            bool is_beat_grid_locked, std::string origin_database_uuid,
            int64_t origin_track_id, const std::vector<char>& track_data,
            const std::vector<char>& overview_waveform_data,
            const std::vector<char>& beat_data,
            const std::vector<char>& quick_cues, const std::vector<char>& loops,
            djinterop::stdx::optional<int64_t> third_party_source_id,
            int64_t streaming_flags, bool explicit_lyrics)
    {
        assert(!result);

        result = track_row{
            id,
            play_order,
            length,
            bpm,
            year,
            std::move(path),
            std::move(filename),
            bitrate,
            bpm_analyzed,
            album_art_id,
            file_bytes,
            std::move(title),
            std::move(artist),
            std::move(album),
            std::move(genre),
            std::move(comment),
            std::move(label),
            std::move(composer),
            std::move(remixer),
            key,
            rating,
            std::move(album_art),
            to_time_point(time_last_played),
            is_played,
            std::move(file_type),
            is_analyzed,
            to_time_point(date_created),
            to_time_point(date_added),
            is_available,
            is_metadata_of_packed_track_changed,
            is_performance_data_of_packed_track_changed,
            played_indicator,
            is_metadata_imported,
            pdb_import_key,
            std::move(streaming_source),
            std::move(uri),
            is_beat_grid_locked,
            std::move(origin_database_uuid),
            origin_track_id,
            track_data_blob::from_blob(track_data),
            overview_waveform_data_blob::from_blob(overview_waveform_data),
            beat_data_blob::from_blob(beat_data),
            quick_cues_blob::from_blob(quick_cues),
            loops_blob::from_blob(loops),
            third_party_source_id,
            streaming_flags,
            explicit_lyrics};
    };

    return result;
}

}  // namespace djinterop::engine::v2
