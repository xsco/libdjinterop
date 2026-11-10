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

#include "../util/random.hpp"

#include <array>

namespace djinterop::onelibrary
{
namespace
{
const char* const schema_db_version = "1000";

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

// U+FFFA (␚) and U+FFFB (␛) delimiters for localisable labels.
const char* default_menuItem_rows[] = {
    "INSERT INTO menuItem VALUES (1, 128, '\xEF\xBF\xBA" "GENRE" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (2, 129, '\xEF\xBF\xBA" "ARTIST" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (3, 130, '\xEF\xBF\xBA" "ALBUM" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (4, 131, '\xEF\xBF\xBA" "TRACK" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (5, 133, '\xEF\xBF\xBA" "BPM" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (6, 134, '\xEF\xBF\xBA" "RATING" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (7, 135, '\xEF\xBF\xBA" "YEAR" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (8, 136, '\xEF\xBF\xBA" "REMIXER" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (9, 137, '\xEF\xBF\xBA" "LABEL" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (10, 138, '\xEF\xBF\xBA" "ORIGINAL ARTIST" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (11, 139, '\xEF\xBF\xBA" "KEY" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (12, 141, '\xEF\xBF\xBA" "CUE" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (13, 142, '\xEF\xBF\xBA" "COLOR" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (14, 146, '\xEF\xBF\xBA" "TIME" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (15, 147, '\xEF\xBF\xBA" "BITRATE" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (16, 148, '\xEF\xBF\xBA" "FILE NAME" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (17, 132, '\xEF\xBF\xBA" "PLAYLIST" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (18, 152, '\xEF\xBF\xBA" "HOT CUE BANK" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (19, 149, '\xEF\xBF\xBA" "HISTORY" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (20, 145, '\xEF\xBF\xBA" "SEARCH" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (21, 150, '\xEF\xBF\xBA" "COMMENTS" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (22, 140, '\xEF\xBF\xBA" "DATE ADDED" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (23, 151, '\xEF\xBF\xBA" "DJ PLAY COUNT" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (24, 144, '\xEF\xBF\xBA" "FOLDER" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (25, 161, '\xEF\xBF\xBA" "DEFAULT" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (26, 162, '\xEF\xBF\xBA" "ALPHABET" "\xEF\xBF\xBB')",
    "INSERT INTO menuItem VALUES (27, 170, '\xEF\xBF\xBA" "MATCHING" "\xEF\xBF\xBB')",
};

// ID skips are identical to RB.
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
    "INSERT INTO category VALUES (15, 13, 0, 0)",
    "INSERT INTO category VALUES (17, 24, 9, 1)",
    "INSERT INTO category VALUES (18, 20, 7, 1)",
    "INSERT INTO category VALUES (19, 14, 0, 0)",
    "INSERT INTO category VALUES (20, 15, 0, 0)",
    "INSERT INTO category VALUES (21, 16, 0, 0)",
    "INSERT INTO category VALUES (22, 19, 6, 1)",
    "INSERT INTO category VALUES (23, 18, 0, 0)",
    "INSERT INTO category VALUES (26, 27, 8, 1)",
    "INSERT INTO category VALUES (27, 22, 10, 1)",
};

// id 14 is intentionally skipped similarly to RB.
const char* default_sort_rows[] = {
    "INSERT INTO sort VALUES (0, 25, 1, 1, 0)",
    "INSERT INTO sort VALUES (1, 26, 2, 1, 0)",
    "INSERT INTO sort VALUES (2, 2, 3, 1, 0)",
    "INSERT INTO sort VALUES (3, 3, 4, 1, 0)",
    "INSERT INTO sort VALUES (4, 5, 5, 1, 0)",
    "INSERT INTO sort VALUES (5, 6, 6, 1, 0)",
    "INSERT INTO sort VALUES (6, 1, 0, 0, 0)",
    "INSERT INTO sort VALUES (7, 21, 0, 0, 0)",
    "INSERT INTO sort VALUES (8, 14, 0, 0, 0)",
    "INSERT INTO sort VALUES (9, 8, 0, 0, 0)",
    "INSERT INTO sort VALUES (10, 9, 0, 0, 0)",
    "INSERT INTO sort VALUES (11, 10, 0, 0, 0)",
    "INSERT INTO sort VALUES (12, 11, 7, 1, 0)",
    "INSERT INTO sort VALUES (13, 15, 0, 0, 0)",
    "INSERT INTO sort VALUES (15, 13, 0, 0, 0)",
    "INSERT INTO sort VALUES (16, 23, 0, 0, 0)",
    "INSERT INTO sort VALUES (17, 22, 0, 0, 0)",
};

// Default rekordbox My Tag tree.
const char* default_myTag_rows[] = {
    "INSERT INTO myTag VALUES (1, 0, 'Genre', 1, 0)",
    "INSERT INTO myTag VALUES (2, 1, 'Components', 1, 0)",
    "INSERT INTO myTag VALUES (3, 2, 'Situation', 1, 0)",
    "INSERT INTO myTag VALUES (4, 3, 'Untitled Column', 1, 0)",
    "INSERT INTO myTag VALUES (78619492, 3, 'Nu Disco', 0, 1)",
    "INSERT INTO myTag VALUES (542690471, 1, 'Deep House', 0, 1)",
    "INSERT INTO myTag VALUES (557783755, 0, 'My Comment', 0, 4)",
    "INSERT INTO myTag VALUES (734835734, 7, 'Build down', 0, 3)",
    "INSERT INTO myTag VALUES (924150941, 5, 'Piano', 0, 2)",
    "INSERT INTO myTag VALUES (1157481955, 0, 'Main Floor', 0, 3)",
    "INSERT INTO myTag VALUES (1373956844, 5, 'Build up', 0, 3)",
    "INSERT INTO myTag VALUES (1494640599, 4, 'Percussion', 0, 2)",
    "INSERT INTO myTag VALUES (1574634827, 5, 'Bass Music', 0, 1)",
    "INSERT INTO myTag VALUES (1588221825, 1, 'Vocal', 0, 2)",
    "INSERT INTO myTag VALUES (1829832119, 2, 'Techno', 0, 1)",
    "INSERT INTO myTag VALUES (1945901698, 6, 'Trap', 0, 1)",
    "INSERT INTO myTag VALUES (2453969498, 3, 'Sub Bass', 0, 2)",
    "INSERT INTO myTag VALUES (2799474499, 6, 'Dark', 0, 2)",
    "INSERT INTO myTag VALUES (2875086209, 2, 'Lounge', 0, 3)",
    "INSERT INTO myTag VALUES (2929018516, 2, 'Beat', 0, 2)",
    "INSERT INTO myTag VALUES (3007189819, 6, 'Peak Time', 0, 3)",
    "INSERT INTO myTag VALUES (3146290123, 3, 'Mid Night', 0, 3)",
    "INSERT INTO myTag VALUES (3167910249, 4, 'Morning', 0, 3)",
    "INSERT INTO myTag VALUES (3265347719, 0, 'Synth', 0, 2)",
    "INSERT INTO myTag VALUES (3366526410, 4, 'Electro House', 0, 1)",
    "INSERT INTO myTag VALUES (3795478438, 0, 'Acid House', 0, 1)",
    "INSERT INTO myTag VALUES (3944383242, 7, 'Upper', 0, 2)",
    "INSERT INTO myTag VALUES (4207964230, 1, 'Second Floor', 0, 3)",
};

}  // anonymous namespace

void create_tables(sqlite::database& db)
{
    const std::array<const char*, 22> ddl{
        ddl_content, ddl_artist, ddl_album, ddl_genre, ddl_label,
        ddl_key, ddl_color, ddl_image, ddl_cue, ddl_playlist,
        ddl_playlist_content, ddl_history, ddl_history_content,
        ddl_hotCueBankList, ddl_hotCueBankList_cue, ddl_myTag,
        ddl_myTag_content, ddl_menuItem, ddl_category, ddl_sort,
        ddl_property, ddl_recommendedLike,
    };

    for (auto* sql : ddl)
        db << sql;

    const std::array<const char*, 4> indexes{
        idx_playlist_content_playlist_id,
        idx_myTag_content_content_id,
        idx_myTag_content_myTag_id,
        idx_hotCueBankList_cue_hotCueBankList_id,
    };

    for (auto* sql : indexes)
        db << sql;
}

void insert_default_catalogues(sqlite::database& db)
{
    for (auto* sql : default_color_rows)
        db << sql;

    for (auto* sql : default_menuItem_rows)
        db << sql;

    for (auto* sql : default_category_rows)
        db << sql;

    for (auto* sql : default_sort_rows)
        db << sql;

    for (auto* sql : default_myTag_rows)
        db << sql;

    db << "INSERT INTO property VALUES "
          "(?, ?, 0, strftime('%Y-%m-%d', 'now', 'localtime'), 0, ?)"
       << "" << schema_db_version
       << djinterop::util::generate_random_int64();
}

}  // namespace djinterop::onelibrary
