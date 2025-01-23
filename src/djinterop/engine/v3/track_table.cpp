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

#include <djinterop/engine/v3/track_table.hpp>

#include <cassert>
#include <cstddef>
#include <utility>

#include <djinterop/exceptions.hpp>

#include "../../util/chrono.hpp"
#include "../engine_library_context.hpp"

namespace djinterop::engine::v3
{
namespace
{
template <typename ColumnType>
ColumnType get_column(
    sqlite::database& db, int64_t id, const std::string& column_name)
{
    std::optional<ColumnType> result;
    auto sql = "SELECT " + column_name + " FROM Track WHERE id = ?";
    db << sql << id >> [&](ColumnType value) { result = std::move(value); };

    if (result)
        return *result;

    throw track_row_id_error{"No row found for given id"};
}

template <>
std::chrono::system_clock::time_point get_column(
    sqlite::database& db, int64_t id, const std::string& column_name)
{
    auto timestamp = get_column<int64_t>(db, id, column_name);
    return djinterop::util::to_time_point(timestamp);
}

template <>
std::optional<std::chrono::system_clock::time_point> get_column(
    sqlite::database& db, int64_t id, const std::string& column_name)

{
    auto timestamp = get_column<std::optional<int64_t>>(db, id, column_name);
    return djinterop::util::to_time_point(timestamp);
}

template <typename ColumnType>
void set_column(
    sqlite::database& db, int64_t id, const std::string& column_name,
    const ColumnType& value)
{
    auto sql = "UPDATE Track SET " + column_name + " = ? WHERE id = ?";
    db << sql << value << id;

    if (db.rows_modified() > 0)
        return;

    throw track_row_id_error{"No row found for given id"};
}

template <>
void set_column(
    sqlite::database& db, int64_t id, const std::string& column_name,
    const std::chrono::system_clock::time_point& value)
{
    auto timestamp = djinterop::util::to_timestamp(value);
    set_column<int64_t>(db, id, column_name, timestamp);
}

template <>
void set_column(
    sqlite::database& db, int64_t id, const std::string& column_name,
    const std::optional<std::chrono::system_clock::time_point>& value)
{
    auto timestamp = djinterop::util::to_timestamp(value);
    set_column<std::optional<int64_t>>(db, id, column_name, timestamp);
}

}  // anonymous namespace

track_table::track_table(std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

int64_t track_table::add(const track_row& row)
{
    if (row.id != TRACK_ROW_ID_NONE)
    {
        throw track_row_id_error{
            "The provided track row already pertains to a persisted track, "
            "and so it cannot be created again"};
    }

    context_->db << "INSERT INTO Track ("
                    "playOrder, length, bpm, year, "
                    "path, filename, bitrate, bpmAnalyzed, "
                    "albumArtId, fileBytes, title, "
                    "artist, album, genre, comment, "
                    "label, composer, remixer, key, "
                    "rating, albumArt, "
                    "timeLastPlayed, isPlayed, "
                    "fileType, isAnalyzed, "
                    "dateCreated, "
                    "dateAdded, isAvailable, "
                    "isMetadataOfPackedTrackChanged, "
                    "isPerfomanceDataOfPackedTrackChanged, "  // sic
                    "playedIndicator, isMetadataImported, "
                    "pdbImportKey, streamingSource, uri, "
                    "isBeatGridLocked, originDatabaseUuid, "
                    "originTrackId, "
                    "streamingFlags, explicitLyrics, "
                    "lastEditTime) "
                    "VALUES ("
                    "?, ?, ?, ?, "
                    "?, ?, ?, ?, "
                    "?, ?, ?, "
                    "?, ?, ?, ?, "
                    "?, ?, ?, ?, "
                    "?, ?, "
                    "?, ?, "
                    "?, ?, "
                    "?, "
                    "?, ?, "
                    "?, "
                    "?, "
                    "?, ?, "
                    "?, ?, ?, "
                    "?, ?, "
                    "?, "
                    "?, ?, "
                    "?)"
                 << row.play_order << row.length << row.bpm << row.year
                 << row.path << row.filename << row.bitrate
                 << row.bpm_analyzed << row.album_art_id << row.file_bytes
                 << row.title << row.artist << row.album << row.genre
                 << row.comment << row.label << row.composer << row.remixer
                 << row.key << row.rating << row.album_art
                 << djinterop::util::to_timestamp(row.time_last_played)
                 << row.is_played << row.file_type << row.is_analyzed
                 << djinterop::util::to_timestamp(row.date_created)
                 << djinterop::util::to_timestamp(row.date_added)
                 << row.is_available
                 << row.is_metadata_of_packed_track_changed
                 << row.is_performance_data_of_packed_track_changed
                 << row.played_indicator << row.is_metadata_imported
                 << row.pdb_import_key << row.streaming_source << row.uri
                 << row.is_beat_grid_locked << row.origin_database_uuid
                 << row.origin_track_id
                 << row.streaming_flags << row.explicit_lyrics
                 << djinterop::util::to_timestamp(row.last_edit_time);

    return context_->db.last_insert_rowid();
}

std::vector<int64_t> track_table::all_ids() const
{
    std::vector<int64_t> results{};

    context_->db << "SELECT id FROM Track" >> [&](int64_t id)
    { results.push_back(id); };

    return results;
}

bool track_table::exists(int64_t id) const
{
    bool result = false;
    context_->db << "SELECT COUNT(*) FROM Track WHERE id = ?" << id >>
        [&](int64_t count)
    {
        if (count == 1)
        {
            result = true;
        }
        else if (count > 1)
        {
            throw track_database_inconsistency{
                "More than one track with the same ID", id};
        }
    };
    return result;
}

std::optional<track_row> track_table::get(int64_t id) const
{
    std::optional<track_row> result;

    context_->db
            << "SELECT id, playOrder, length, bpm, year, path, filename, "
               "bitrate, bpmAnalyzed, albumArtId, fileBytes, title, "
               "artist, album, genre, comment, label, composer, remixer, "
               "key, rating, albumArt, timeLastPlayed, isPlayed, "
               "fileType, isAnalyzed, dateCreated, dateAdded, "
               "isAvailable, isMetadataOfPackedTrackChanged, "
               "isPerfomanceDataOfPackedTrackChanged, "  // sic
               "playedIndicator, isMetadataImported, pdbImportKey, "
               "streamingSource, uri, isBeatGridLocked, "
               "originDatabaseUuid, originTrackId, "
               "streamingFlags, explicitLyrics, "
               "lastEditTime "
               "FROM Track WHERE id = ?"
            << id >>
        [&](int64_t id, std::optional<int64_t> play_order, int64_t length,
            std::optional<int64_t> bpm, std::optional<int64_t> year,
            std::string path, std::string filename,
            std::optional<int64_t> bitrate,
            std::optional<double> bpm_analyzed, int64_t album_art_id,
            std::optional<int64_t> file_bytes,
            std::optional<std::string> title,
            std::optional<std::string> artist,
            std::optional<std::string> album,
            std::optional<std::string> genre,
            std::optional<std::string> comment,
            std::optional<std::string> label,
            std::optional<std::string> composer,
            std::optional<std::string> remixer,
            std::optional<int32_t> key, int64_t rating,
            std::optional<std::string> album_art,
            std::optional<int64_t> time_last_played, bool is_played,
            std::string file_type, bool is_analyzed, int64_t date_created,
            int64_t date_added, bool is_available,
            bool is_metadata_of_packed_track_changed,
            bool is_performance_data_of_packed_track_changed,
            std::optional<int64_t> played_indicator,
            bool is_metadata_imported, int64_t pdb_import_key,
            std::optional<std::string> streaming_source,
            std::optional<std::string> uri, bool is_beat_grid_locked,
            std::string origin_database_uuid, int64_t origin_track_id,
            int64_t streaming_flags, bool explicit_lyrics,
            int64_t last_edit_time)
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
            djinterop::util::to_time_point(time_last_played),
            is_played,
            std::move(file_type),
            is_analyzed,
            djinterop::util::to_time_point(date_created),
            djinterop::util::to_time_point(date_added),
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
            streaming_flags,
            explicit_lyrics,
            djinterop::util::to_time_point(last_edit_time)};
    };

    return result;
}

std::optional<int64_t> track_table::find_id_by_path(
    const std::string& path) const
{
    std::optional<int64_t> result;

    context_->db << "SELECT id FROM Track WHERE path = ?" << path >>
        [&](int64_t id)
    {
        assert(!result);
        result = id;
    };

    return result;
}

void track_table::remove(int64_t id)
{
    // All other references to the track should automatically be cleared by
    // "ON DELETE CASCADE"
    context_->db << "DELETE FROM Track WHERE id = ?" << id;

    if (context_->db.rows_modified() == 0)
    {
        throw std::invalid_argument{"Track id not found to remove"};
    }
}

void track_table::update(const track_row& row)
{
    if (row.id == TRACK_ROW_ID_NONE)
    {
        throw track_row_id_error{
            "The track row to update does not contain a track id"};
    }

    context_->db << "UPDATE Track SET "
                    "playOrder = ?, length = ?, bpm = ?, year = ?, "
                    "path = ?, filename = ?, bitrate = ?, bpmAnalyzed = ?, "
                    "albumArtId = ?, fileBytes = ?, title = ?, "
                    "artist = ?, album = ?, genre = ?, comment = ?, "
                    "label = ?, composer = ?, remixer = ?, key = ?, "
                    "rating = ?, albumArt = ?, "
                    "timeLastPlayed = ?, isPlayed = ?, "
                    "fileType = ?, isAnalyzed = ?, "
                    "dateCreated = ?, "
                    "dateAdded = ?, isAvailable = ?, "
                    "isMetadataOfPackedTrackChanged = ?, "
                    "isPerfomanceDataOfPackedTrackChanged = ?, "  // sic
                    "playedIndicator = ?, isMetadataImported = ?, "
                    "pdbImportKey = ?, streamingSource = ?, uri = ?, "
                    "isBeatGridLocked = ?, originDatabaseUuid = ?, "
                    "originTrackId = ?, streamingFlags = ?, "
                    "explicitLyrics = ?, "
                    "lastEditTime = ?"
                    "WHERE id = ?"
                 << row.play_order << row.length << row.bpm << row.year
                 << row.path << row.filename << row.bitrate
                 << row.bpm_analyzed << row.album_art_id << row.file_bytes
                 << row.title << row.artist << row.album << row.genre
                 << row.comment << row.label << row.composer << row.remixer
                 << row.key << row.rating << row.album_art
                 << djinterop::util::to_timestamp(row.time_last_played)
                 << row.is_played << row.file_type << row.is_analyzed
                 << djinterop::util::to_timestamp(row.date_created)
                 << djinterop::util::to_timestamp(row.date_added)
                 << row.is_available
                 << row.is_metadata_of_packed_track_changed
                 << row.is_performance_data_of_packed_track_changed
                 << row.played_indicator << row.is_metadata_imported
                 << row.pdb_import_key << row.streaming_source << row.uri
                 << row.is_beat_grid_locked << row.origin_database_uuid
                 << row.origin_track_id << row.streaming_flags
                 << row.explicit_lyrics
                 << djinterop::util::to_timestamp(row.last_edit_time)
                 << row.id;
}

std::optional<int64_t> track_table::get_play_order(int64_t id)
{
    return get_column<std::optional<int64_t>>(context_->db, id, "playOrder");
}

void track_table::set_play_order(int64_t id, std::optional<int64_t> play_order)
{
    set_column<std::optional<int64_t>>(
        context_->db, id, "playOrder", play_order);
}

int64_t track_table::get_length(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "length");
}

void track_table::set_length(int64_t id, int64_t length)
{
    set_column<int64_t>(context_->db, id, "length", length);
}

std::optional<int64_t> track_table::get_bpm(int64_t id)
{
    return get_column<std::optional<int64_t>>(context_->db, id, "bpm");
}

void track_table::set_bpm(int64_t id, std::optional<int64_t> bpm)
{
    set_column<std::optional<int64_t>>(context_->db, id, "bpm", bpm);
}

std::optional<int64_t> track_table::get_year(int64_t id)
{
    return get_column<std::optional<int64_t>>(context_->db, id, "year");
}

void track_table::set_year(int64_t id, const std::optional<int64_t>& year)
{
    set_column<std::optional<int64_t>>(context_->db, id, "year", year);
}

std::string track_table::get_path(int64_t id)
{
    return get_column<std::string>(context_->db, id, "path");
}

void track_table::set_path(int64_t id, const std::string& path)
{
    set_column<std::string>(context_->db, id, "path", path);
}

std::string track_table::get_filename(int64_t id)
{
    return get_column<std::string>(context_->db, id, "filename");
}

void track_table::set_filename(int64_t id, const std::string& filename)
{
    set_column<std::string>(context_->db, id, "filename", filename);
}

std::optional<int64_t> track_table::get_bitrate(int64_t id)
{
    return get_column<std::optional<int64_t>>(context_->db, id, "bitrate");
}

void track_table::set_bitrate(int64_t id, std::optional<int64_t> bitrate)
{
    set_column<std::optional<int64_t>>(context_->db, id, "bitrate", bitrate);
}

std::optional<double> track_table::get_bpm_analyzed(int64_t id)
{
    return get_column<std::optional<double>>(context_->db, id, "bpmAnalyzed");
}

void track_table::set_bpm_analyzed(
    int64_t id, std::optional<double> bpm_analyzed)
{
    set_column<std::optional<double>>(
        context_->db, id, "bpmAnalyzed", bpm_analyzed);
}

int64_t track_table::get_album_art_id(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "albumArtId");
}

void track_table::set_album_art_id(int64_t id, int64_t album_art_id)
{
    set_column<int64_t>(context_->db, id, "albumArtId", album_art_id);
}

std::optional<int64_t> track_table::get_file_bytes(int64_t id)
{
    return get_column<std::optional<int64_t>>(context_->db, id, "fileBytes");
}

void track_table::set_file_bytes(int64_t id, std::optional<int64_t> file_bytes)
{
    set_column<std::optional<int64_t>>(
        context_->db, id, "fileBytes", file_bytes);
}

std::optional<std::string> track_table::get_title(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "title");
}

void track_table::set_title(
    int64_t id, const std::optional<std::string>& title)
{
    set_column<std::optional<std::string>>(context_->db, id, "title", title);
}

std::optional<std::string> track_table::get_artist(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "artist");
}

void track_table::set_artist(
    int64_t id, const std::optional<std::string>& artist)
{
    set_column<std::optional<std::string>>(context_->db, id, "artist", artist);
}

std::optional<std::string> track_table::get_album(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "album");
}

void track_table::set_album(
    int64_t id, const std::optional<std::string>& album)
{
    set_column<std::optional<std::string>>(context_->db, id, "album", album);
}

std::optional<std::string> track_table::get_genre(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "genre");
}

void track_table::set_genre(
    int64_t id, const std::optional<std::string>& genre)
{
    set_column<std::optional<std::string>>(context_->db, id, "genre", genre);
}

std::optional<std::string> track_table::get_comment(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "comment");
}

void track_table::set_comment(
    int64_t id, const std::optional<std::string>& comment)
{
    set_column<std::optional<std::string>>(
        context_->db, id, "comment", comment);
}

std::optional<std::string> track_table::get_label(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "label");
}

void track_table::set_label(
    int64_t id, const std::optional<std::string>& label)
{
    set_column<std::optional<std::string>>(context_->db, id, "label", label);
}

std::optional<std::string> track_table::get_composer(int64_t id)
{
    return get_column<std::optional<std::string>>(
        context_->db, id, "composer");
}

void track_table::set_composer(
    int64_t id, const std::optional<std::string>& composer)
{
    set_column<std::optional<std::string>>(
        context_->db, id, "composer", composer);
}

std::optional<std::string> track_table::get_remixer(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "remixer");
}

void track_table::set_remixer(
    int64_t id, const std::optional<std::string>& remixer)
{
    set_column<std::optional<std::string>>(
        context_->db, id, "remixer", remixer);
}

std::optional<int32_t> track_table::get_key(int64_t id)
{
    return get_column<std::optional<int32_t>>(context_->db, id, "key");
}

void track_table::set_key(int64_t id, std::optional<int32_t> key)
{
    set_column<std::optional<int32_t>>(context_->db, id, "key", key);
}

int64_t track_table::get_rating(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "rating");
}

void track_table::set_rating(int64_t id, int64_t rating)
{
    set_column<int64_t>(context_->db, id, "rating", rating);
}

std::optional<std::string> track_table::get_album_art(int64_t id)
{
    return get_column<std::optional<std::string>>(
        context_->db, id, "albumArt");
}

void track_table::set_album_art(
    int64_t id, const std::optional<std::string>& album_art)
{
    set_column<std::optional<std::string>>(
        context_->db, id, "albumArt", album_art);
}

std::optional<std::chrono::system_clock::time_point>
track_table::get_time_last_played(int64_t id)
{
    return get_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "timeLastPlayed");
}

void track_table::set_time_last_played(
    int64_t id, const std::optional<std::chrono::system_clock::time_point>&
                    time_last_played)
{
    set_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "timeLastPlayed", time_last_played);
}

bool track_table::get_is_played(int64_t id)
{
    return get_column<bool>(context_->db, id, "isPlayed");
}

void track_table::set_is_played(int64_t id, bool is_played)
{
    set_column<bool>(context_->db, id, "isPlayed", is_played);
}

std::string track_table::get_file_type(int64_t id)
{
    return get_column<std::string>(context_->db, id, "fileType");
}

void track_table::set_file_type(int64_t id, const std::string& file_type)
{
    set_column<std::string>(context_->db, id, "fileType", file_type);
}

bool track_table::get_is_analyzed(int64_t id)
{
    return get_column<bool>(context_->db, id, "isAnalyzed");
}

void track_table::set_is_analyzed(int64_t id, bool is_analyzed)
{
    set_column<bool>(context_->db, id, "isAnalyzed", is_analyzed);
}

std::optional<std::chrono::system_clock::time_point>
track_table::get_date_created(int64_t id)
{
    return get_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "dateCreated");
}

void track_table::set_date_created(
    int64_t id,
    const std::optional<std::chrono::system_clock::time_point>& date_created)
{
    set_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "dateCreated", date_created);
}

std::optional<std::chrono::system_clock::time_point>
track_table::get_date_added(int64_t id)
{
    return get_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "dateAdded");
}

void track_table::set_date_added(
    int64_t id,
    const std::optional<std::chrono::system_clock::time_point>& date_added)
{
    set_column<std::optional<std::chrono::system_clock::time_point>>(
        context_->db, id, "dateAdded", date_added);
}

bool track_table::get_is_available(int64_t id)
{
    return get_column<bool>(context_->db, id, "isAvailable");
}

void track_table::set_is_available(int64_t id, bool is_available)
{
    set_column<bool>(context_->db, id, "isAvailable", is_available);
}

bool track_table::get_is_metadata_of_packed_track_changed(int64_t id)
{
    return get_column<bool>(context_->db, id, "isMetadataOfPackedTrackChanged");
}

void track_table::set_is_metadata_of_packed_track_changed(
    int64_t id, bool is_metadata_of_packed_track_changed)
{
    set_column<bool>(
        context_->db, id, "isMetadataOfPackedTrackChanged",
        is_metadata_of_packed_track_changed);
}

bool track_table::get_is_performance_data_of_packed_track_changed(int64_t id)
{
    return get_column<bool>(
        context_->db, id, "isPerfomanceDataOfPackedTrackChanged");  // sic
}

void track_table::set_is_performance_data_of_packed_track_changed(
    int64_t id, bool is_performance_data_of_packed_track_changed)
{
    set_column<bool>(
        context_->db, id, "isPerfomanceDataOfPackedTrackChanged",  // sic
        is_performance_data_of_packed_track_changed);
}

std::optional<int64_t> track_table::get_played_indicator(int64_t id)
{
    return get_column<std::optional<int64_t>>(
        context_->db, id, "playedIndicator");
}

void track_table::set_played_indicator(
    int64_t id, const std::optional<int64_t>& played_indicator)
{
    set_column<std::optional<int64_t>>(
        context_->db, id, "playedIndicator", played_indicator);
}

bool track_table::get_is_metadata_imported(int64_t id)
{
    return get_column<bool>(context_->db, id, "isMetadataImported");
}

void track_table::set_is_metadata_imported(
    int64_t id, bool is_metadata_imported)
{
    set_column<bool>(
        context_->db, id, "isMetadataImported", is_metadata_imported);
}

int64_t track_table::get_pdb_import_key(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "pdbImportKey");
}

void track_table::set_pdb_import_key(int64_t id, int64_t pdb_import_key)
{
    set_column<int64_t>(context_->db, id, "pdbImportKey", pdb_import_key);
}

std::optional<std::string> track_table::get_streaming_source(int64_t id)
{
    return get_column<std::optional<std::string>>(
        context_->db, id, "streamingSource");
}

void track_table::set_streaming_source(
    int64_t id, const std::optional<std::string>& streaming_source)
{
    set_column<std::optional<std::string>>(
        context_->db, id, "streamingSource", streaming_source);
}

std::optional<std::string> track_table::get_uri(int64_t id)
{
    return get_column<std::optional<std::string>>(context_->db, id, "uri");
}

void track_table::set_uri(int64_t id, const std::optional<std::string>& uri)
{
    set_column<std::optional<std::string>>(context_->db, id, "uri", uri);
}

bool track_table::get_is_beat_grid_locked(int64_t id)
{
    return get_column<bool>(context_->db, id, "isBeatGridLocked");
}

void track_table::set_is_beat_grid_locked(int64_t id, bool is_beat_grid_locked)
{
    set_column<bool>(context_->db, id, "isBeatGridLocked", is_beat_grid_locked);
}

std::string track_table::get_origin_database_uuid(int64_t id)
{
    return get_column<std::string>(context_->db, id, "originDatabaseUuid");
}

void track_table::set_origin_database_uuid(
    int64_t id, const std::string& origin_database_uuid)
{
    set_column<std::string>(
        context_->db, id, "originDatabaseUuid", origin_database_uuid);
}

int64_t track_table::get_origin_track_id(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "originTrackId");
}

void track_table::set_origin_track_id(int64_t id, int64_t origin_track_id)
{
    set_column<int64_t>(context_->db, id, "originTrackId", origin_track_id);
}

int64_t track_table::get_streaming_flags(int64_t id)
{
    return get_column<int64_t>(context_->db, id, "streamingFlags");
}

void track_table::set_streaming_flags(int64_t id, int64_t streaming_flags)
{
    set_column<int64_t>(context_->db, id, "streamingFlags", streaming_flags);
}

bool track_table::get_explicit_lyrics(int64_t id)
{
    return get_column<bool>(context_->db, id, "explicitLyrics");
}

void track_table::set_explicit_lyrics(int64_t id, bool explicit_lyrics)
{
    set_column<bool>(context_->db, id, "explicitLyrics", explicit_lyrics);
}

std::chrono::system_clock::time_point track_table::get_last_edit_time(
    int64_t id)
{
    if (context_->schema < engine_schema::schema_2_20_3)
        throw djinterop::unsupported_operation{
            "The `lastEditTime` column is not available for this "
            "database version"};

    return get_column<std::chrono::system_clock::time_point>(
        context_->db, id, "lastEditTime");
}

void track_table::set_last_edit_time(
    int64_t id, std::chrono::system_clock::time_point last_edit_time)
{
    if (context_->schema < engine_schema::schema_2_20_3)
        throw djinterop::unsupported_operation{
            "The `lastEditTime` column is not available for this "
            "database version"};

    set_column<std::chrono::system_clock::time_point>(
        context_->db, id, "lastEditTime", last_edit_time);
}

}  // namespace djinterop::engine::v3
