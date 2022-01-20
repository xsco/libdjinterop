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

#include <sqlite_modern_cpp.h>

#include <djinterop/djinterop.hpp>

#include "djinterop/util.hpp"
#include "schema_1_7_1.hpp"
#include "schema_validate_utils.hpp"

namespace djinterop::engine::schema
{
void schema_1_7_1::verify_information(sqlite::database& db, const std::string& db_name) const
{
    {
        table_info cols{db, db_name, "Information"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "currentPlayedIndiciator", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(
            iter, end, "lastRekordBoxLibraryImportReadCounter", "INTEGER", 0,
            "", 0);
        ++iter;
        validate(iter, end, "schemaVersionMajor", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "schemaVersionMinor", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "schemaVersionPatch", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "uuid", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Information");
    }
    {
        index_list indices{db, db_name, "Information"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Information_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Information");
    }
    {
        index_info ii{db, db_name, "index_Information_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Information_id");
    }
}

void schema_1_7_1::verify_track(sqlite::database& db) const
{
    {
        table_info cols{db, "music", "Track"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "bitrate", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "bpm", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "bpmAnalyzed", "REAL", 0, "", 0);
        ++iter;
        validate(iter, end, "filename", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "idAlbumArt", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "idTrackInExternalDatabase", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "isExternalTrack", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "length", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "lengthCalculated", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "path", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "pdbImportKey", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "playOrder", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackType", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "uuidOfExternalDatabase", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "year", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Track");
    }
    {
        index_list indices{db, "music", "Track"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Track_filename", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_id", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_idAlbumArt", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_idTrackInExternalDatabase", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_isExternalTrack", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_path", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_uuidOfExternalDatabase", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Track");
    }
    {
        index_info ii{db, "music", "index_Track_filename"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "filename");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_filename");
    }
    {
        index_info ii{db, "music", "index_Track_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_id");
    }
    {
        index_info ii{db, "music", "index_Track_idAlbumArt"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "idAlbumArt");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_idAlbumArt");
    }
    {
        index_info ii{db, "music", "index_Track_idTrackInExternalDatabase"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "idTrackInExternalDatabase");
        ++iter;
        validate_no_more(
            iter, end, "index_info", "index_Track_idTrackInExternalDatabase");
    }
    {
        index_info ii{db, "music", "index_Track_isExternalTrack"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "isExternalTrack");
        ++iter;
        validate_no_more(
            iter, end, "index_info", "index_Track_isExternalTrack");
    }
    {
        index_info ii{db, "music", "index_Track_path"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "path");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_path");
    }
    {
        index_info ii{db, "music", "index_Track_uuidOfExternalDatabase"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "uuidOfExternalDatabase");
        ++iter;
        validate_no_more(
            iter, end, "index_info", "index_Track_uuidOfExternalDatabase");
    }
}

void schema_1_7_1::verify_performance_data(sqlite::database& db) const
{
    {
        table_info cols{db, "perfdata", "PerformanceData"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "beatData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "hasRekordboxValues", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "hasSeratoValues", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "highResolutionWaveFormData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "isAnalyzed", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "isRendered", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "loops", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "overviewWaveFormData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "quickCues", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "trackData", "BLOB", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "PerformanceData");
    }
    {
        index_list indices{db, "perfdata", "PerformanceData"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_PerformanceData_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "PerformanceData");
    }
    {
        index_info ii{db, "perfdata", "index_PerformanceData_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PerformanceData_id");
    }
}

void schema_1_7_1::verify_music_schema(sqlite::database& db) const
{
    // Same list of tables as 1.6.0.
    verify_music_master_list(db);

    // Note: most tables in the music DB are unchanged vs. schema 1.6.0, apart
    // from `information` and `track`, which add information about importing
    // from RekordBox.
    verify_information(db, "music");
    verify_album_art(db);
    verify_copied_track(db);
    verify_crate(db);
    verify_crate_hierarchy(db);
    verify_crate_parent_list(db);
    verify_crate_track_list(db);
    verify_historylist(db);
    verify_historylist_track_list(db);
    verify_meta_data(db);
    verify_meta_data_integer(db);
    verify_playlist(db);
    verify_playlist_track_list(db);
    verify_preparelist(db);
    verify_preparelist_track_list(db);
    verify_track(db);
}

void schema_1_7_1::verify_performance_schema(sqlite::database& db) const
{
    // Same list of tables as 1.6.0.
    verify_performance_master_list(db);

    // New columns relating to importing from external libraries.
    verify_information(db, "perfdata");
    verify_performance_data(db);
}

void schema_1_7_1::create_music_schema(sqlite::database& db)
{
    // Note that the table creation order is precisely the same as that produced
    // by a real hardware player.

    // Track
    db << "CREATE TABLE music.Track ( [id] INTEGER, [playOrder] INTEGER , "
          "[length] INTEGER , [lengthCalculated] INTEGER , [bpm] INTEGER , "
          "[year] INTEGER , [path] TEXT , [filename] TEXT , [bitrate] INTEGER "
          ", [bpmAnalyzed] REAL , [trackType] INTEGER , [isExternalTrack] "
          "NUMERIC , [uuidOfExternalDatabase] TEXT , "
          "[idTrackInExternalDatabase] INTEGER , [idAlbumArt] INTEGER  "
          "REFERENCES AlbumArt ( id )  ON DELETE RESTRICT, [pdbImportKey] "
          "INTEGER , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX music.index_Track_id ON Track ( id )";
    db << "CREATE INDEX music.index_Track_path ON Track ( path )";
    db << "CREATE INDEX music.index_Track_filename ON Track ( filename )";
    db << "CREATE INDEX music.index_Track_isExternalTrack ON Track ( "
          "isExternalTrack )";
    db << "CREATE INDEX music.index_Track_uuidOfExternalDatabase ON Track ( "
          "uuidOfExternalDatabase )";
    db << "CREATE INDEX music.index_Track_idTrackInExternalDatabase ON Track ( "
          "idTrackInExternalDatabase )";
    db << "CREATE INDEX music.index_Track_idAlbumArt ON Track ( idAlbumArt )";

    // Information
    db << "DROP TABLE IF EXISTS music.Information";
    db << "CREATE TABLE music.Information ( [id] INTEGER, [uuid] TEXT , "
          "[schemaVersionMajor] INTEGER , [schemaVersionMinor] INTEGER , "
          "[schemaVersionPatch] INTEGER , [currentPlayedIndiciator] INTEGER , "
          "[lastRekordBoxLibraryImportReadCounter] INTEGER , PRIMARY KEY ( "
          "[id] ) )";
    db << "CREATE INDEX music.index_Information_id ON Information ( id )";

    // Metadata
    db << "CREATE TABLE music.MetaData ( [id] INTEGER  REFERENCES Track ( id ) "
          " ON DELETE CASCADE, [type] INTEGER, [text] TEXT , PRIMARY KEY ( "
          "[id], [type] ) )";
    db << "CREATE INDEX music.index_MetaData_id ON MetaData ( id )";
    db << "CREATE INDEX music.index_MetaData_type ON MetaData ( type )";
    db << "CREATE INDEX music.index_MetaData_text ON MetaData ( text )";

    // MetadataInteger
    db << "CREATE TABLE music.MetaDataInteger ( [id] INTEGER  REFERENCES Track "
          "( id )  ON DELETE CASCADE, [type] INTEGER, [value] INTEGER , "
          "PRIMARY KEY ( [id], [type] ) )";
    db << "CREATE INDEX music.index_MetaDataInteger_id ON MetaDataInteger ( id "
          ")";
    db << "CREATE INDEX music.index_MetaDataInteger_type ON MetaDataInteger ( "
          "type )";
    db << "CREATE INDEX music.index_MetaDataInteger_value ON MetaDataInteger ( "
          "value )";

    // Playlist
    db << "CREATE TABLE music.Playlist ( [id] INTEGER, [title] TEXT , PRIMARY "
          "KEY ( [id] ) )";
    db << "CREATE INDEX music.index_Playlist_id ON Playlist ( id )";

    // PlaylistTrackList
    db << "CREATE TABLE music.PlaylistTrackList ( [playlistId] INTEGER  "
          "REFERENCES Playlist ( id )  ON DELETE CASCADE, [trackId] INTEGER  "
          "REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , "
          "[trackNumber] INTEGER )";
    db << "CREATE INDEX music.index_PlaylistTrackList_playlistId ON "
          "PlaylistTrackList ( playlistId )";
    db << "CREATE INDEX music.index_PlaylistTrackList_trackId ON "
          "PlaylistTrackList ( trackId )";

    // Preparelist
    db << "CREATE TABLE music.Preparelist ( [id] INTEGER, [title] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX music.index_Preparelist_id ON Preparelist ( id )";

    // PreparelistTrackList
    db << "CREATE TABLE music.PreparelistTrackList ( [playlistId] INTEGER  "
          "REFERENCES Preparelist ( id )  ON DELETE CASCADE, [trackId] INTEGER "
          " REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , "
          "[trackNumber] INTEGER )";
    db << "CREATE INDEX music.index_PreparelistTrackList_playlistId ON "
          "PreparelistTrackList ( playlistId )";
    db << "CREATE INDEX music.index_PreparelistTrackList_trackId ON "
          "PreparelistTrackList ( trackId )";

    // Historylist
    db << "CREATE TABLE music.Historylist ( [id] INTEGER, [title] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX music.index_Historylist_id ON Historylist ( id )";

    // HistorylistTrackList
    db << "CREATE TABLE music.HistorylistTrackList ( [historylistId] INTEGER  "
          "REFERENCES Historylist ( id )  ON DELETE CASCADE, [trackId] INTEGER "
          " REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , [date] "
          "INTEGER )";
    db << "CREATE INDEX music.index_HistorylistTrackList_historylistId ON "
          "HistorylistTrackList ( historylistId )";
    db << "CREATE INDEX music.index_HistorylistTrackList_trackId ON "
          "HistorylistTrackList ( trackId )";
    db << "CREATE INDEX music.index_HistorylistTrackList_date ON "
          "HistorylistTrackList ( date )";

    // Crate
    db << "CREATE TABLE music.Crate ( [id] INTEGER, [title] TEXT , [path] TEXT "
          ", PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX music.index_Crate_id ON Crate ( id )";
    db << "CREATE INDEX music.index_Crate_title ON Crate ( title )";
    db << "CREATE INDEX music.index_Crate_path ON Crate ( path )";

    // CrateParentList
    db << "CREATE TABLE music.CrateParentList ( [crateOriginId] INTEGER  "
          "REFERENCES Crate ( id )  ON DELETE CASCADE, [crateParentId] INTEGER "
          " REFERENCES Crate ( id )  ON DELETE CASCADE)";
    db << "CREATE INDEX music.index_CrateParentList_crateOriginId ON "
          "CrateParentList ( crateOriginId )";
    db << "CREATE INDEX music.index_CrateParentList_crateParentId ON "
          "CrateParentList ( crateParentId )";

    // CrateTrackList
    db << "CREATE TABLE music.CrateTrackList ( [crateId] INTEGER  REFERENCES "
          "Crate ( id )  ON DELETE CASCADE, [trackId] INTEGER  REFERENCES "
          "Track ( id )  ON DELETE CASCADE)";
    db << "CREATE INDEX music.index_CrateTrackList_crateId ON CrateTrackList ( "
          "crateId )";
    db << "CREATE INDEX music.index_CrateTrackList_trackId ON CrateTrackList ( "
          "trackId )";

    // CrateHierarchy
    db << "CREATE TABLE music.CrateHierarchy ( [crateId] INTEGER  REFERENCES "
          "Crate ( id )  ON DELETE CASCADE, [crateIdChild] INTEGER  REFERENCES "
          "Crate ( id )  ON DELETE CASCADE)";
    db << "CREATE INDEX music.index_CrateHierarchy_crateId ON CrateHierarchy ( "
          "crateId )";
    db << "CREATE INDEX music.index_CrateHierarchy_crateIdChild ON "
          "CrateHierarchy ( crateIdChild )";

    // AlbumArt
    db << "CREATE TABLE music.AlbumArt ( [id] INTEGER, [hash] TEXT , "
          "[albumArt] BLOB , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX music.index_AlbumArt_id ON AlbumArt ( id )";
    db << "CREATE INDEX music.index_AlbumArt_hash ON AlbumArt ( hash )";

    // CopiedTrack
    db << "CREATE TABLE music.CopiedTrack ( [trackId] INTEGER  REFERENCES "
          "Track ( id )  ON DELETE CASCADE, [uuidOfSourceDatabase] TEXT , "
          "[idOfTrackInSourceDatabase] INTEGER , PRIMARY KEY ( [trackId] ) )";
    db << "CREATE INDEX music.index_CopiedTrack_trackId ON CopiedTrack ( "
          "trackId )";

    // Generate UUID for the Information table.
    auto uuid_str = generate_random_uuid();

    // Not yet sure how the "currentPlayedIndiciator" (typo deliberate) value
    // is formed.
    auto current_played_indicator_fake_value = 5100658837829259927l;

    // Insert row into Information
    db << "INSERT INTO music.Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator], [lastRekordBoxLibraryImportReadCounter]) "
          "VALUES (?, ?, ?, ?, ?, ?)"
       << uuid_str << schema_version.maj << schema_version.min
       << schema_version.pat << current_played_indicator_fake_value << 0;

    // Insert default album art entry
    db << "INSERT INTO music.AlbumArt VALUES (1, '', NULL)";

    // Default history list entry
    db << "INSERT INTO music.Historylist VALUES (1, 'History 1')";

    // Default prepare list entry
    db << "INSERT INTO music.Preparelist VALUES (1, 'Prepare')";
}

void schema_1_7_1::create_performance_schema(sqlite::database& db)
{
    // Information
    db << "DROP TABLE IF EXISTS perfdata.Information";
    db << "CREATE TABLE perfdata.Information ( [id] INTEGER, [uuid] TEXT , "
          "[schemaVersionMajor] INTEGER , [schemaVersionMinor] INTEGER , "
          "[schemaVersionPatch] INTEGER , [currentPlayedIndiciator] INTEGER , "
          "[lastRekordBoxLibraryImportReadCounter] INTEGER , PRIMARY KEY ( "
          "[id] ) )";
    db << "CREATE INDEX perfdata.index_Information_id ON Information ( id )";

    // PerformanceData
    db << "DROP TABLE IF EXISTS perfdata.PerformanceData";
    db << "CREATE TABLE perfdata.PerformanceData ( [id] INTEGER, [isAnalyzed] "
          "NUMERIC , [isRendered] NUMERIC , [trackData] BLOB , "
          "[highResolutionWaveFormData] BLOB , [overviewWaveFormData] BLOB , "
          "[beatData] BLOB , [quickCues] BLOB , [loops] BLOB , "
          "[hasSeratoValues] NUMERIC , [hasRekordboxValues] NUMERIC , PRIMARY "
          "KEY ( [id] ) )";
    db << "CREATE INDEX perfdata.index_PerformanceData_id ON PerformanceData ( "
          "id )";

    // Generate UUID for the Information table
    auto uuid_str = generate_random_uuid();

    // Insert row into Information
    db << "INSERT INTO perfdata.Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator], [lastRekordBoxLibraryImportReadCounter]) "
          "VALUES (?, ?, ?, ?, ?, ?)"
       << uuid_str << schema_version.maj << schema_version.min
       << schema_version.pat << 0 << 0;
}

}  // namespace djinterop::engine::schema
