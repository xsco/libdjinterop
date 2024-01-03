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

#include "../../util/random.hpp"
#include "schema_1_11_1.hpp"
#include "schema_1_17_0.hpp"
#include "schema_validate_utils.hpp"

namespace djinterop::engine::schema
{
void schema_1_17_0::verify_music_master_list(sqlite::database& db) const
{
    {
        master_list items{db, "music", "table"};
        auto iter = items.begin(), end = items.end();
        validate(iter, end, "music", "table", "AlbumArt", "AlbumArt");
        ++iter;
        validate(iter, end, "music", "table", "ChangeLog", "ChangeLog");
        ++iter;
        validate(iter, end, "music", "table", "CopiedTrack", "CopiedTrack");
        ++iter;
        validate(iter, end, "music", "table", "Information", "Information");
        ++iter;
        validate(iter, end, "music", "table", "List", "List");
        ++iter;
        validate(iter, end, "music", "table", "ListHierarchy", "ListHierarchy");
        ++iter;
        validate(
            iter, end, "music", "table", "ListParentList", "ListParentList");
        ++iter;
        validate(iter, end, "music", "table", "ListTrackList", "ListTrackList");
        ++iter;
        validate(iter, end, "music", "table", "MetaData", "MetaData");
        ++iter;
        validate(
            iter, end, "music", "table", "MetaDataInteger", "MetaDataInteger");
        ++iter;
        validate(iter, end, "music", "table", "Pack", "Pack");
        ++iter;
        validate(iter, end, "music", "table", "Track", "Track");
        ++iter;
        validate(
            iter, end, "music", "table", "sqlite_sequence", "sqlite_sequence");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        master_list items{db, "music", "view"};
        auto iter = items.begin(), end = items.end();
        validate(iter, end, "music", "view", "Crate", "Crate");
        ++iter;
        validate(
            iter, end, "music", "view", "CrateHierarchy", "CrateHierarchy");
        ++iter;
        validate(
            iter, end, "music", "view", "CrateParentList", "CrateParentList");
        ++iter;
        validate(
            iter, end, "music", "view", "CrateTrackList", "CrateTrackList");
        ++iter;
        validate(iter, end, "music", "view", "Historylist", "Historylist");
        ++iter;
        validate(
            iter, end, "music", "view", "HistorylistTrackList",
            "HistorylistTrackList");
        ++iter;
        validate(iter, end, "music", "view", "Playlist", "Playlist");
        ++iter;
        validate(
            iter, end, "music", "view", "PlaylistTrackList",
            "PlaylistTrackList");
        ++iter;
        validate(iter, end, "music", "view", "Preparelist", "Preparelist");
        ++iter;
        validate(
            iter, end, "music", "view", "PreparelistTrackList",
            "PreparelistTrackList");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_1_17_0::verify_performance_master_list(sqlite::database& db) const
{
    {
        master_list items{db, "perfdata", "table"};
        auto iter = items.begin(), end = items.end();
        validate(iter, end, "perfdata", "table", "ChangeLog", "ChangeLog");
        ++iter;
        validate(iter, end, "perfdata", "table", "Information", "Information");
        ++iter;
        validate(
            iter, end, "perfdata", "table", "PerformanceData",
            "PerformanceData");
        ++iter;
        validate(
            iter, end, "music", "table", "sqlite_sequence", "sqlite_sequence");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        master_list items{db, "perfdata", "view"};
        auto iter = items.begin(), end = items.end();
        validate_no_more(iter, end);
    }
}

void schema_1_17_0::verify_change_log(
    sqlite::database& db, const std::string& db_name) const
{
    {
        table_info cols{db, db_name, "ChangeLog"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "itemId", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, db_name, "ChangeLog"};
        auto iter = indices.begin(), end = indices.end();
        validate_no_more(iter, end);
    }
}

void schema_1_17_0::verify_list(sqlite::database& db) const
{
    {
        table_info cols{db, "music", "List"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "isExplicitlyExported", "NUMERIC", 0, "1", 0);
        ++iter;
        validate(iter, end, "isFolder", "NUMERIC", 0, "", 0);
        ++iter;
        validate(iter, end, "ordering", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "path", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "trackCount", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "type", "INTEGER", 0, "", 2);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_list indices{db, "music", "List"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_List_id", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_List_ordering", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_List_path", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_List_type", 0, "c", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_List_1", 1, "pk", 0);
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "music", "index_List_ordering"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "ordering");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "music", "index_List_path"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "path");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "music", "index_List_type"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "type");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "music", "index_List_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end);
    }
    {
        index_info ii{db, "music", "sqlite_autoindex_List_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate(iter, end, 1, "type");
        ++iter;
        validate_no_more(iter, end);
    }
}

void schema_1_17_0::verify_pack(sqlite::database& db) const
{
    {
        table_info cols{db, "music", "Pack"};
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
        index_list indices{db, "music", "Pack"};
        auto iter = indices.begin(), end = indices.end();
        validate_no_more(iter, end);
    }
}

void schema_1_17_0::verify_music_schema(sqlite::database& db) const
{
    // New tables `ChangeLog` and `Pack` have been introduced.
    verify_music_master_list(db);

    // This schema adds a different default value to a recently-added column on
    // the `List` table, as well as the new `ChangeLog` and `Pack` tables.
    verify_information(db, "music");
    verify_album_art(db);
    verify_change_log(db, "music");
    verify_copied_track(db);
    verify_crate(db);
    verify_crate_hierarchy(db);
    verify_crate_parent_list(db);
    verify_crate_track_list(db);
    verify_historylist(db);
    verify_historylist_track_list(db);
    verify_list(db);
    verify_list_hierarchy(db);
    verify_list_parent_list(db);
    verify_list_track_list(db);
    verify_meta_data(db);
    verify_meta_data_integer(db);
    verify_pack(db);
    verify_playlist(db);
    verify_playlist_track_list(db);
    verify_preparelist(db);
    verify_preparelist_track_list(db);
    verify_track(db);
}

void schema_1_17_0::verify_performance_schema(sqlite::database& db) const
{
    // New `ChangeLog` table.
    verify_performance_master_list(db);

    // The only major change is the new `ChangeLog` table.
    verify_information(db, "perfdata");
    verify_change_log(db, "perfdata");
    schema_1_11_1::verify_performance_data(db);
}

void schema_1_17_0::create_music_schema(sqlite::database& db)
{
    db << "CREATE TABLE music.Track ( [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
          "[playOrder] INTEGER, [length] INTEGER, [lengthCalculated] INTEGER, "
          "[bpm] INTEGER, [year] INTEGER, [path] TEXT, [filename] TEXT, "
          "[bitrate] INTEGER, [bpmAnalyzed] REAL, [trackType] INTEGER, "
          "[isExternalTrack] NUMERIC, [uuidOfExternalDatabase] TEXT, "
          "[idTrackInExternalDatabase] INTEGER, [idAlbumArt] INTEGER, "
          "[fileBytes] INTEGER, [pdbImportKey] INTEGER, [uri] TEXT, CONSTRAINT "
          "C_path UNIQUE ([path]), FOREIGN KEY ( [idAlbumArt] ) REFERENCES "
          "AlbumArt ( [id] )  ON DELETE RESTRICT);";
    db << "CREATE TABLE music.Information ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [uuid] TEXT, [schemaVersionMajor] INTEGER, "
          "[schemaVersionMinor] INTEGER, [schemaVersionPatch] INTEGER, "
          "[currentPlayedIndiciator] INTEGER, "
          "[lastRekordBoxLibraryImportReadCounter] INTEGER);";
    db << "CREATE TABLE music.MetaData ( [id] INTEGER, [type] INTEGER, [text] "
          "TEXT, PRIMARY KEY ( [id], [type] ) , FOREIGN KEY ( [id] ) "
          "REFERENCES Track ( [id] )  ON DELETE CASCADE);";
    db << "CREATE TABLE music.MetaDataInteger ( [id] INTEGER, [type] INTEGER, "
          "[value] INTEGER, PRIMARY KEY ( [id], [type] ) , FOREIGN KEY ( [id] "
          ") REFERENCES Track ( [id] )  ON DELETE CASCADE);";
    db << "CREATE TABLE music.AlbumArt ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [hash] TEXT, [albumArt] BLOB);";
    db << "CREATE TABLE music.CopiedTrack ( [trackId] INTEGER, "
          "[uuidOfSourceDatabase] TEXT, [idOfTrackInSourceDatabase] INTEGER, "
          "PRIMARY KEY ( [trackId] ) , FOREIGN KEY ( [trackId] ) REFERENCES "
          "Track ( [id] )  ON DELETE CASCADE);";
    db << "CREATE TABLE music.List ( [id] INTEGER, [type] INTEGER, [title] "
          "TEXT, [path] TEXT, [isFolder] NUMERIC, [trackCount] INTEGER, "
          "[ordering] INTEGER, [isExplicitlyExported] NUMERIC DEFAULT 1, "
          "PRIMARY KEY ( [id], [type] ) );";
    db << "CREATE TABLE music.ListTrackList ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [listId] INTEGER, [listType] INTEGER, [trackId] "
          "INTEGER, [trackIdInOriginDatabase] INTEGER, [databaseUuid] TEXT, "
          "[trackNumber] INTEGER, FOREIGN KEY ( [listId], [listType] ) "
          "REFERENCES List ( [id], [type] )  ON DELETE CASCADE, FOREIGN KEY ( "
          "[trackId] ) REFERENCES Track ( [id] )  ON DELETE CASCADE);";
    db << "CREATE TABLE music.ListHierarchy ( [listId] INTEGER, [listType] "
          "INTEGER, [listIdChild] INTEGER, [listTypeChild] INTEGER, FOREIGN "
          "KEY ( [listId], [listType] ) REFERENCES List ( [id], [type] )  ON "
          "DELETE CASCADE, FOREIGN KEY ( [listIdChild], [listTypeChild] ) "
          "REFERENCES List ( [id], [type] )  ON DELETE CASCADE);";
    db << "CREATE TABLE music.ListParentList ( [listOriginId] INTEGER, "
          "[listOriginType] INTEGER, [listParentId] INTEGER, [listParentType] "
          "INTEGER, FOREIGN KEY ( [listOriginId], [listOriginType] ) "
          "REFERENCES List ( [id], [type] )  ON DELETE CASCADE, FOREIGN KEY ( "
          "[listParentId], [listParentType] ) REFERENCES List ( [id], [type] ) "
          " ON DELETE CASCADE);";
    db << "CREATE TABLE music.Pack ( [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
          "[packId] TEXT, [changeLogDatabaseUuid] TEXT, [changeLogId] "
          "INTEGER);";
    db << "CREATE TABLE music.ChangeLog ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [itemId] INTEGER);";
    db << "CREATE INDEX music.index_Track_id ON Track ( id );";
    db << "CREATE INDEX music.index_Track_path ON Track ( path );";
    db << "CREATE INDEX music.index_Track_filename ON Track ( filename );";
    db << "CREATE INDEX music.index_Track_isExternalTrack ON Track ( "
          "isExternalTrack );";
    db << "CREATE INDEX music.index_Track_uuidOfExternalDatabase ON Track ( "
          "uuidOfExternalDatabase );";
    db << "CREATE INDEX music.index_Track_idTrackInExternalDatabase ON Track ( "
          "idTrackInExternalDatabase );";
    db << "CREATE INDEX music.index_Track_idAlbumArt ON Track ( idAlbumArt );";
    db << "CREATE INDEX music.index_Track_uri ON Track ( uri );";
    db << "CREATE TRIGGER music.trigger_after_insert_Track AFTER INSERT ON "
          "Track WHEN NEW.id <= (SELECT seq FROM sqlite_sequence WHERE name = "
          "'Track') BEGIN SELECT RAISE(ABORT, 'Recycling deleted track id''s "
          "are not allowed'); END;";
    db << "CREATE TRIGGER music.trigger_before_update_Track BEFORE UPDATE ON "
          "Track WHEN NEW.id <> OLD.id BEGIN SELECT RAISE(ABORT, 'Changing "
          "track id''s are not allowed'); END;";
    db << "CREATE TRIGGER music.trigger_after_delete_Track AFTER DELETE ON "
          "Track WHEN OLD.id > COALESCE((SELECT MAX(id) FROM Track), 0) BEGIN "
          "DELETE FROM Track WHERE path IS NULL; INSERT INTO Track(id) "
          "VALUES(NULL); END;";
    db << "CREATE INDEX music.index_Information_id ON Information ( id );";
    db << "CREATE INDEX music.index_MetaData_id ON MetaData ( id );";
    db << "CREATE INDEX music.index_MetaData_type ON MetaData ( type );";
    db << "CREATE INDEX music.index_MetaData_text ON MetaData ( text );";
    db << "CREATE INDEX music.index_MetaDataInteger_id ON MetaDataInteger ( id "
          ");";
    db << "CREATE INDEX music.index_MetaDataInteger_type ON MetaDataInteger ( "
          "type );";
    db << "CREATE INDEX music.index_MetaDataInteger_value ON MetaDataInteger ( "
          "value );";
    db << "CREATE INDEX music.index_AlbumArt_id ON AlbumArt ( id );";
    db << "CREATE INDEX music.index_AlbumArt_hash ON AlbumArt ( hash );";
    db << "CREATE INDEX music.index_CopiedTrack_trackId ON CopiedTrack ( "
          "trackId );";
    db << "CREATE INDEX music.index_List_id ON List ( id );";
    db << "CREATE INDEX music.index_List_type ON List ( type );";
    db << "CREATE INDEX music.index_List_path ON List ( path );";
    db << "CREATE INDEX music.index_List_ordering ON List ( ordering );";
    db << "CREATE VIEW music.Playlist AS SELECT id, title FROM List WHERE type "
          "= 1;";
    db << "CREATE VIEW music.Historylist AS SELECT id, title FROM List WHERE "
          "type = 2;";
    db << "CREATE VIEW music.Preparelist AS SELECT id, title FROM List WHERE "
          "type = 3;";
    db << "CREATE VIEW music.Crate AS SELECT id AS id, title AS title, path AS "
          "path FROM List WHERE type = 4;";
    db << "CREATE INDEX music.index_ListTrackList_listId ON ListTrackList ( "
          "listId );";
    db << "CREATE INDEX music.index_ListTrackList_listType ON ListTrackList ( "
          "listType );";
    db << "CREATE INDEX music.index_ListTrackList_trackId ON ListTrackList ( "
          "trackId );";
    db << "CREATE VIEW music.PlaylistTrackList AS SELECT listId AS playlistId, "
          "trackId, trackIdInOriginDatabase, databaseUuid, trackNumber FROM "
          "ListTrackList AS ltl INNER JOIN List AS l ON l.id = ltl.listId AND "
          "l.type = ltl.listType WHERE ltl.listType = 1;";
    db << "CREATE VIEW music.HistorylistTrackList AS SELECT listId AS "
          "historylistId, trackId, trackIdInOriginDatabase, databaseUuid, 0 AS "
          "date FROM ListTrackList AS ltl INNER JOIN List AS l ON l.id = "
          "ltl.listId AND l.type = ltl.listType WHERE ltl.listType = 2;";
    db << "CREATE VIEW music.PreparelistTrackList AS SELECT listId AS "
          "playlistId, trackId, trackIdInOriginDatabase, databaseUuid, "
          "trackNumber FROM ListTrackList AS ltl INNER JOIN List AS l ON l.id "
          "= ltl.listId AND l.type = ltl.listType WHERE ltl.listType = 3;";
    db << "CREATE VIEW music.CrateTrackList AS SELECT listId AS crateId, "
          "trackId AS trackId FROM ListTrackList AS ltl INNER JOIN List AS l "
          "ON l.id = ltl.listId AND l.type = ltl.listType WHERE ltl.listType = "
          "4;";
    db << "CREATE TRIGGER music.trigger_delete_Playlist INSTEAD OF DELETE ON "
          "Playlist FOR EACH ROW BEGIN   DELETE FROM List WHERE type = 1 AND "
          "OLD.id = id AND OLD.title = title; END;";
    db << "CREATE TRIGGER music.trigger_update_Playlist INSTEAD OF UPDATE ON "
          "Playlist FOR EACH ROW BEGIN   UPDATE List SET id = NEW.id, title = "
          "NEW.title   WHERE  id = OLD.id AND title = OLD.title   ;  END;";
    db << "CREATE TRIGGER music.trigger_delete_Historylist INSTEAD OF DELETE "
          "ON Historylist FOR EACH ROW BEGIN   DELETE FROM List WHERE type = 2 "
          "AND OLD.id = id AND OLD.title = title; END;";
    db << "CREATE TRIGGER music.trigger_update_Historylist INSTEAD OF UPDATE "
          "ON Historylist FOR EACH ROW BEGIN   UPDATE List SET id = NEW.id, "
          "title = NEW.title   WHERE  id = OLD.id AND title = OLD.title   ;  "
          "END;";
    db << "CREATE TRIGGER music.trigger_delete_Preparelist INSTEAD OF DELETE "
          "ON Preparelist FOR EACH ROW BEGIN   DELETE FROM List WHERE type = 3 "
          "AND OLD.id = id AND OLD.title = title; END;";
    db << "CREATE TRIGGER music.trigger_update_Preparelist INSTEAD OF UPDATE "
          "ON Preparelist FOR EACH ROW BEGIN   UPDATE List SET id = NEW.id, "
          "title = NEW.title   WHERE  id = OLD.id AND title = OLD.title   ;  "
          "END;";
    db << "CREATE TRIGGER music.trigger_delete_Crate INSTEAD OF DELETE ON "
          "Crate FOR EACH ROW BEGIN   DELETE FROM List WHERE type = 4 AND "
          "OLD.id = id AND OLD.title = title AND OLD.path = path; END;";
    db << "CREATE TRIGGER music.trigger_update_Crate INSTEAD OF UPDATE ON "
          "Crate FOR EACH ROW BEGIN   UPDATE List SET id = NEW.id, title = "
          "NEW.title, path = NEW.path   WHERE  id = OLD.id AND title = "
          "OLD.title AND path = OLD.path   ;  END;";
    db << "CREATE TRIGGER music.trigger_insert_Playlist INSTEAD OF INSERT ON "
          "Playlist FOR EACH ROW BEGIN   INSERT INTO List ( id, type, title, "
          "path, isFolder, trackCount, ordering )    VALUES ( NEW.id, 1, "
          "NEW.title, NEW.title || \";\", 0, 0, NEW.id ) ;  INSERT INTO "
          "ListParentList ( listOriginId, listOriginType, listParentId, "
          "listParentType )   VALUES ( NEW.id, 1,            NEW.id, 1 ) ; "
          "END;";
    db << "CREATE TRIGGER music.trigger_insert_Historylist INSTEAD OF INSERT "
          "ON Historylist FOR EACH ROW BEGIN   INSERT INTO List ( id, type, "
          "title, path, isFolder, trackCount, ordering )    VALUES ( NEW.id, "
          "2, NEW.title, NEW.title || \";\", 0, 0, NEW.id ) ;  INSERT INTO "
          "ListParentList ( listOriginId, listOriginType, listParentId, "
          "listParentType )   VALUES ( NEW.id, 2,            NEW.id, 2 ) ; "
          "END;";
    db << "CREATE TRIGGER music.trigger_insert_Preparelist INSTEAD OF INSERT "
          "ON Preparelist FOR EACH ROW BEGIN   INSERT INTO List ( id, type, "
          "title, path, isFolder, trackCount, ordering )    VALUES ( NEW.id, "
          "3, NEW.title, NEW.title || \";\", 0, 0, NEW.id ) ;  INSERT INTO "
          "ListParentList ( listOriginId, listOriginType, listParentId, "
          "listParentType )   VALUES ( NEW.id, 3,            NEW.id, 3 ) ; "
          "END;";
    db << "CREATE TRIGGER music.trigger_insert_Crate INSTEAD OF INSERT ON "
          "Crate FOR EACH ROW BEGIN   INSERT INTO List ( id, type, title, "
          "path, isFolder, trackCount, ordering )    VALUES ( NEW.id, 4, "
          "NEW.title, NEW.path, 0, 0, NEW.id ) ; END;";
    db << "CREATE TRIGGER music.trigger_insert_order_update_List AFTER INSERT "
          "ON List FOR EACH ROW WHEN NEW.ordering IS NULL BEGIN    UPDATE List "
          "SET ordering = (SELECT IFNULL(MAX(ordering) + 1, 1) FROM List )     "
          "WHERE id = NEW.id AND type = NEW.type; END;";
    db << "CREATE TRIGGER music.trigger_after_insert_List AFTER INSERT ON List "
          "FOR EACH ROW BEGIN   UPDATE List   SET trackCount = 0    WHERE id = "
          "NEW.id AND type = NEW.type AND trackCount IS NULL   ;END;";
    db << "CREATE TRIGGER music.trigger_track_added_to_ListTrackList AFTER "
          "INSERT ON ListTrackList FOR EACH ROW BEGIN UPDATE List SET "
          "trackCount = trackCount + 1 WHERE id = NEW.listId AND type = "
          "NEW.listType; END;";
    db << "CREATE TRIGGER music.trigger_track_removed_from_ListTrackList AFTER "
          "DELETE ON ListTrackList FOR EACH ROW BEGIN UPDATE List SET "
          "trackCount = trackCount - 1 WHERE id = OLD.listId AND type = "
          "OLD.listType; END;";
    db << "CREATE TRIGGER music.trigger_delete_PlaylistTrackList INSTEAD OF "
          "DELETE ON PlaylistTrackList FOR EACH ROW BEGIN   DELETE FROM "
          "ListTrackList WHERE listType = 1 AND OLD.playlistId = listId AND "
          "OLD.trackId = trackId AND OLD.trackIdInOriginDatabase = "
          "trackIdInOriginDatabase AND OLD.databaseUuid = databaseUuid AND "
          "OLD.trackNumber = trackNumber; END;";
    db << "CREATE TRIGGER music.trigger_update_PlaylistTrackList INSTEAD OF "
          "UPDATE ON PlaylistTrackList FOR EACH ROW BEGIN   UPDATE "
          "ListTrackList SET listId = NEW.playlistId , trackId = NEW.trackId , "
          "trackIdInOriginDatabase = NEW.trackIdInOriginDatabase , "
          "databaseUuid = NEW.databaseUuid , trackNumber = NEW.trackNumber   "
          "WHERE listType = 1 AND OLD.playlistId = listId AND OLD.trackId = "
          "trackId AND OLD.trackIdInOriginDatabase = trackIdInOriginDatabase "
          "AND OLD.databaseUuid = databaseUuid AND OLD.trackNumber = "
          "trackNumber ; END;";
    db << "CREATE TRIGGER music.trigger_insert_PlaylistTrackList INSTEAD OF "
          "INSERT ON PlaylistTrackList FOR EACH ROW BEGIN   INSERT INTO "
          "ListTrackList ( listId, listType, trackId, trackIdInOriginDatabase, "
          "databaseUuid, trackNumber )                     SELECT "
          "NEW.playlistId, 1, NEW.trackId, NEW.trackIdInOriginDatabase, "
          "NEW.databaseUuid, NEW.trackNumber                    FROM List AS l "
          "WHERE l.id = NEW.playlistId AND l.type = 1 AND l.isFolder = 0 ; "
          "END;";
    db << "CREATE TRIGGER music.trigger_delete_HistorylistTrackList INSTEAD OF "
          "DELETE ON HistorylistTrackList FOR EACH ROW BEGIN   DELETE FROM "
          "ListTrackList WHERE listType = 2 AND OLD.historylistId = listId AND "
          "OLD.trackId = trackId AND OLD.trackIdInOriginDatabase = "
          "trackIdInOriginDatabase AND OLD.databaseUuid = databaseUuid; END;";
    db << "CREATE TRIGGER music.trigger_update_HistorylistTrackList INSTEAD OF "
          "UPDATE ON HistorylistTrackList FOR EACH ROW BEGIN   UPDATE "
          "ListTrackList SET listId = NEW.historylistId , trackId = "
          "NEW.trackId , trackIdInOriginDatabase = NEW.trackIdInOriginDatabase "
          ", databaseUuid = NEW.databaseUuid   WHERE listType = 2 AND "
          "OLD.historylistId = listId AND OLD.trackId = trackId AND "
          "OLD.trackIdInOriginDatabase = trackIdInOriginDatabase AND "
          "OLD.databaseUuid = databaseUuid ; END;";
    db << "CREATE TRIGGER music.trigger_insert_HistorylistTrackList INSTEAD OF "
          "INSERT ON HistorylistTrackList FOR EACH ROW BEGIN   INSERT INTO "
          "ListTrackList ( listId, listType, trackId, trackIdInOriginDatabase, "
          "databaseUuid, trackNumber )                     SELECT "
          "NEW.historylistId, 2, NEW.trackId, NEW.trackIdInOriginDatabase, "
          "NEW.databaseUuid, 0                    FROM List AS l WHERE l.id = "
          "NEW.historylistId AND l.type = 2 AND l.isFolder = 0 ; END;";
    db << "CREATE TRIGGER music.trigger_delete_PreparelistTrackList INSTEAD OF "
          "DELETE ON PreparelistTrackList FOR EACH ROW BEGIN   DELETE FROM "
          "ListTrackList WHERE listType = 3 AND OLD.playlistId = listId AND "
          "OLD.trackId = trackId AND OLD.trackIdInOriginDatabase = "
          "trackIdInOriginDatabase AND OLD.databaseUuid = databaseUuid AND "
          "OLD.trackNumber = trackNumber; END;";
    db << "CREATE TRIGGER music.trigger_update_PreparelistTrackList INSTEAD OF "
          "UPDATE ON PreparelistTrackList FOR EACH ROW BEGIN   UPDATE "
          "ListTrackList SET listId = NEW.playlistId , trackId = NEW.trackId , "
          "trackIdInOriginDatabase = NEW.trackIdInOriginDatabase , "
          "databaseUuid = NEW.databaseUuid , trackNumber = NEW.trackNumber   "
          "WHERE listType = 3 AND OLD.playlistId = listId AND OLD.trackId = "
          "trackId AND OLD.trackIdInOriginDatabase = trackIdInOriginDatabase "
          "AND OLD.databaseUuid = databaseUuid AND OLD.trackNumber = "
          "trackNumber ; END;";
    db << "CREATE TRIGGER music.trigger_insert_PreparelistTrackList INSTEAD OF "
          "INSERT ON PreparelistTrackList FOR EACH ROW BEGIN   INSERT INTO "
          "ListTrackList ( listId, listType, trackId, trackIdInOriginDatabase, "
          "databaseUuid, trackNumber )                     SELECT "
          "NEW.playlistId, 3, NEW.trackId, NEW.trackIdInOriginDatabase, "
          "NEW.databaseUuid, NEW.trackNumber                    FROM List AS l "
          "WHERE l.id = NEW.playlistId AND l.type = 3 AND l.isFolder = 0 ; "
          "END;";
    db << "CREATE TRIGGER music.trigger_delete_CrateTrackList INSTEAD OF "
          "DELETE ON CrateTrackList FOR EACH ROW BEGIN   DELETE FROM "
          "ListTrackList WHERE listType = 4 AND OLD.crateId = listId AND "
          "OLD.trackId = trackId; END;";
    db << "CREATE TRIGGER music.trigger_insert_CrateTrackList INSTEAD OF "
          "INSERT ON CrateTrackList FOR EACH ROW BEGIN   INSERT INTO "
          "ListTrackList ( listId, listType, trackId, trackIdInOriginDatabase, "
          "databaseUuid, trackNumber )    VALUES ( NEW.crateId, 4, "
          "NEW.trackId, 0, 0, 0 ) ; END;";
    db << "CREATE INDEX music.index_ListHierarchy_listId ON ListHierarchy ( "
          "listId );";
    db << "CREATE INDEX music.index_ListHierarchy_listType ON ListHierarchy ( "
          "listType );";
    db << "CREATE INDEX music.index_ListHierarchy_listIdChild ON ListHierarchy "
          "( listIdChild );";
    db << "CREATE INDEX music.index_ListHierarchy_listTypeChild ON "
          "ListHierarchy ( listTypeChild );";
    db << "CREATE VIEW music.CrateHierarchy AS SELECT listId AS crateId, "
          "listIdChild AS crateIdChild FROM ListHierarchy WHERE listType = 4 "
          "AND listTypeChild = 4;";
    db << "CREATE TRIGGER music.trigger_delete_CrateHierarchy INSTEAD OF "
          "DELETE ON CrateHierarchy FOR EACH ROW BEGIN   DELETE FROM "
          "ListHierarchy WHERE listId = OLD.crateId AND listType = 4 AND "
          "listIdChild = OLD.crateIdChild AND listTypeChild = 4 ;  END;";
    db << "CREATE TRIGGER music.trigger_insert_CrateHierarchy INSTEAD OF "
          "INSERT ON CrateHierarchy FOR EACH ROW BEGIN   INSERT INTO "
          "ListHierarchy ( listId, listType, listIdChild, listTypeChild )    "
          "VALUES ( NEW.crateId, 4, NEW.crateIdChild, 4 ) ; END;";
    db << "CREATE INDEX music.index_ListParentList_listOriginId ON "
          "ListParentList ( listOriginId );";
    db << "CREATE INDEX music.index_ListParentList_listOriginType ON "
          "ListParentList ( listOriginType );";
    db << "CREATE INDEX music.index_ListParentList_listParentId ON "
          "ListParentList ( listParentId );";
    db << "CREATE INDEX music.index_ListParentList_listParentType ON "
          "ListParentList ( listParentType );";
    db << "CREATE VIEW music.CrateParentList AS SELECT listOriginId AS "
          "crateOriginId, listParentId AS crateParentId FROM ListParentList "
          "WHERE listOriginType = 4 AND listParentType = 4;";
    db << "CREATE TRIGGER music.trigger_delete_CrateParentList INSTEAD OF "
          "DELETE ON CrateParentList FOR EACH ROW BEGIN   DELETE FROM "
          "ListParentList WHERE OLD.crateOriginId = listOriginId AND "
          "listOriginType = 4 AND OLD.crateParentId = listParentId AND "
          "listParentType = 4; END;";
    db << "CREATE TRIGGER music.trigger_insert_CrateParentList INSTEAD OF "
          "INSERT ON CrateParentList FOR EACH ROW BEGIN   INSERT INTO "
          "ListParentList ( listOriginId, listOriginType, listParentId, "
          "listParentType )    VALUES ( NEW.crateOriginId, 4, "
          "NEW.crateParentId, 4 ) ; END;";
    db << "CREATE TRIGGER music.trigger_after_update_Track AFTER UPDATE ON "
          "Track FOR EACH ROW BEGIN INSERT INTO ChangeLog (itemId) "
          "VALUES(NEW.id); END;";
    db << "CREATE TRIGGER music.trigger_after_update_MetaData AFTER UPDATE ON "
          "MetaData FOR EACH ROW BEGIN INSERT INTO ChangeLog (itemId) "
          "VALUES(NEW.id); END;";
    db << "CREATE TRIGGER music.trigger_after_update_MetaDataInteger AFTER "
          "UPDATE ON MetaDataInteger FOR EACH ROW BEGIN INSERT INTO ChangeLog "
          "(itemId) VALUES(NEW.id); END;";

    // Generate UUID for the Information table.
    auto uuid_str = djinterop::util::generate_random_uuid();

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

void schema_1_17_0::create_performance_schema(sqlite::database& db)
{
    db << "CREATE TABLE perfdata.PerformanceData ( [id] INTEGER, [isAnalyzed] "
          "NUMERIC, [isRendered] NUMERIC, [trackData] BLOB, "
          "[highResolutionWaveFormData] BLOB, [overviewWaveFormData] BLOB, "
          "[beatData] BLOB, [quickCues] BLOB, [loops] BLOB, [hasSeratoValues] "
          "NUMERIC, [hasRekordboxValues] NUMERIC, [hasTraktorValues] NUMERIC, "
          "PRIMARY KEY ( [id] ) );";
    db << "CREATE TABLE perfdata.Information ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [uuid] TEXT, [schemaVersionMajor] INTEGER, "
          "[schemaVersionMinor] INTEGER, [schemaVersionPatch] INTEGER, "
          "[currentPlayedIndiciator] INTEGER, "
          "[lastRekordBoxLibraryImportReadCounter] INTEGER);";
    db << "CREATE TABLE perfdata.ChangeLog ( [id] INTEGER PRIMARY KEY "
          "AUTOINCREMENT, [itemId] INTEGER);";
    db << "CREATE INDEX perfdata.index_PerformanceData_id ON PerformanceData ( "
          "id );";
    db << "CREATE INDEX perfdata.index_Information_id ON Information ( id );";
    db << "CREATE TRIGGER perfdata.trigger_after_update_PerformanceData AFTER "
          "UPDATE ON PerformanceData FOR EACH ROW BEGIN INSERT INTO ChangeLog "
          "(itemId) VALUES(NEW.id); END;";

    // Generate UUID for the Information table
    auto uuid_str = djinterop::util::generate_random_uuid();

    // Insert row into Information
    db << "INSERT INTO perfdata.Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator], [lastRekordBoxLibraryImportReadCounter]) "
          "VALUES (?, ?, ?, ?, ?, ?)"
       << uuid_str << schema_version.maj << schema_version.min
       << schema_version.pat << 0 << 0;
}

}  // namespace djinterop::engine::schema
