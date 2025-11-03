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
#include "schema_3_1_0.hpp"

namespace djinterop::engine::schema
{
void schema_3_1_0::create(sqlite::database& db)
{
      // Schema 3.1.0 modifies trigger definitions on the Track and
      // PerformanceData tables.
    db << "CREATE TABLE Information ( 	id INTEGER PRIMARY KEY AUTOINCREMENT, "
          "	uuid TEXT, 	schemaVersionMajor INTEGER, 	schemaVersionMinor "
          "INTEGER, 	schemaVersionPatch INTEGER, 	"
          "currentPlayedIndiciator INTEGER, 	"
          "lastRekordBoxLibraryImportReadCounter INTEGER);";
    db << "CREATE TABLE AlbumArt ( 	id INTEGER PRIMARY KEY AUTOINCREMENT, 	"
          "hash TEXT, 	albumArt BLOB );";
    db << "CREATE TABLE Pack ( 	id INTEGER PRIMARY KEY AUTOINCREMENT, 	packId "
          "TEXT, 	changeLogDatabaseUuid TEXT, 	changeLogId INTEGER, 	"
          "lastPackTime DATETIME );";
    db << "CREATE TABLE Playlist ( 	id INTEGER PRIMARY KEY AUTOINCREMENT, 	"
          "title TEXT, 	parentListId INTEGER, 	isPersisted BOOLEAN, 	"
          "nextListId INTEGER, 	lastEditTime DATETIME, 	isExplicitlyExported "
          "BOOLEAN, 	CONSTRAINT C_NAME_UNIQUE_FOR_PARENT UNIQUE (title, "
          "parentListId), 	CONSTRAINT C_NEXT_LIST_ID_UNIQUE_FOR_PARENT UNIQUE "
          "(parentListId, nextListId) );";
    db << "CREATE TABLE PlaylistEntity ( 	id INTEGER PRIMARY KEY "
          "AUTOINCREMENT, 	listId INTEGER, 	trackId INTEGER, 	"
          "databaseUuid TEXT, 	nextEntityId INTEGER, 	membershipReference "
          "INTEGER, 	CONSTRAINT C_NAME_UNIQUE_FOR_LIST UNIQUE (listId, "
          "databaseUuid, trackId), 	FOREIGN KEY (listId) REFERENCES Playlist "
          "(id) ON DELETE CASCADE );";
    db << "CREATE TABLE Smartlist ( 	listUuid TEXT NOT NULL PRIMARY KEY, "
          "	title TEXT, 	parentPlaylistPath TEXT, 	nextPlaylistPath TEXT, "
          "	nextListUuid TEXT, 	rules TEXT, 	lastEditTime DATETIME, 	"
          "CONSTRAINT C_NAME_UNIQUE_FOR_PARENT UNIQUE (title, "
          "parentPlaylistPath), 	CONSTRAINT C_NEXT_LIST_UNIQUE_FOR_PARENT "
          "UNIQUE (parentPlaylistPath, nextPlaylistPath, nextListUuid) );";
    db << "CREATE TABLE Track ( 	id INTEGER PRIMARY KEY AUTOINCREMENT, 	"
          "playOrder INTEGER, 	length INTEGER, 	bpm INTEGER, 	year "
          "INTEGER, 	path TEXT, 	filename TEXT, 	bitrate INTEGER, 	"
          "bpmAnalyzed REAL, 	albumArtId INTEGER, 	fileBytes INTEGER, 	"
          "title TEXT, 	artist TEXT, 	album TEXT, 	genre TEXT, 	"
          "comment TEXT, 	label TEXT, 	composer TEXT, 	remixer TEXT, 	"
          "key INTEGER, 	rating INTEGER, 	albumArt TEXT, 	timeLastPlayed "
          "DATETIME, 	isPlayed BOOLEAN, 	fileType TEXT, 	isAnalyzed "
          "BOOLEAN, 	dateCreated DATETIME, 	dateAdded DATETIME, 	"
          "isAvailable BOOLEAN, 	isMetadataOfPackedTrackChanged BOOLEAN, "
          "	isPerfomanceDataOfPackedTrackChanged BOOLEAN, 	playedIndicator "
          "INTEGER, 	isMetadataImported BOOLEAN, 	pdbImportKey INTEGER, "
          "	streamingSource TEXT, 	uri TEXT, 	isBeatGridLocked BOOLEAN, 	"
          "originDatabaseUuid TEXT, 	originTrackId INTEGER, 	streamingFlags "
          "INTEGER, 	explicitLyrics BOOLEAN, 	lastEditTime DATETIME, 	"
          "CONSTRAINT C_originDatabaseUuid_originTrackId UNIQUE "
          "(originDatabaseUuid, originTrackId), 	CONSTRAINT C_path UNIQUE "
          "(path), 	FOREIGN KEY (albumArtId) REFERENCES AlbumArt (id) ON "
          "DELETE RESTRICT );";
    db << "CREATE TABLE PerformanceData ( 	trackId INTEGER PRIMARY KEY, 	"
          "trackData BLOB, 	overviewWaveFormData BLOB, 	beatData BLOB, 	"
          "quickCues BLOB, 	loops BLOB, 	thirdPartySourceId INTEGER, 	"
          "activeOnLoadLoops INTEGER, 	FOREIGN KEY(trackId) REFERENCES "
          "Track(id) ON DELETE CASCADE ON UPDATE CASCADE );";
    db << "CREATE TABLE PreparelistEntity ( 	id INTEGER PRIMARY KEY "
          "AUTOINCREMENT, 	trackId INTEGER, 	trackNumber INTEGER, 	"
          "FOREIGN KEY (trackId) REFERENCES Track (id) ON DELETE CASCADE );";
    db << "DELETE FROM sqlite_sequence;";
    db << "CREATE INDEX index_AlbumArt_hash ON AlbumArt (hash);";
    db << "CREATE INDEX index_PlaylistEntity_nextEntityId_listId ON "
          "PlaylistEntity(nextEntityId, listId);";
    db << "CREATE TRIGGER trigger_after_insert_Pack_timestamp AFTER INSERT ON "
          "Pack FOR EACH ROW WHEN NEW.lastPackTime IS NULL BEGIN 	UPDATE "
          "Pack SET lastPackTime = strftime('%s') WHERE ROWID = NEW.ROWID; "
          "END;";
    db << "CREATE TRIGGER trigger_after_insert_Pack_changeLogId AFTER INSERT "
          "ON Pack FOR EACH ROW WHEN NEW.changeLogId = 0 BEGIN 	UPDATE Pack "
          "SET changeLogId = 1 WHERE ROWID = NEW.ROWID; END;";
    db << "CREATE VIEW ChangeLog (id, trackId) AS SELECT 0, 0 WHERE FALSE;";
    db << "CREATE TRIGGER trigger_before_insert_List BEFORE INSERT ON Playlist "
          "FOR EACH ROW BEGIN 	UPDATE Playlist SET 		nextListId = -(1 + "
          "nextListId) 	WHERE nextListId = NEW.nextListId 	AND parentListId = "
          "NEW.parentListId; END;";
    db << "CREATE TRIGGER trigger_after_insert_List AFTER INSERT ON Playlist "
          "FOR EACH ROW BEGIN 	UPDATE Playlist SET 		nextListId = "
          "NEW.id 	WHERE nextListId = -(1 + NEW.nextListId) 	AND "
          "parentListId = NEW.parentListId; END;";
    db << "CREATE TRIGGER trigger_after_delete_List AFTER DELETE ON Playlist "
          "FOR EACH ROW BEGIN 	UPDATE Playlist SET 		nextListId = "
          "OLD.nextListId 	WHERE nextListId = OLD.id; 	DELETE FROM Playlist "
          "	WHERE parentListId = OLD.id; END;";
    db << "CREATE TRIGGER trigger_after_update_isPersistParent AFTER UPDATE ON "
          "Playlist 	WHEN (old.isPersisted = 0 	AND new.isPersisted = 1) "
          "	OR (old.parentListId != new.parentListId 	AND new.isPersisted = "
          "1) BEGIN 	UPDATE Playlist SET 		isPersisted = 1 	WHERE "
          "id IN (SELECT parentListId FROM PlaylistAllParent WHERE id=new.id); "
          "END;";
    db << "CREATE TRIGGER trigger_after_update_isPersistChild AFTER UPDATE ON "
          "Playlist 	WHEN old.isPersisted = 1 	AND new.isPersisted = 0 "
          "BEGIN 	UPDATE Playlist SET 		isPersisted = 0 	WHERE id "
          "IN (SELECT childListId FROM PlaylistAllChildren WHERE id=new.id); "
          "END;";
    db << "CREATE TRIGGER trigger_after_insert_isPersist AFTER INSERT ON "
          "Playlist 	WHEN new.isPersisted = 1 BEGIN 	UPDATE Playlist SET "
          "		isPersisted = 1 	WHERE id IN (SELECT parentListId FROM "
          "PlaylistAllParent WHERE id=new.id); END;";
    db << "CREATE VIEW PlaylistAllParent AS WITH FindAllParent AS ( 	SELECT "
          "id, parentListId FROM Playlist 	UNION ALL 	SELECT "
          "recursiveCTE.id, Plist.parentListId FROM Playlist Plist 	INNER JOIN "
          "FindAllParent recursiveCTE 	ON recursiveCTE.parentListId = "
          "Plist.id ) SELECT * FROM FindAllParent;";
    db << "CREATE VIEW PlaylistAllChildren AS WITH FindAllChild AS ( SELECT "
          "id, id as childListId FROM Playlist UNION ALL SELECT "
          "recursiveCTE.id, Plist.id FROM Playlist Plist INNER JOIN "
          "FindAllChild recursiveCTE ON recursiveCTE.childListId = "
          "Plist.parentListId ) SELECT * FROM FindAllChild WHERE id <> "
          "childListId;";
    db << "CREATE VIEW PlaylistPath AS WITH RECURSIVE Heirarchy AS ( 	SELECT "
          "id AS child, parentListId AS parent, title AS name, 1 AS depth FROM "
          "Playlist 	UNION ALL 	SELECT child, parentListId AS parent, "
          "title AS name, h.depth + 1 AS depth FROM Playlist c 	JOIN Heirarchy "
          "h ON h.parent = c.id 	ORDER BY depth DESC ), OrderedList AS ( "
          "	SELECT id , nextListId, 1 AS position 	FROM Playlist 	WHERE "
          "nextListId = 0 	UNION ALL 	SELECT c.id , c.nextListId , "
          "l.position + 1 	FROM Playlist c 	INNER JOIN OrderedList l 	ON "
          "c.nextListId = l.id ), NameConcat AS ( 	SELECT 		child AS id, "
          "		GROUP_CONCAT(name ,';') || ';' AS path 	FROM 	( 		SELECT "
          "child, name 		FROM Heirarchy 		ORDER BY depth DESC 	) 	"
          "GROUP BY child ) SELECT 	id, 	path, 	ROW_NUMBER() OVER 	( 	"
          "	ORDER BY 		(SELECT COUNT(*) FROM (SELECT * FROM Heirarchy "
          "WHERE child = id) ) DESC, 		(SELECT position FROM OrderedList "
          "ol WHERE ol.id = c.id) ASC 	) AS position FROM Playlist c LEFT "
          "JOIN NameConcat g USING (id);";
    db << "CREATE TRIGGER trigger_before_delete_PlaylistEntity BEFORE DELETE "
          "ON PlaylistEntity WHEN OLD.trackId > 0 BEGIN 	UPDATE "
          "PlaylistEntity SET 		nextEntityId = OLD.nextEntityId 	WHERE "
          "nextEntityId = OLD.id 	AND listId = OLD.listId; END;";
    db << "CREATE INDEX index_Track_filename ON Track (filename);";
    db << "CREATE INDEX index_Track_albumArtId ON Track (albumArtId);";
    db << "CREATE INDEX index_Track_uri ON Track (uri);";
    db << "CREATE INDEX index_Track_title ON Track(title);";
    db << "CREATE INDEX index_Track_length ON Track(length);";
    db << "CREATE INDEX index_Track_rating ON Track(rating);";
    db << "CREATE INDEX index_Track_year ON Track(year);";
    db << "CREATE INDEX index_Track_dateAdded ON Track(dateAdded);";
    db << "CREATE INDEX index_Track_genre ON Track(genre);";
    db << "CREATE INDEX index_Track_artist ON Track(artist);";
    db << "CREATE INDEX index_Track_album ON Track(album);";
    db << "CREATE INDEX index_Track_key ON Track(key);";
    db << "CREATE INDEX index_Track_bpmAnalyzed ON Track(CAST(bpmAnalyzed + "
          "0.5 AS int));";
    db << "CREATE TRIGGER trigger_after_insert_Track_check_id AFTER INSERT ON "
          "Track 	WHEN NEW.id <= (SELECT seq FROM sqlite_sequence WHERE name "
          "= 'Track') BEGIN 	SELECT RAISE(ABORT, 'Recycling deleted track "
          "id''s are not allowed'); END;";
    db << "CREATE TRIGGER trigger_after_update_Track_check_Id BEFORE UPDATE ON "
          "Track 	WHEN NEW.id <> OLD.id BEGIN 	SELECT RAISE(ABORT, "
          "'Changing track id''s are not allowed'); END;";
    db << "CREATE TRIGGER trigger_after_insert_Track_fix_origin AFTER INSERT "
          "ON Track 	WHEN IFNULL(NEW.originTrackId, 0) = 0 	OR "
          "IFNULL(NEW.originDatabaseUuid, '') = '' BEGIN 	UPDATE Track SET "
          "		originTrackId = NEW.id, 		originDatabaseUuid = (SELECT "
          "uuid FROM Information) 	WHERE track.id = NEW.id; END;";
    db << "CREATE TRIGGER trigger_after_update_Track_fix_origin AFTER UPDATE "
          "ON Track 	WHEN IFNULL(NEW.originTrackId, 0) = 0 	OR "
          "IFNULL(NEW.originDatabaseUuid, '') = '' BEGIN 	UPDATE Track SET "
          "		originTrackId = NEW.id, 		originDatabaseUuid = (SELECT "
          "uuid FROM Information) 	WHERE track.id = NEW.id; END;";
    db << "CREATE TRIGGER trigger_after_update_only_Track_timestamp "
          "AFTER UPDATE OF length,"
          "bpm, year, filename, bitrate, bpmAnalyzed, albumArtId, title, "
          "artist,"
          "album, genre, comment, label, composer, remixer, key, rating, "
          "albumArt,"
          "fileType, isAnalyzed, isBeatgridLocked,"
          "explicitLyrics ON Track FOR EACH ROW BEGIN UPDATE Track SET"
          "    lastEditTime = strftime('%s') WHERE ROWID = NEW.ROWID;"
          "END;";
    db << "CREATE TRIGGER trigger_PerformanceData_after_update_Track_timestamp "
          "AFTER UPDATE OF trackData, isAnalyzed, overviewWaveFormData, "
          "beatData, quickCues, loops, activeOnLoadLoops ON PerformanceData "
          "FOR EACH ROW BEGIN UPDATE Track SET lastEditTime = strftime('%s') "
          "WHERE id = NEW.trackId; "
          "END;";
    db << "CREATE TRIGGER trigger_after_insert_Track_insert_performance_data "
          "AFTER INSERT ON Track BEGIN 	INSERT INTO "
          "PerformanceData(trackId) "
          "VALUES(NEW.id); END;";
    db << "CREATE INDEX index_PreparelistEntity_trackId ON PreparelistEntity "
          "(trackId);";

    // Generate UUID for the Information table.
    auto uuid_str = djinterop::util::generate_random_uuid();

    // Not yet sure how the "currentPlayedIndiciator" (typo deliberate) value
    // is formed.
    auto current_played_indicator_fake_value =
        djinterop::util::generate_random_int64();

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
