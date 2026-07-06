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

#include "schema.hpp"

#include <sqlite3.h>

#include <array>
#include <stdexcept>
#include <string>

namespace djinterop::onelibrary
{

const char* const schema_db_version = "1000";

namespace
{
void exec_sql(sqlite3* db, const char* sql)
{
    char* err_msg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK)
    {
        std::string msg = err_msg ? err_msg : "unknown error";
        sqlite3_free(err_msg);
        throw onelibrary_error{"SQL error: " + msg};
    }
}

}  // anonymous namespace

// Tables

const char* ddl_content = R"SQL(
CREATE TABLE content (
    content_id                      INTEGER,
    title                           TEXT,
    titleForSearch                  TEXT,
    subtitle                        TEXT,
    bpmx100                         INTEGER,
    length                          INTEGER,
    trackNo                         INTEGER,
    discNo                          INTEGER,
    artist_id_artist                INTEGER,
    artist_id_remixer               INTEGER,
    artist_id_originalArtist        INTEGER,
    artist_id_composer              INTEGER,
    artist_id_lyricist              INTEGER,
    album_id                        INTEGER,
    genre_id                        INTEGER,
    label_id                        INTEGER,
    key_id                          INTEGER,
    color_id                        INTEGER,
    image_id                        INTEGER,
    djComment                       TEXT,
    rating                          INTEGER,
    releaseYear                     INTEGER,
    releaseDate                     TEXT,
    dateCreated                     TEXT,
    dateAdded                       TEXT,
    path                            TEXT,
    fileName                        TEXT,
    fileSize                        INTEGER,
    fileType                        INTEGER,
    bitrate                         INTEGER,
    bitDepth                        INTEGER,
    samplingRate                    INTEGER,
    isrc                            TEXT,
    djPlayCount                     INTEGER,
    isHotCueAutoLoadOn              INTEGER,
    isKuvoDeliverStatusOn           INTEGER,
    kuvoDeliveryComment             TEXT,
    masterDbId                      INTEGER,
    masterContentId                 INTEGER,
    analysisDataFilePath            TEXT,
    analysedBits                    INTEGER,
    contentLink                     INTEGER,
    hasModified                     INTEGER,
    cueUpdateCount                  INTEGER,
    analysisDataUpdateCount         INTEGER,
    informationUpdateCount          INTEGER
))SQL";

const char* ddl_artist = R"SQL(
CREATE TABLE artist (
    artist_id       INTEGER,
    name            TEXT,
    nameForSearch   TEXT
))SQL";

const char* ddl_album = R"SQL(
CREATE TABLE album (
    album_id        INTEGER,
    name            TEXT,
    artist_id       INTEGER,
    image_id        INTEGER,
    isComplation    INTEGER,
    nameForSearch   TEXT
))SQL";

const char* ddl_genre = R"SQL(
CREATE TABLE genre (
    genre_id    INTEGER,
    name        TEXT
))SQL";

const char* ddl_label = R"SQL(
CREATE TABLE label (
    label_id    INTEGER,
    name        TEXT
))SQL";

const char* ddl_key = R"SQL(
CREATE TABLE key (
    key_id  INTEGER,
    name    TEXT
))SQL";

const char* ddl_color = R"SQL(
CREATE TABLE color (
    color_id    INTEGER,
    name        TEXT
))SQL";

const char* ddl_image = R"SQL(
CREATE TABLE image (
    image_id    INTEGER,
    path        TEXT
))SQL";

const char* ddl_cue = R"SQL(
CREATE TABLE cue (
    cue_id                          INTEGER,
    content_id                      INTEGER,
    kind                            INTEGER,
    colorTableIndex                 INTEGER,
    cueComment                      TEXT,
    isActiveLoop                    INTEGER,
    beatLoopNumerator               INTEGER,
    beatLoopDenominator             INTEGER,
    inUsec                          INTEGER,
    outUsec                         INTEGER,
    in150FramePerSec                INTEGER,
    out150FramePerSec               INTEGER,
    inMpegFrameNumber               INTEGER,
    outMpegFrameNumber              INTEGER,
    inMpegAbs                       INTEGER,
    outMpegAbs                      INTEGER,
    inDecodingStartFramePosition    INTEGER,
    outDecodingStartFramePosition   INTEGER,
    inFileOffsetInBlock             INTEGER,
    OutFileOffsetInBlock            INTEGER,
    inNumberOfSampleInBlock         INTEGER,
    outNumberOfSampleInBlock        INTEGER
))SQL";

const char* ddl_playlist = R"SQL(
CREATE TABLE playlist (
    playlist_id         INTEGER,
    sequenceNo          INTEGER,
    name                TEXT,
    image_id            INTEGER,
    attribute           INTEGER,
    playlist_id_parent  INTEGER
))SQL";

const char* ddl_playlist_content = R"SQL(
CREATE TABLE playlist_content (
    playlist_id INTEGER,
    content_id  INTEGER,
    sequenceNo  INTEGER
))SQL";

const char* ddl_history = R"SQL(
CREATE TABLE history (
    history_id          INTEGER,
    sequenceNo          INTEGER,
    name                TEXT,
    attribute           INTEGER,
    history_id_parent   INTEGER
))SQL";

const char* ddl_history_content = R"SQL(
CREATE TABLE history_content (
    history_id  INTEGER,
    content_id  INTEGER,
    sequenceNo  INTEGER
))SQL";

const char* ddl_hotCueBankList = R"SQL(
CREATE TABLE hotCueBankList (
    hotCueBankList_id           INTEGER,
    sequenceNo                  INTEGER,
    name                        TEXT,
    image_id                    INTEGER,
    attribute                   INTEGER,
    hotCueBankList_id_parent    INTEGER
))SQL";

const char* ddl_hotCueBankList_cue = R"SQL(
CREATE TABLE hotCueBankList_cue (
    hotCueBankList_id   INTEGER,
    cue_id              INTEGER,
    sequenceNo          INTEGER
))SQL";

const char* ddl_myTag = R"SQL(
CREATE TABLE myTag (
    myTag_id            INTEGER,
    sequenceNo          INTEGER,
    name                TEXT,
    attribute           INTEGER,
    myTag_id_parent     INTEGER
))SQL";

const char* ddl_myTag_content = R"SQL(
CREATE TABLE myTag_content (
    myTag_id    INTEGER,
    content_id  INTEGER
))SQL";

const char* ddl_menuItem = R"SQL(
CREATE TABLE menuItem (
    menuItem_id INTEGER,
    kind        INTEGER,
    name        TEXT
))SQL";

const char* ddl_category = R"SQL(
CREATE TABLE category (
    category_id INTEGER,
    menuItem_id INTEGER,
    sequenceNo  INTEGER,
    isVisible   INTEGER
))SQL";

const char* ddl_sort = R"SQL(
CREATE TABLE sort (
    sort_id                 INTEGER,
    menuItem_id             INTEGER,
    sequenceNo              INTEGER,
    isVisible               INTEGER,
    isSelectedAsSubColumn   INTEGER
))SQL";

const char* ddl_property = R"SQL(
CREATE TABLE property (
    deviceName              TEXT,
    dbVersion               TEXT,
    numberOfContents        INTEGER,
    createdDate             TEXT,
    backGroundColorType     INTEGER,
    myTagMasterDBID         INTEGER
))SQL";

const char* ddl_recommendedLike = R"SQL(
CREATE TABLE recommendedLike (
    content_id_1    INTEGER,
    content_id_2    INTEGER,
    rating          INTEGER,
    createdDate     INTEGER
))SQL";

// Indexes

const char* idx_playlist_content_playlist_id =
    "CREATE INDEX index_playlist_content_playlist_id "
    "ON playlist_content(playlist_id)";

const char* idx_myTag_content_content_id =
    "CREATE INDEX index_myTag_content_content_id "
    "ON myTag_content(content_id)";

const char* idx_myTag_content_myTag_id =
    "CREATE INDEX index_myTag_content_myTag_id "
    "ON myTag_content(myTag_id)";

const char* idx_hotCueBankList_cue_hotCueBankList_id =
    "CREATE INDEX index_hotCueBankList_cue_hotCueBankList_id "
    "ON hotCueBankList_cue(hotCueBankList_id)";

namespace
{
const char* default_color_rows[] = {
    "INSERT INTO color VALUES (1, 'Pink')",
    "INSERT INTO color VALUES (2, 'Red')",
    "INSERT INTO color VALUES (3, 'Orange')",
    "INSERT INTO color VALUES (4, 'Yellow')",
    "INSERT INTO color VALUES (5, 'Green')",
    "INSERT INTO color VALUES (6, 'Aqua')",
    "INSERT INTO color VALUES (7, 'Blue')",
    "INSERT INTO color VALUES (8, 'Purple')",
};

// U+FFFA (␚) and U+FFFB (␛) delimiters for localisable labels.  Stored as
// TEXT to match rekordbox.  Hex escapes are split into adjacent literals
// because C++ hex escape parsing is greedy.
const char* default_menuItem_rows[] = {
    "INSERT INTO menuItem VALUES (1, 128, '\xEF\xBF\xBA"
    "GENRE"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (2, 129, '\xEF\xBF\xBA"
    "ARTIST"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (3, 130, '\xEF\xBF\xBA"
    "ALBUM"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (4, 131, '\xEF\xBF\xBA"
    "TRACK"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (5, 133, '\xEF\xBF\xBA"
    "BPM"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (6, 134, '\xEF\xBF\xBA"
    "RATING"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (7, 135, '\xEF\xBF\xBA"
    "YEAR"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (8, 136, '\xEF\xBF\xBA"
    "REMIXER"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (9, 137, '\xEF\xBF\xBA"
    "LABEL"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (10, 138, '\xEF\xBF\xBA"
    "ORIGINAL ARTIST"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (11, 139, '\xEF\xBF\xBA"
    "KEY"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (12, 141, '\xEF\xBF\xBA"
    "CUE"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (13, 142, '\xEF\xBF\xBA"
    "COLOR"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (14, 146, '\xEF\xBF\xBA"
    "TIME"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (15, 147, '\xEF\xBF\xBA"
    "BITRATE"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (16, 148, '\xEF\xBF\xBA"
    "FILE NAME"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (17, 132, '\xEF\xBF\xBA"
    "PLAYLIST"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (18, 152, '\xEF\xBF\xBA"
    "HOT CUE BANK"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (19, 149, '\xEF\xBF\xBA"
    "HISTORY"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (20, 145, '\xEF\xBF\xBA"
    "SEARCH"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (21, 150, '\xEF\xBF\xBA"
    "COMMENTS"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (22, 140, '\xEF\xBF\xBA"
    "DATE ADDED"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (23, 151, '\xEF\xBF\xBA"
    "DJ PLAY COUNT"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (24, 144, '\xEF\xBF\xBA"
    "FOLDER"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (25, 161, '\xEF\xBF\xBA"
    "DEFAULT"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (26, 162, '\xEF\xBF\xBA"
    "ALPHABET"
    "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (27, 170, '\xEF\xBF\xBA"
    "MATCHING"
    "\xEF\xBF\xBB')",
};

const char* default_category_rows[] = {
    "INSERT INTO category VALUES (1, 1, 0, 0)",
    "INSERT INTO category VALUES (2, 2, 1, 1)",
    "INSERT INTO category VALUES (3, 3, 2, 1)",
    "INSERT INTO category VALUES (4, 4, 3, 1)",
    "INSERT INTO category VALUES (5, 17, 5, 1)",
    "INSERT INTO category VALUES (6, 5, 0, 0)",
    "INSERT INTO category VALUES (7, 6, 0, 0)",
    "INSERT INTO category VALUES (8, 7, 0, 0)",
    "INSERT INTO category VALUES (9, 8, 0, 0)",
    "INSERT INTO category VALUES (10, 9, 0, 0)",
    "INSERT INTO category VALUES (11, 10, 0, 0)",
    "INSERT INTO category VALUES (12, 11, 4, 1)",
    "INSERT INTO category VALUES (13, 13, 0, 0)",
    "INSERT INTO category VALUES (14, 24, 9, 1)",
    "INSERT INTO category VALUES (15, 20, 7, 1)",
    "INSERT INTO category VALUES (16, 14, 0, 0)",
    "INSERT INTO category VALUES (17, 15, 0, 0)",
    "INSERT INTO category VALUES (18, 16, 0, 0)",
    "INSERT INTO category VALUES (19, 19, 6, 1)",
    "INSERT INTO category VALUES (20, 18, 0, 0)",
    "INSERT INTO category VALUES (21, 27, 8, 1)",
};

const char* default_sort_rows[] = {
    "INSERT INTO sort VALUES (1, 25, 1, 1, 0)",
    "INSERT INTO sort VALUES (2, 26, 2, 1, 0)",
    "INSERT INTO sort VALUES (3, 2, 3, 1, 0)",
    "INSERT INTO sort VALUES (4, 3, 4, 1, 0)",
    "INSERT INTO sort VALUES (5, 5, 5, 1, 0)",
    "INSERT INTO sort VALUES (6, 6, 6, 1, 0)",
    "INSERT INTO sort VALUES (7, 1, 0, 0, 0)",
    "INSERT INTO sort VALUES (8, 21, 0, 0, 0)",
    "INSERT INTO sort VALUES (9, 14, 0, 0, 0)",
    "INSERT INTO sort VALUES (10, 8, 0, 0, 0)",
    "INSERT INTO sort VALUES (11, 9, 0, 0, 0)",
    "INSERT INTO sort VALUES (12, 10, 0, 0, 0)",
    "INSERT INTO sort VALUES (13, 11, 7, 1, 0)",
    "INSERT INTO sort VALUES (14, 15, 0, 0, 0)",
    "INSERT INTO sort VALUES (15, 13, 0, 0, 0)",
    "INSERT INTO sort VALUES (16, 23, 0, 0, 0)",
    "INSERT INTO sort VALUES (17, 22, 0, 0, 0)",
};

int count_rows(sqlite3* db, const char* sql)
{
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw onelibrary_error{
            std::string{"SQL prepare error: "} + sqlite3_errmsg(db)};
    }
    int count = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

}  // anonymous namespace

void create(void* db_handle)
{
    auto* db = static_cast<sqlite3*>(db_handle);

    const std::array<const char*, 22> ddl{
        ddl_content,
        ddl_artist,
        ddl_album,
        ddl_genre,
        ddl_label,
        ddl_key,
        ddl_color,
        ddl_image,
        ddl_cue,
        ddl_playlist,
        ddl_playlist_content,
        ddl_history,
        ddl_history_content,
        ddl_hotCueBankList,
        ddl_hotCueBankList_cue,
        ddl_myTag,
        ddl_myTag_content,
        ddl_menuItem,
        ddl_category,
        ddl_sort,
        ddl_property,
        ddl_recommendedLike,
    };

    for (auto* stmt : ddl)
        exec_sql(db, stmt);

    exec_sql(db, idx_playlist_content_playlist_id);
    exec_sql(db, idx_myTag_content_content_id);
    exec_sql(db, idx_myTag_content_myTag_id);
    exec_sql(db, idx_hotCueBankList_cue_hotCueBankList_id);
}

void verify(void* db_handle)
{
    auto* db = static_cast<sqlite3*>(db_handle);

    int tables =
        count_rows(db, "SELECT count(*) FROM sqlite_master WHERE type='table'");
    if (tables != 22)
    {
        throw onelibrary_error{
            "Expected 22 tables, found " + std::to_string(tables)};
    }

    int indexes =
        count_rows(db, "SELECT count(*) FROM sqlite_master WHERE type='index'");
    if (indexes != 4)
    {
        throw onelibrary_error{
            "Expected 4 indexes, found " + std::to_string(indexes)};
    }
}

void seed(void* db_handle)
{
    auto* db = static_cast<sqlite3*>(db_handle);

    for (auto* stmt : default_color_rows)
        exec_sql(db, stmt);

    for (auto* stmt : default_menuItem_rows)
        exec_sql(db, stmt);

    for (auto* stmt : default_category_rows)
        exec_sql(db, stmt);

    for (auto* stmt : default_sort_rows)
        exec_sql(db, stmt);

    const char* property_sql =
        "INSERT INTO property VALUES "
        "(?, ?, 0, strftime('%Y-%m-%d %H:%M:%S', 'now', 'localtime'), 0, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, property_sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw onelibrary_error{
            std::string{"SQL prepare error: "} + sqlite3_errmsg(db)};
    }
    sqlite3_bind_text(stmt, 1, "rbx", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, schema_db_version, -1, SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 3, 0);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE)
    {
        throw onelibrary_error{std::string{"SQL error: "} + sqlite3_errmsg(db)};
    }
}

}  // namespace djinterop::onelibrary
