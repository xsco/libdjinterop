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

#include "engine_storage.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/exceptions.hpp>

#include "../../util/filesystem.hpp"
#include "../engine_library_dir_utils.hpp"
#include "../schema/schema.hpp"

namespace djinterop::engine::v1
{
namespace
{
engine_storage load_existing(const std::string& directory)
{
    auto db = load_legacy_sqlite_database(directory);

    const auto schema = schema::detect_schema(db, "music");
    return engine_storage{directory, schema, db};
}

}  // anonymous namespace

engine_storage::engine_storage(const std::string& directory) :
    engine_storage{load_existing(directory)}
{
}

engine_storage::engine_storage(
    const std::string& directory, const engine_schema& schema,
    sqlite::database db) :
    directory{directory},
    db{std::move(db)}, schema{schema}
{
}

std::shared_ptr<engine_storage> engine_storage::create(
    const std::string& directory, const engine_schema& schema)
{
    auto db = create_legacy_sqlite_database(directory);

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return std::shared_ptr<engine_storage>{
        new engine_storage{directory, schema, std::move(db)}};
}

std::shared_ptr<engine_storage> engine_storage::create_temporary(
    const engine_schema& schema)
{
    auto db = create_temporary_legacy_sqlite_database();

    // Create the desired schema on the new database.
    auto schema_creator = schema::make_schema_creator_validator(schema);
    schema_creator->create(db);

    return std::shared_ptr<engine_storage>{
        new engine_storage{":memory:", schema, std::move(db)}};
}

int64_t engine_storage::create_track(
    std::optional<int64_t> play_order, std::optional<int64_t> length,
    std::optional<int64_t> length_calculated, std::optional<int64_t> bpm,
    std::optional<int64_t> year,
    const std::optional<std::string>& relative_path,
    const std::optional<std::string>& filename,
    std::optional<int64_t> bitrate, std::optional<double> bpm_analyzed,
    std::optional<int64_t> track_type,
    std::optional<int64_t> is_external_track,
    const std::optional<std::string>& uuid_of_external_database,
    std::optional<int64_t> id_track_in_external_database,
    std::optional<int64_t> album_art_id, std::optional<int64_t> file_bytes,
    std::optional<int64_t> pdb_import_key,
    const std::optional<std::string>& uri,
    std::optional<int64_t> is_beatgrid_locked)
{
    if (schema >= engine_schema::schema_1_18_0_desktop)
    {
        db << "INSERT INTO Track (playOrder, length, "
              "lengthCalculated, bpm, year, path, filename, bitrate, "
              "bpmAnalyzed, trackType, isExternalTrack, "
              "uuidOfExternalDatabase, idTrackInExternalDatabase, "
              "idAlbumArt, fileBytes, pdbImportKey, uri, "
              "isBeatGridLocked) "
              "VALUES (?, ?, "
              "?, ?, ?, ?, ?, ?, "
              "?, ?, ?, "
              "?, ?, "
              "?, ?, ?, ?, "
              "?)"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << file_bytes           // Added in 1.15.0
           << pdb_import_key       // Added in 1.7.1
           << uri                  // Added in 1.15.0
           << is_beatgrid_locked;  // Added in 1.18.0
    }
    else if (schema >= engine_schema::schema_1_15_0)
    {
        db << "INSERT INTO Track (playOrder, length, "
              "lengthCalculated, bpm, year, path, filename, bitrate, "
              "bpmAnalyzed, trackType, isExternalTrack, "
              "uuidOfExternalDatabase, idTrackInExternalDatabase, "
              "idAlbumArt, fileBytes, pdbImportKey, uri) "
              "VALUES (?, ?, "
              "?, ?, ?, ?, ?, ?, "
              "?, ?, ?, "
              "?, ?, "
              "?, ?, ?, ?)"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << file_bytes      // Added in 1.15.0
           << pdb_import_key  // Added in 1.7.1
           << uri;            // Added in 1.15.0
    }
    else if (schema >= engine_schema::schema_1_7_1)
    {
        db << "INSERT INTO Track (playOrder, length, "
              "lengthCalculated, bpm, year, path, filename, bitrate, "
              "bpmAnalyzed, trackType, isExternalTrack, "
              "uuidOfExternalDatabase, idTrackInExternalDatabase, "
              "idAlbumArt, pdbImportKey) "
              "VALUES (?, ?, "
              "?, ?, ?, ?, ?, ?, "
              "?, ?, ?, "
              "?, ?, "
              "?, ?)"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << pdb_import_key;  // Added in 1.7.1
    }
    else
    {
        db << "INSERT INTO Track (playOrder, length, "
              "lengthCalculated, bpm, year, path, filename, bitrate, "
              "bpmAnalyzed, trackType, isExternalTrack, "
              "uuidOfExternalDatabase, idTrackInExternalDatabase, "
              "idAlbumArt) "
              "VALUES (?, ?, "
              "?, ?, ?, ?, ?, ?, "
              "?, ?, ?, "
              "?, ?, "
              "?)"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id;
    }

    return db.last_insert_rowid();
}

track_row engine_storage::get_track(int64_t id)
{
    std::optional<track_row> result;
    if (schema >= engine_schema::schema_1_18_0_desktop)
    {
        db << ("SELECT playOrder, length, lengthCalculated, bpm, year, path, "
               "filename, bitrate, bpmAnalyzed, trackType, isExternalTrack, "
               "uuidOfExternalDatabase, idTrackInExternalDatabase, idAlbumArt, "
               "fileBytes, pdbImportKey, uri, isBeatGridLocked "
               "FROM Track WHERE id = ?")
           << id >>
            [&](std::optional<int64_t> play_order,
                std::optional<int64_t> length,
                std::optional<int64_t> length_calculated,
                std::optional<int64_t> bpm, std::optional<int64_t> year,
                std::optional<std::string> relative_path,
                std::optional<std::string> filename,
                std::optional<int64_t> bitrate,
                std::optional<double> bpm_analyzed,
                std::optional<int64_t> track_type,
                std::optional<int64_t> is_external_track,
                std::optional<std::string> uuid_of_external_database,
                std::optional<int64_t> id_track_in_external_database,
                std::optional<int64_t> album_art_id,
                std::optional<int64_t> file_bytes,
                std::optional<int64_t> pdb_import_key,
                std::optional<std::string> uri,
                std::optional<int64_t> is_beatgrid_locked) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }
                result = track_row{
                    play_order,
                    length,
                    length_calculated,
                    bpm,
                    year,
                    std::move(relative_path),
                    std::move(filename),
                    bitrate,
                    bpm_analyzed,
                    track_type,
                    is_external_track,
                    std::move(uuid_of_external_database),
                    id_track_in_external_database,
                    album_art_id,
                    file_bytes,
                    pdb_import_key,
                    std::move(uri),
                    is_beatgrid_locked};
            };
    }
    else if (schema >= engine_schema::schema_1_15_0)
    {
        db << ("SELECT playOrder, length, lengthCalculated, bpm, year, path, "
               "filename, bitrate, bpmAnalyzed, trackType, isExternalTrack, "
               "uuidOfExternalDatabase, idTrackInExternalDatabase, idAlbumArt, "
               "fileBytes, pdbImportKey, uri "
               "FROM Track WHERE id = ?")
           << id >>
            [&](std::optional<int64_t> play_order,
                std::optional<int64_t> length,
                std::optional<int64_t> length_calculated,
                std::optional<int64_t> bpm, std::optional<int64_t> year,
                std::optional<std::string> relative_path,
                std::optional<std::string> filename,
                std::optional<int64_t> bitrate,
                std::optional<double> bpm_analyzed,
                std::optional<int64_t> track_type,
                std::optional<int64_t> is_external_track,
                std::optional<std::string> uuid_of_external_database,
                std::optional<int64_t> id_track_in_external_database,
                std::optional<int64_t> album_art_id,
                std::optional<int64_t> file_bytes,
                std::optional<int64_t> pdb_import_key,
                std::optional<std::string> uri) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }
                result = track_row{
                    play_order,
                    length,
                    length_calculated,
                    bpm,
                    year,
                    std::move(relative_path),
                    std::move(filename),
                    bitrate,
                    bpm_analyzed,
                    track_type,
                    is_external_track,
                    std::move(uuid_of_external_database),
                    id_track_in_external_database,
                    album_art_id,
                    file_bytes,
                    pdb_import_key,
                    std::move(uri)};
            };
    }
    else if (schema >= engine_schema::schema_1_7_1)
    {
        db << ("SELECT playOrder, length, lengthCalculated, bpm, year, path, "
               "filename, bitrate, bpmAnalyzed, trackType, isExternalTrack, "
               "uuidOfExternalDatabase, idTrackInExternalDatabase, idAlbumArt, "
               "pdbImportKey "
               "FROM Track WHERE id = ?")
           << id >>
            [&](std::optional<int64_t> play_order,
                std::optional<int64_t> length,
                std::optional<int64_t> length_calculated,
                std::optional<int64_t> bpm, std::optional<int64_t> year,
                std::optional<std::string> relative_path,
                std::optional<std::string> filename,
                std::optional<int64_t> bitrate,
                std::optional<double> bpm_analyzed,
                std::optional<int64_t> track_type,
                std::optional<int64_t> is_external_track,
                std::optional<std::string> uuid_of_external_database,
                std::optional<int64_t> id_track_in_external_database,
                std::optional<int64_t> album_art_id,
                std::optional<int64_t> pdb_import_key) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }
                result = track_row{
                    play_order,
                    length,
                    length_calculated,
                    bpm,
                    year,
                    std::move(relative_path),
                    std::move(filename),
                    bitrate,
                    bpm_analyzed,
                    track_type,
                    is_external_track,
                    std::move(uuid_of_external_database),
                    id_track_in_external_database,
                    album_art_id,
                    std::nullopt,  // file_bytes
                    pdb_import_key};
            };
    }
    else
    {
        db << ("SELECT playOrder, length, lengthCalculated, bpm, year, path, "
               "filename, bitrate, bpmAnalyzed, trackType, isExternalTrack, "
               "uuidOfExternalDatabase, idTrackInExternalDatabase, idAlbumArt "
               "FROM Track WHERE id = ?")
           << id >>
            [&](std::optional<int64_t> play_order,
                std::optional<int64_t> length,
                std::optional<int64_t> length_calculated,
                std::optional<int64_t> bpm, std::optional<int64_t> year,
                std::optional<std::string> relative_path,
                std::optional<std::string> filename,
                std::optional<int64_t> bitrate,
                std::optional<double> bpm_analyzed,
                std::optional<int64_t> track_type,
                std::optional<int64_t> is_external_track,
                std::optional<std::string> uuid_of_external_database,
                std::optional<int64_t> id_track_in_external_database,
                std::optional<int64_t> album_art_id) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }
                result = track_row{
                    play_order,
                    length,
                    length_calculated,
                    bpm,
                    year,
                    std::move(relative_path),
                    std::move(filename),
                    bitrate,
                    bpm_analyzed,
                    track_type,
                    is_external_track,
                    std::move(uuid_of_external_database),
                    id_track_in_external_database,
                    album_art_id};
            };
    }

    if (!result)
    {
        throw track_deleted{id};
    }
    return *result;
}

/// Update a row in the `Track` table.
void engine_storage::update_track(
    int64_t id, std::optional<int64_t> play_order,
    std::optional<int64_t> length, std::optional<int64_t> length_calculated,
    std::optional<int64_t> bpm, std::optional<int64_t> year,
    const std::optional<std::string>& relative_path,
    const std::optional<std::string>& filename,
    std::optional<int64_t> bitrate, std::optional<double> bpm_analyzed,
    std::optional<int64_t> track_type,
    std::optional<int64_t> is_external_track,
    const std::optional<std::string>& uuid_of_external_database,
    std::optional<int64_t> id_track_in_external_database,
    std::optional<int64_t> album_art_id, std::optional<int64_t> file_bytes,
    std::optional<int64_t> pdb_import_key,
    const std::optional<std::string>& uri,
    std::optional<int64_t> is_beatgrid_locked)
{
    if (schema >= engine_schema::schema_1_18_0_desktop)
    {
        db << "UPDATE Track SET "
              "playOrder = ?, length = ?, lengthCalculated = ?, bpm = ?, "
              "year = ?, path = ?, filename = ?, bitrate = ?, bpmAnalyzed = ?, "
              "trackType = ?, isExternalTrack = ?, uuidOfExternalDatabase = ?, "
              "idTrackInExternalDatabase = ?, idAlbumArt = ?, "
              "fileBytes = ?, pdbImportKey = ?, uri = ?, isBeatGridLocked = ? "
              "WHERE id = ?"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << file_bytes          // Added in 1.15.0
           << pdb_import_key      // Added in 1.7.1
           << uri                 // Added in 1.15.0
           << is_beatgrid_locked  // Added in 1.18.0
           << id;
    }
    else if (schema >= engine_schema::schema_1_15_0)
    {
        db << "UPDATE Track SET "
              "playOrder = ?, length = ?, lengthCalculated = ?, bpm = ?, "
              "year = ?, path = ?, filename = ?, bitrate = ?, bpmAnalyzed = ?, "
              "trackType = ?, isExternalTrack = ?, uuidOfExternalDatabase = ?, "
              "idTrackInExternalDatabase = ?, idAlbumArt = ?, "
              "fileBytes = ?, pdbImportKey = ?, uri = ? "
              "WHERE id = ?"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << file_bytes      // Added in 1.15.0
           << pdb_import_key  // Added in 1.7.1
           << uri             // Added in 1.15.0
           << id;
    }
    else if (schema >= engine_schema::schema_1_7_1)
    {
        db << "UPDATE Track SET "
              "playOrder = ?, length = ?, lengthCalculated = ?, bpm = ?, "
              "year = ?, path = ?, filename = ?, bitrate = ?, bpmAnalyzed = ?, "
              "trackType = ?, isExternalTrack = ?, uuidOfExternalDatabase = ?, "
              "idTrackInExternalDatabase = ?, idAlbumArt = ?, "
              "pdbImportKey = ? "
              "WHERE id = ?"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id
           << pdb_import_key  // Added in 1.7.1
           << id;
    }
    else
    {
        db << "UPDATE Track SET "
              "playOrder = ?, length = ?, lengthCalculated = ?, bpm = ?, "
              "year = ?, path = ?, filename = ?, bitrate = ?, bpmAnalyzed = ?, "
              "trackType = ?, isExternalTrack = ?, uuidOfExternalDatabase = ?, "
              "idTrackInExternalDatabase = ?, idAlbumArt = ? "
              "WHERE id = ?"
           << play_order << length << length_calculated << bpm << year
           << relative_path << filename << bitrate << bpm_analyzed << track_type
           << is_external_track << uuid_of_external_database
           << id_track_in_external_database << album_art_id << id;
    }
}

std::vector<meta_data_row> engine_storage::get_all_meta_data(int64_t id)
{
    std::vector<meta_data_row> results;
    db << "SELECT id, type, text FROM MetaData "
          "WHERE id = ? AND text IS NOT NULL"
       << id >>
        [&results](int64_t id, int64_t type, std::string text) {
            results.push_back(
                {id, static_cast<metadata_str_type>(type), std::move(text)});
        };

    return results;
}

std::optional<std::string> engine_storage::get_meta_data(
    int64_t id, metadata_str_type type)
{
    std::optional<std::string> result;
    db << "SELECT text FROM MetaData WHERE id = ? AND "
          "type = ? AND text IS NOT NULL"
       << id << static_cast<int64_t>(type) >>
        [&](std::string text) {
            if (!result)
            {
                result = std::move(text);
            }
            else
            {
                throw track_database_inconsistency{
                    "More than one MetaData entry of the same type for the "
                    "same track",
                    id};
            }
        };
    return result;
}

void engine_storage::set_meta_data(
    int64_t id, metadata_str_type type, std::optional<std::string> content)
{
    if (content)
    {
        set_meta_data(id, type, std::string{*content});
    }
    else
    {
        db << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)" << id
           << static_cast<int64_t>(type) << nullptr;
    }
}

void engine_storage::set_meta_data(
    int64_t id, metadata_str_type type, const std::string& content)
{
    db << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)" << id
       << static_cast<int64_t>(type) << content;
}

void engine_storage::set_meta_data(
    int64_t id, const std::optional<std::string>& title,
    const std::optional<std::string>& artist,
    const std::optional<std::string>& album,
    const std::optional<std::string>& genre,
    const std::optional<std::string>& comment,
    const std::optional<std::string>& publisher,
    const std::optional<std::string>& composer,
    const std::optional<std::string>& duration_mm_ss,
    const std::optional<std::string>& ever_played,
    const std::optional<std::string>& file_extension)
{
    // Note that rows are created even for null values.
    std::optional<std::string> no_value;
    if (schema >= engine_schema::schema_1_15_0)
    {
        // A new unknown entry of type 17 may appear from 1.15.0 onwards.
        db << "INSERT OR REPLACE INTO MetaData(id, type, text) VALUES "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?)"
           << id << static_cast<int64_t>(metadata_str_type::title) << title
           << id << static_cast<int64_t>(metadata_str_type::artist) << artist
           << id << static_cast<int64_t>(metadata_str_type::album) << album
           << id << static_cast<int64_t>(metadata_str_type::genre) << genre
           << id << static_cast<int64_t>(metadata_str_type::comment) << comment
           << id << static_cast<int64_t>(metadata_str_type::publisher)
           << publisher << id
           << static_cast<int64_t>(metadata_str_type::composer) << composer
           << id << static_cast<int64_t>(metadata_str_type::unknown_8)
           << no_value << id
           << static_cast<int64_t>(metadata_str_type::unknown_9) << no_value
           << id << static_cast<int64_t>(metadata_str_type::duration_mm_ss)
           << duration_mm_ss << id
           << static_cast<int64_t>(metadata_str_type::ever_played)
           << ever_played << id
           << static_cast<int64_t>(metadata_str_type::file_extension)
           << file_extension << id
           << static_cast<int64_t>(metadata_str_type::unknown_15) << "1" << id
           << static_cast<int64_t>(metadata_str_type::unknown_16) << "1" << id
           << static_cast<int64_t>(metadata_str_type::unknown_17) << no_value;
    }
    else
    {
        db << "INSERT OR REPLACE INTO MetaData(id, type, text) VALUES "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?)"
           << id << static_cast<int64_t>(metadata_str_type::title) << title
           << id << static_cast<int64_t>(metadata_str_type::artist) << artist
           << id << static_cast<int64_t>(metadata_str_type::album) << album
           << id << static_cast<int64_t>(metadata_str_type::genre) << genre
           << id << static_cast<int64_t>(metadata_str_type::comment) << comment
           << id << static_cast<int64_t>(metadata_str_type::publisher)
           << publisher << id
           << static_cast<int64_t>(metadata_str_type::composer) << composer
           << id << static_cast<int64_t>(metadata_str_type::unknown_8)
           << no_value << id
           << static_cast<int64_t>(metadata_str_type::unknown_9) << no_value
           << id << static_cast<int64_t>(metadata_str_type::duration_mm_ss)
           << duration_mm_ss << id
           << static_cast<int64_t>(metadata_str_type::ever_played)
           << ever_played << id
           << static_cast<int64_t>(metadata_str_type::file_extension)
           << file_extension << id
           << static_cast<int64_t>(metadata_str_type::unknown_15) << "1" << id
           << static_cast<int64_t>(metadata_str_type::unknown_16) << "1";
    }
}

std::vector<meta_data_integer_row> engine_storage::get_all_meta_data_integer(
    int64_t id)
{
    std::vector<meta_data_integer_row> results;
    db << "SELECT id, type, value FROM MetaDataInteger "
          "WHERE id = ? AND value IS NOT NULL"
       << id >>
        [&results](int64_t id, int64_t type, int64_t value) {
            results.push_back(
                {id, static_cast<metadata_int_type>(type), value});
        };

    return results;
}

std::optional<int64_t> engine_storage::get_meta_data_integer(
    int64_t id, metadata_int_type type)
{
    std::optional<int64_t> result;
    db << "SELECT value FROM MetaDataInteger WHERE id = "
          "? AND type = ? AND value IS NOT NULL"
       << id << static_cast<int64_t>(type) >>
        [&](int64_t value) {
            if (!result)
            {
                result = value;
            }
            else
            {
                throw track_database_inconsistency{
                    "More than one MetaDataInteger entry of the same type "
                    "for the same track",
                    id};
            }
        };
    return result;
}

void engine_storage::set_meta_data_integer(
    int64_t id, metadata_int_type type, std::optional<int64_t> content)
{
    db << "REPLACE INTO MetaDataInteger (id, type, value) VALUES (?, ?, ?)"
       << id << static_cast<int64_t>(type) << content;
}

void engine_storage::set_meta_data_integer(
    int64_t id, std::optional<int64_t> musical_key,
    std::optional<int64_t> rating,
    std::optional<int64_t> last_played_timestamp,
    std::optional<int64_t> last_modified_timestamp,
    std::optional<int64_t> last_accessed_timestamp,
    std::optional<int64_t> last_play_hash)
{
    // Note that rows are created even for null values.
    //
    // Hardware players have been observed to record integer metadata in the
    // order 4, 5, 1, 2, 3, 6, 8, 7, 9, 10, 11, for reasons unknown.  The code
    // below replicates this order for maximum compatibility.
    std::optional<int64_t> no_value;
    if (schema >= engine_schema::schema_1_11_1)
    {
        // A new unknown entry of type 12 may appear from 1.11.1 onwards.
        db << "INSERT OR REPLACE INTO MetaDataInteger (id, type, value) VALUES "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?)"
           << id << static_cast<int64_t>(metadata_int_type::musical_key)
           << musical_key << id
           << static_cast<int64_t>(metadata_int_type::rating) << rating << id
           << static_cast<int64_t>(metadata_int_type::last_played_ts)
           << last_played_timestamp << id
           << static_cast<int64_t>(metadata_int_type::last_modified_ts)
           << last_modified_timestamp << id
           << static_cast<int64_t>(metadata_int_type::last_accessed_ts)
           << last_accessed_timestamp << id
           << static_cast<int64_t>(metadata_int_type::unknown_6) << no_value
           << id << static_cast<int64_t>(metadata_int_type::unknown_8)
           << no_value << id
           << static_cast<int64_t>(metadata_int_type::unknown_7) << no_value
           << id << static_cast<int64_t>(metadata_int_type::unknown_9)
           << no_value << id
           << static_cast<int64_t>(metadata_int_type::last_play_hash)
           << last_play_hash << id
           << static_cast<int64_t>(metadata_int_type::unknown_11) << 1 << id
           << static_cast<int64_t>(metadata_int_type::unknown_12) << 1;
    }
    else
    {
        db << "INSERT OR REPLACE INTO MetaDataInteger (id, type, value) VALUES "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?), "
              "(?, ?, ?)"
           << id << static_cast<int64_t>(metadata_int_type::musical_key)
           << musical_key << id
           << static_cast<int64_t>(metadata_int_type::rating) << rating << id
           << static_cast<int64_t>(metadata_int_type::last_played_ts)
           << last_played_timestamp << id
           << static_cast<int64_t>(metadata_int_type::last_modified_ts)
           << last_modified_timestamp << id
           << static_cast<int64_t>(metadata_int_type::last_accessed_ts)
           << last_accessed_timestamp << id
           << static_cast<int64_t>(metadata_int_type::unknown_6) << no_value
           << id << static_cast<int64_t>(metadata_int_type::unknown_8)
           << no_value << id
           << static_cast<int64_t>(metadata_int_type::unknown_7) << no_value
           << id << static_cast<int64_t>(metadata_int_type::unknown_9)
           << no_value << id
           << static_cast<int64_t>(metadata_int_type::last_play_hash)
           << last_play_hash << id
           << static_cast<int64_t>(metadata_int_type::unknown_11) << 1;
    }
}

/// Remove an existing entry in the `PerformanceData` table, if it exists.
void engine_storage::clear_performance_data(int64_t id)
{
    db << "DELETE FROM PerformanceData WHERE id = ?" << id;
}

performance_data_row engine_storage::get_performance_data(int64_t id)
{
    std::optional<performance_data_row> result;
    if (schema >= engine_schema::schema_1_11_1)
    {
        db << "SELECT id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues, hasRekordboxValues, hasTraktorValues "
              "FROM PerformanceData WHERE id = ?"
           << id >>
            [&](int64_t id, int64_t is_analyzed, int64_t is_rendered,
                const std::vector<std::byte>& track_data_blob,
                const std::vector<std::byte>& high_res_waveform_data_blob,
                const std::vector<std::byte>& overview_waveform_data_blob,
                const std::vector<std::byte>& beat_data_blob,
                const std::vector<std::byte>& quick_cues_data_blob,
                const std::vector<std::byte>& loops_data_blob,
                int64_t has_serato_values, int64_t has_rekordbox_values,
                int64_t has_traktor_values) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }

                result = performance_data_row{
                    id,
                    is_analyzed,
                    is_rendered,
                    track_data::decode(track_data_blob),
                    high_res_waveform_data::decode(high_res_waveform_data_blob),
                    overview_waveform_data::decode(overview_waveform_data_blob),
                    beat_data::decode(beat_data_blob),
                    quick_cues_data::decode(quick_cues_data_blob),
                    loops_data::decode(loops_data_blob),
                    has_serato_values,
                    has_rekordbox_values,
                    has_traktor_values};
            };
    }
    else if (schema >= engine_schema::schema_1_7_1)
    {
        db << "SELECT id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues, hasRekordboxValues "
              "FROM PerformanceData WHERE id = ?"
           << id >>
            [&](int64_t id, int64_t is_analyzed, int64_t is_rendered,
                const std::vector<std::byte>& track_data_blob,
                const std::vector<std::byte>& high_res_waveform_data_blob,
                const std::vector<std::byte>& overview_waveform_data_blob,
                const std::vector<std::byte>& beat_data_blob,
                const std::vector<std::byte>& quick_cues_data_blob,
                const std::vector<std::byte>& loops_data_blob,
                int64_t has_serato_values, int64_t has_rekordbox_values) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }

                result = performance_data_row{
                    id,
                    is_analyzed,
                    is_rendered,
                    track_data::decode(track_data_blob),
                    high_res_waveform_data::decode(high_res_waveform_data_blob),
                    overview_waveform_data::decode(overview_waveform_data_blob),
                    beat_data::decode(beat_data_blob),
                    quick_cues_data::decode(quick_cues_data_blob),
                    loops_data::decode(loops_data_blob),
                    has_serato_values,
                    has_rekordbox_values,
                };
            };
    }
    else
    {
        db << "SELECT id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues "
              "FROM PerformanceData WHERE id = ?"
           << id >>
            [&](int64_t id, int64_t is_analyzed, int64_t is_rendered,
                const std::vector<std::byte>& track_data_blob,
                const std::vector<std::byte>& high_res_waveform_data_blob,
                const std::vector<std::byte>& overview_waveform_data_blob,
                const std::vector<std::byte>& beat_data_blob,
                const std::vector<std::byte>& quick_cues_data_blob,
                const std::vector<std::byte>& loops_data_blob,
                int64_t has_serato_values) {
                if (result)
                {
                    throw track_database_inconsistency{
                        "More than one track with the same id", id};
                }

                result = performance_data_row{
                    id,
                    is_analyzed,
                    is_rendered,
                    track_data::decode(track_data_blob),
                    high_res_waveform_data::decode(high_res_waveform_data_blob),
                    overview_waveform_data::decode(overview_waveform_data_blob),
                    beat_data::decode(beat_data_blob),
                    quick_cues_data::decode(quick_cues_data_blob),
                    loops_data::decode(loops_data_blob),
                    has_serato_values};
            };
    }

    if (!result)
    {
        // It is a legitimate scenario for a track to not have any performance
        // data recorded - it normally means that the track has not been
        // fully analysed.  In such a case, we can return default data here.
        return performance_data_row{id};
    }
    return *result;
}

void engine_storage::set_performance_data(
    int64_t id, int64_t is_analyzed, int64_t is_rendered,
    const track_data& track_data,
    const high_res_waveform_data& high_res_waveform_data,
    const overview_waveform_data& overview_waveform_data,
    const beat_data& beat_data, const quick_cues_data& quick_cues_data,
    const loops_data& loops_data, int64_t has_serato_values,
    int64_t has_rekordbox_values, int64_t has_traktor_values)
{
    // TODO (mr-smidge): check encoding/decoding invariants.

    if (schema >= engine_schema::schema_1_11_1)
    {
        db << "INSERT OR REPLACE INTO PerformanceData ("
              "id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues, hasRekordboxValues, hasTraktorValues) "
              "VALUES (?, ?, ?, "
              "?, ?, "
              "?, ?, ?, ?, "
              "?, ?, ?)"
           << id << is_analyzed << is_rendered << track_data.encode()
           << high_res_waveform_data.encode() << overview_waveform_data.encode()
           << beat_data.encode() << quick_cues_data.encode()
           << loops_data.encode() << has_serato_values << has_rekordbox_values
           << has_traktor_values;
    }
    else if (schema >= engine_schema::schema_1_7_1)
    {
        db << "INSERT OR REPLACE INTO PerformanceData ("
              "id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues, hasRekordboxValues) "
              "VALUES (?, ?, ?, "
              "?, ?, "
              "?, ?, ?, ?, "
              "?, ?)"
           << id << is_analyzed << is_rendered << track_data.encode()
           << high_res_waveform_data.encode() << overview_waveform_data.encode()
           << beat_data.encode() << quick_cues_data.encode()
           << loops_data.encode() << has_serato_values << has_rekordbox_values;
    }
    else
    {
        db << "INSERT OR REPLACE INTO PerformanceData ("
              "id, isAnalyzed, isRendered, "
              "trackData, highResolutionWaveFormData, "
              "overviewWaveFormData, beatData, quickCues, loops, "
              "hasSeratoValues) "
              "VALUES (?, ?, ?, "
              "?, ?, "
              "?, ?, ?, ?, "
              "?)"
           << id << is_analyzed << is_rendered << track_data.encode()
           << high_res_waveform_data.encode() << overview_waveform_data.encode()
           << beat_data.encode() << quick_cues_data.encode()
           << loops_data.encode() << has_serato_values;
    }
}

}  // namespace djinterop::engine::v1
