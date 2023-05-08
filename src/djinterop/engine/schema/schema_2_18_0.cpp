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

#include "../../util/random.hpp"
#include "schema_2_18_0.hpp"
#include "schema_validate_utils.hpp"

namespace djinterop::engine::schema
{
void schema_2_18_0::verify_master_list(sqlite::database& db) const
{
    {
        master_list items{db, "table"};
        auto iter = items.begin(), end = items.end();
        validate(iter, end, "table", "AlbumArt", "AlbumArt");
        ++iter;
        validate(iter, end, "table", "ChangeLog", "ChangeLog");
        ++iter;
        validate(iter, end, "table", "Information", "Information");
        ++iter;
        validate(iter, end, "table", "Pack", "Pack");
        ++iter;
        validate(iter, end, "table", "Playlist", "Playlist");
        ++iter;
        validate(iter, end, "table", "PlaylistEntity", "PlaylistEntity");
        ++iter;
        validate(iter, end, "table", "PreparelistEntity", "PreparelistEntity");
        ++iter;
        validate(iter, end, "table", "Track", "Track");
        ++iter;
        validate(iter, end, "table", "sqlite_sequence", "sqlite_sequence");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        master_list items{db, "view"};
        auto iter = items.begin(), end = items.end();
        validate(iter, end, "view", "PerformanceData", "PerformanceData");
        ++iter;
        validate(
            iter, end, "view", "PlaylistAllChildren", "PlaylistAllChildren");
        ++iter;
        validate(iter, end, "view", "PlaylistAllParent", "PlaylistAllParent");
        ++iter;
        validate(iter, end, "view", "PlaylistPath", "PlaylistPath");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_album_art(sqlite::database& db) const
{
    {
        table_info cols{db, "AlbumArt"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "albumArt", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "hash", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "AlbumArt"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_AlbumArt_hash", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "index_AlbumArt_hash"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "hash");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_change_log(sqlite::database& db) const
{
    {
        table_info cols{db, "ChangeLog"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "ChangeLog"};
        auto iter = indices.begin(), end = indices.end();
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_information(sqlite::database& db) const
{
    {
        table_info cols{db, "Information"};
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
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "Information"};
        auto iter = indices.begin(), end = indices.end();
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_pack(sqlite::database& db) const
{
    {
        table_info cols{db, "Pack"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "changeLogDatabaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "changeLogId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "packId", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "Pack"};
        auto iter = indices.begin(), end = indices.end();
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_playlist(sqlite::database& db) const
{
    {
        table_info cols{db, "Playlist"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "isExplicitlyExported", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "isPersisted", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "lastEditTime", "DATETIME", 0, "", 0);
        ++iter;
        validate(iter, end, "nextListId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "parentListId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "Playlist"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "sqlite_autoindex_Playlist_1", 1, "u", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_Playlist_2", 1, "u", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "sqlite_autoindex_Playlist_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "title");
        ++iter;
        validate(iter, end, 1, "parentListId");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "sqlite_autoindex_Playlist_2"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "parentListId");
        ++iter;
        validate(iter, end, 1, "nextListId");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_playlist_entity(sqlite::database& db) const
{
    {
        table_info cols{db, "PlaylistEntity"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "databaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "listId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "membershipReference", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "nextEntityId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "PlaylistEntity"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "sqlite_autoindex_PlaylistEntity_1", 1, "u", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "sqlite_autoindex_PlaylistEntity_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "listId");
        ++iter;
        validate(iter, end, 1, "databaseUuid");
        ++iter;
        validate(iter, end, 2, "trackId");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_preparelist_entity(sqlite::database& db) const
{
    {
        table_info cols{db, "PreparelistEntity"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackNumber", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "PreparelistEntity"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_PreparelistEntity_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "index_PreparelistEntity_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify_track(sqlite::database& db) const
{
    {
        table_info cols{db, "Track"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "album", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "albumArt", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "albumArtId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "artist", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "beatData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "bitrate", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "bpm", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "bpmAnalyzed", "REAL", 0, "", 0);
        ++iter;
        validate(iter, end, "comment", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "composer", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "dateAdded", "DATETIME", 0, "", 0);
        ++iter;
        validate(iter, end, "dateCreated", "DATETIME", 0, "", 0);
        ++iter;
        validate(iter, end, "explicitLyrics", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "fileBytes", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "fileType", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "filename", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "genre", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "isAnalyzed", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "isAvailable", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "isBeatGridLocked", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "isMetadataImported", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(
            iter, end, "isMetadataOfPackedTrackChanged", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(
            iter, end, "isPerfomanceDataOfPackedTrackChanged", "BOOLEAN", 0, "",
            0);
        ++iter;
        validate(iter, end, "isPlayed", "BOOLEAN", 0, "", 0);
        ++iter;
        validate(iter, end, "key", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "label", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "length", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "loops", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "originDatabaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "originTrackId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "overviewWaveFormData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "path", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "pdbImportKey", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "playOrder", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "playedIndicator", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "quickCues", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "rating", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "remixer", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "streamingFlags", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "streamingSource", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "thirdPartySourceId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "timeLastPlayed", "DATETIME", 0, "", 0);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "trackData", "BLOB", 0, "", 0);
        ++iter;
        validate(iter, end, "uri", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "year", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "Track"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Track_albumArtId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_filename", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Track_uri", 0, "c", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_Track_1", 1, "u", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_Track_2", 1, "u", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "index_Track_albumArtId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "albumArtId");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "index_Track_filename"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "filename");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "index_Track_uri"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "uri");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "sqlite_autoindex_Track_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "originDatabaseUuid");
        ++iter;
        validate(iter, end, 1, "originTrackId");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "sqlite_autoindex_Track_2"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "path");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_2_18_0::verify(sqlite::database& db) const
{
    verify_master_list(db);

    verify_information(db);
    verify_album_art(db);
    verify_change_log(db);
    verify_pack(db);
    verify_playlist(db);
    verify_playlist_entity(db);
    verify_preparelist_entity(db);
    verify_track(db);
}

void schema_2_18_0::create(sqlite::database& db)
{
    db << "CREATE TABLE Information ( id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "uuid TEXT, schemaVersionMajor INTEGER, schemaVersionMinor INTEGER, "
          "schemaVersionPatch INTEGER, currentPlayedIndiciator INTEGER, "
          "lastRekordBoxLibraryImportReadCounter INTEGER);";
    db << "CREATE TABLE Track ( id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "playOrder INTEGER, length INTEGER, bpm INTEGER, year INTEGER, path "
          "TEXT, filename TEXT, bitrate INTEGER, bpmAnalyzed REAL, albumArtId "
          "INTEGER, fileBytes INTEGER, title TEXT, artist TEXT, album TEXT, "
          "genre TEXT, comment TEXT, label TEXT, composer TEXT, remixer TEXT, "
          "key INTEGER, rating INTEGER, albumArt TEXT, timeLastPlayed "
          "DATETIME, isPlayed BOOLEAN, fileType TEXT, isAnalyzed BOOLEAN, "
          "dateCreated DATETIME, dateAdded DATETIME, isAvailable BOOLEAN, "
          "isMetadataOfPackedTrackChanged BOOLEAN, "
          "isPerfomanceDataOfPackedTrackChanged BOOLEAN, playedIndicator "
          "INTEGER, isMetadataImported BOOLEAN, pdbImportKey INTEGER, "
          "streamingSource TEXT, uri TEXT, isBeatGridLocked BOOLEAN, "
          "originDatabaseUuid TEXT, originTrackId INTEGER, trackData BLOB, "
          "overviewWaveFormData BLOB, beatData BLOB, quickCues BLOB, loops "
          "BLOB, thirdPartySourceId INTEGER, streamingFlags INTEGER, "
          "explicitLyrics BOOLEAN, CONSTRAINT "
          "C_originDatabaseUuid_originTrackId UNIQUE (originDatabaseUuid, "
          "originTrackId), CONSTRAINT C_path UNIQUE (path), FOREIGN KEY "
          "(albumArtId) REFERENCES AlbumArt (id) ON DELETE RESTRICT );";
    db << "CREATE TABLE ChangeLog ( id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "trackId INTEGER, FOREIGN KEY (trackId) REFERENCES Track (id) ON "
          "DELETE SET NULL );";
    db << "CREATE TABLE AlbumArt ( id INTEGER PRIMARY KEY AUTOINCREMENT, hash "
          "TEXT, albumArt BLOB );";
    db << "CREATE TABLE Pack ( id INTEGER PRIMARY KEY AUTOINCREMENT, packId "
          "TEXT, changeLogDatabaseUuid TEXT, changeLogId INTEGER );";
    db << "CREATE TABLE PlaylistEntity ( id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "listId INTEGER, trackId INTEGER, databaseUuid TEXT, nextEntityId "
          "INTEGER, membershipReference INTEGER, CONSTRAINT "
          "C_NAME_UNIQUE_FOR_LIST UNIQUE (listId, databaseUuid, trackId), "
          "FOREIGN KEY (listId) REFERENCES Playlist (id) ON DELETE CASCADE );";
    db << "CREATE TABLE Playlist ( id INTEGER PRIMARY KEY AUTOINCREMENT, title "
          "TEXT, parentListId INTEGER, isPersisted BOOLEAN, nextListId "
          "INTEGER, lastEditTime DATETIME, isExplicitlyExported BOOLEAN, "
          "CONSTRAINT C_NAME_UNIQUE_FOR_PARENT UNIQUE (title, parentListId), "
          "CONSTRAINT C_NEXT_LIST_ID_UNIQUE_FOR_PARENT UNIQUE (parentListId, "
          "nextListId) );";
    db << "CREATE TABLE PreparelistEntity ( id INTEGER PRIMARY KEY "
          "AUTOINCREMENT, trackId INTEGER, trackNumber INTEGER, FOREIGN KEY "
          "(trackId) REFERENCES Track (id) ON DELETE CASCADE );";
    db << "CREATE INDEX index_Track_filename ON Track (filename);";
    db << "CREATE INDEX index_Track_albumArtId ON Track (albumArtId);";
    db << "CREATE INDEX index_Track_uri ON Track (uri);";
    db << "CREATE TRIGGER trigger_after_insert_Track_check_id AFTER INSERT ON "
          "Track WHEN NEW.id <= (SELECT seq FROM sqlite_sequence WHERE name = "
          "'Track') BEGIN SELECT RAISE(ABORT, 'Recycling deleted track id''s "
          "are not allowed'); END;";
    db << "CREATE TRIGGER trigger_after_update_Track_check_Id BEFORE UPDATE ON "
          "Track WHEN NEW.id <> OLD.id BEGIN SELECT RAISE(ABORT, 'Changing "
          "track id''s are not allowed'); END;";
    db << "CREATE TRIGGER trigger_after_insert_Track_fix_origin AFTER INSERT "
          "ON Track WHEN IFNULL(NEW.originTrackId, 0) = 0 OR "
          "IFNULL(NEW.originDatabaseUuid, '') = '' BEGIN UPDATE Track SET "
          "originTrackId = NEW.id, originDatabaseUuid = (SELECT uuid FROM "
          "Information) WHERE track.id = NEW.id; END;";
    db << "CREATE TRIGGER trigger_after_update_Track_fix_origin AFTER UPDATE "
          "ON Track WHEN IFNULL(NEW.originTrackId, 0) = 0 OR "
          "IFNULL(NEW.originDatabaseUuid, '') = '' BEGIN UPDATE Track SET "
          "originTrackId = NEW.id, originDatabaseUuid = (SELECT uuid FROM "
          "Information) WHERE track.id = NEW.id; END;";
    db << "CREATE TRIGGER trigger_after_update_Track AFTER UPDATE ON Track FOR "
          "EACH ROW BEGIN INSERT INTO ChangeLog (trackId) VALUES(NEW.id); END;";
    db << "CREATE INDEX index_AlbumArt_hash ON AlbumArt (hash);";
    db << "CREATE TRIGGER trigger_before_insert_List BEFORE INSERT ON Playlist "
          "FOR EACH ROW BEGIN UPDATE Playlist SET nextListId = -(1 + "
          "nextListId) WHERE nextListId = NEW.nextListId AND parentListId = "
          "NEW.parentListId; END;";
    db << "CREATE TRIGGER trigger_after_insert_List AFTER INSERT ON Playlist "
          "FOR EACH ROW BEGIN UPDATE Playlist SET nextListId = NEW.id WHERE "
          "nextListId = -(1 + NEW.nextListId) AND parentListId = "
          "NEW.parentListId; END;";
    db << "CREATE TRIGGER trigger_after_delete_List AFTER DELETE ON Playlist "
          "FOR EACH ROW BEGIN UPDATE Playlist SET nextListId = OLD.nextListId "
          "WHERE nextListId = OLD.id; DELETE FROM Playlist WHERE parentListId "
          "= OLD.id; END;";
    db << "CREATE TRIGGER trigger_after_update_isPersistParent AFTER UPDATE ON "
          "Playlist WHEN (old.isPersisted = 0 AND new.isPersisted = 1) OR "
          "(old.parentListId != new.parentListId AND new.isPersisted = 1) "
          "BEGIN UPDATE Playlist SET isPersisted = 1 WHERE id IN (SELECT "
          "parentListId FROM PlaylistAllParent WHERE id=new.id); END;";
    db << "CREATE TRIGGER trigger_after_update_isPersistChild AFTER UPDATE ON "
          "Playlist WHEN old.isPersisted = 1 AND new.isPersisted = 0 BEGIN "
          "UPDATE Playlist SET isPersisted = 0 WHERE id IN (SELECT childListId "
          "FROM PlaylistAllChildren WHERE id=new.id); END;";
    db << "CREATE TRIGGER trigger_after_insert_isPersist AFTER INSERT ON "
          "Playlist WHEN new.isPersisted = 1 BEGIN UPDATE Playlist SET "
          "isPersisted = 1 WHERE id IN (SELECT parentListId FROM "
          "PlaylistAllParent WHERE id=new.id); END;";
    db << "CREATE VIEW PlaylistPath AS WITH RECURSIVE Heirarchy AS ( SELECT id "
          "AS child, parentListId AS parent, title AS name, 1 AS depth FROM "
          "Playlist UNION ALL SELECT child, parentListId AS parent, title AS "
          "name, h.depth + 1 AS depth FROM Playlist c JOIN Heirarchy h ON "
          "h.parent = c.id ORDER BY depth DESC ), OrderedList AS ( SELECT id , "
          "nextListId, 1 AS position FROM Playlist WHERE nextListId = 0 UNION "
          "ALL SELECT c.id , c.nextListId , l.position + 1 FROM Playlist c "
          "INNER JOIN OrderedList l ON c.nextListId = l.id ), NameConcat AS ( "
          "SELECT child AS id, GROUP_CONCAT(name ,';') || ';' AS path FROM "
          "(SELECT child, name FROM Heirarchy ORDER BY depth DESC ) GROUP BY "
          "child ) SELECT id, path, ROW_NUMBER() OVER (ORDER BY (SELECT "
          "COUNT(*) FROM (SELECT * FROM Heirarchy WHERE child = id) ) DESC, "
          "(SELECT position FROM OrderedList ol WHERE ol.id = c.id) ASC ) AS "
          "position FROM Playlist c LEFT JOIN NameConcat g USING (id);";
    db << "CREATE VIEW PlaylistAllParent AS WITH FindAllParent AS ( SELECT id, "
          "parentListId FROM Playlist UNION ALL SELECT recursiveCTE.id, "
          "Plist.parentListId FROM Playlist Plist INNER JOIN FindAllParent "
          "recursiveCTE ON recursiveCTE.parentListId = Plist.id ) SELECT * "
          "FROM FindAllParent;";
    db << "CREATE VIEW PlaylistAllChildren AS WITH FindAllChild AS ( SELECT "
          "id, id as childListId FROM Playlist UNION ALL SELECT "
          "recursiveCTE.id, Plist.id FROM Playlist Plist INNER JOIN "
          "FindAllChild recursiveCTE ON recursiveCTE.childListId = "
          "Plist.parentListId ) SELECT * FROM FindAllChild WHERE id <> "
          "childListId;";
    db << "CREATE TRIGGER trigger_before_delete_PlaylistEntity BEFORE DELETE "
          "ON PlaylistEntity WHEN OLD.trackId > 0 BEGIN UPDATE PlaylistEntity "
          "SET nextEntityId = OLD.nextEntityId WHERE nextEntityId = OLD.id AND "
          "listId = OLD.listId; END;";
    db << "CREATE INDEX index_PreparelistEntity_trackId ON PreparelistEntity "
          "(trackId);";
    db << "CREATE VIEW PerformanceData AS SELECT id AS trackId, isAnalyzed, "
          "trackData, overviewWaveFormData, beatData, quickCues, loops, "
          "thirdPartySourceId FROM Track;";
    db << "CREATE TRIGGER trigger_instead_insert_PerformanceData INSTEAD OF "
          "INSERT ON PerformanceData FOR EACH ROW BEGIN UPDATE Track SET "
          "isAnalyzed = NEW.isAnalyzed, trackData = NEW.trackData, "
          "overviewWaveFormData = NEW.overviewWaveFormData, beatData = "
          "NEW.beatData, quickCues = NEW.quickCues, loops = NEW.loops, "
          "thirdPartySourceId = NEW.thirdPartySourceId WHERE Track.id = "
          "NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_update_isAnalyzed_PerformanceData "
          "INSTEAD OF UPDATE OF isAnalyzed ON PerformanceData FOR EACH ROW "
          "BEGIN UPDATE Track SET isAnalyzed = NEW.isAnalyzed WHERE Track.id = "
          "NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_update_trackData_PerformanceData "
          "INSTEAD OF UPDATE OF trackData ON PerformanceData FOR EACH ROW "
          "BEGIN UPDATE Track SET trackData = NEW.trackData WHERE Track.id = "
          "NEW.trackId; END;";
    db << "CREATE TRIGGER "
          "trigger_instead_update_overviewWaveFormData_PerformanceData INSTEAD "
          "OF UPDATE OF overviewWaveFormData ON PerformanceData FOR EACH ROW "
          "BEGIN UPDATE Track SET overviewWaveFormData = "
          "NEW.overviewWaveFormData WHERE Track.id = NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_update_beatData_PerformanceData "
          "INSTEAD OF UPDATE OF beatData ON PerformanceData FOR EACH ROW BEGIN "
          "UPDATE Track SET beatData = NEW.beatData WHERE Track.id = "
          "NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_update_quickCues_PerformanceData "
          "INSTEAD OF UPDATE OF quickCues ON PerformanceData FOR EACH ROW "
          "BEGIN UPDATE Track SET quickCues = NEW.quickCues WHERE Track.id = "
          "NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_update_loops_PerformanceData INSTEAD "
          "OF UPDATE OF loops ON PerformanceData FOR EACH ROW BEGIN UPDATE "
          "Track SET loops = NEW.loops WHERE Track.id = NEW.trackId; END;";
    db << "CREATE TRIGGER "
          "trigger_instead_update_thirdPartySourceId_PerformanceData INSTEAD "
          "OF UPDATE OF thirdPartySourceId ON PerformanceData FOR EACH ROW "
          "BEGIN UPDATE Track SET thirdPartySourceId = NEW.thirdPartySourceId "
          "WHERE Track.id = NEW.trackId; END;";
    db << "CREATE TRIGGER trigger_instead_delete_PerformanceData INSTEAD OF "
          "DELETE ON PerformanceData FOR EACH ROW BEGIN UPDATE Track SET "
          "isAnalyzed = NULL, trackData = NULL, overviewWaveFormData = NULL, "
          "beatData = NULL, quickCues = NULL, loops = NULL, thirdPartySourceId "
          "= NULL WHERE Track.id = OLD.trackId; END;";

    // Generate UUID for the Information table.
    auto uuid_str = djinterop::util::generate_random_uuid();

    // Not yet sure how the "currentPlayedIndiciator" (typo deliberate) value
    // is formed.
    auto current_played_indicator_fake_value = 5100658837829259927l;

    // Insert row into Information
    db << "INSERT INTO Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator], [lastRekordBoxLibraryImportReadCounter]) "
          "VALUES (?, ?, ?, ?, ?, ?)"
       << uuid_str << schema_version.maj << schema_version.min
       << schema_version.pat << current_played_indicator_fake_value << 0;

    // Insert default album art entry
    db << "INSERT INTO AlbumArt VALUES (1, '', NULL)";
}

}  // namespace djinterop::engine::schema
