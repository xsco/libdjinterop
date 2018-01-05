/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "schema.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <string>
#include <set>
#include "sqlite_modern_cpp.h"

namespace engineprime {

struct table_info_entry
{
    int col_id;
    std::string col_name;
    std::string col_type;
    int nullable;
    std::string default_value;
    int part_of_pk;
};

struct index_list_entry
{
    int index_id;
    std::string index_name;
    int unique;
    std::string creation_method;
    int partial_index;
};

struct index_info_entry
{
    int col_index_id;
    int col_table_id;
    std::string col_name;
};

inline bool operator <(const table_info_entry &o1, const table_info_entry &o2)
{
    return o1.col_name < o2.col_name;
}

inline bool operator <(const index_list_entry &o1, const index_list_entry &o2)
{
    return o1.index_name < o2.index_name;
}

inline bool operator <(const index_info_entry &o1, const index_info_entry &o2)
{
    return o1.col_index_id < o2.col_index_id;
}

struct table_info
{
    typedef std::set<table_info_entry>::iterator iterator;
    typedef std::set<table_info_entry>::const_iterator const_iterator;

    table_info(sqlite::database &db, const std::string &table_name)
    {
        db << "PRAGMA TABLE_INFO('" + table_name + "')"
           >> [this](int col_id, std::string col_name, std::string col_type,
                   int nullable, std::string default_value, int part_of_pk)
           {
               // Note that emplace() does not support aggregate initialisation
               cols_.insert(table_info_entry{col_id, col_name, col_type, nullable,
                            default_value, part_of_pk});
           };
    }

    iterator begin() { return cols_.begin(); }
    const_iterator begin() const noexcept { return cols_.cbegin(); }
    iterator end() { return cols_.end(); }
    const_iterator end() const noexcept { return cols_.cend(); }

private:
    std::set<table_info_entry> cols_;
};

struct index_list
{
    typedef std::set<index_list_entry>::iterator iterator;
    typedef std::set<index_list_entry>::const_iterator const_iterator;

    index_list(sqlite::database &db, const std::string &table_name)
    {
        db << "PRAGMA INDEX_LIST('" + table_name + "')"
           >> [this](int index_id, std::string index_name, int unique,
                     std::string creation_method, int partial_index)
           {
               // Note that emplace() does not support aggregate initialisation
           indices_.insert(index_list_entry{index_id, index_name, unique,
                                            creation_method, partial_index});
           };
    }

    iterator begin() { return indices_.begin(); }
    const_iterator begin() const noexcept { return indices_.cbegin(); }
    iterator end() { return indices_.end(); }
    const_iterator end() const noexcept { return indices_.cend(); }

private:
    std::set<index_list_entry> indices_;
};

struct index_info
{
    typedef std::set<index_info_entry>::iterator iterator;
    typedef std::set<index_info_entry>::const_iterator const_iterator;

    index_info(sqlite::database &db, const std::string &index_name)
    {
        db << "PRAGMA INDEX_INFO('" + index_name + "')"
           >> [this](int col_index_id, int col_table_id, std::string col_name)
            {
                // Note that emplace() does not support aggregate initialisation
                cols_.insert(index_info_entry{
                        col_index_id, col_table_id, col_name});
            };
    }

    iterator begin() { return cols_.begin(); }
    const_iterator begin() const noexcept { return cols_.cbegin(); }
    iterator end() { return cols_.end(); }
    const_iterator end() const noexcept { return cols_.cend(); }

private:
    std::set<index_info_entry> cols_;
};

void validate(table_info::const_iterator iter, table_info::const_iterator end,
        const std::string &col_name, const std::string &col_type,
        int nullable, const std::string &default_value, int part_of_pk)
{
    if (iter == end)
        throw database_inconsistency{"Column " + col_name + " missing"};
    if (iter->col_name != col_name)
        throw database_inconsistency{"Column " + iter->col_name +
            " in wrong order, expected " + col_name};
    if (iter->col_type != col_type)
        throw database_inconsistency{"Column " + col_name +
            " has wrong type: " + iter->col_type};
    if (iter->nullable != nullable)
        throw database_inconsistency{"Column " + col_name +
            " has wrong nullability: " + std::to_string(iter->nullable)};
    if (iter->default_value != default_value)
        throw database_inconsistency{"Column " + col_name +
            " has wrong default value: \"" + iter->default_value + "\""};
    if (iter->part_of_pk != part_of_pk)
        throw database_inconsistency{"Column " + col_name +
            " has wrong PK membership: " + std::to_string(iter->part_of_pk)};
}

void validate(index_list::const_iterator iter, index_list::const_iterator end,
        const std::string &index_name, int unique,
        const std::string &creation_method, int partial_index)
{
    if (iter == end)
        throw database_inconsistency{"Index " + index_name + " missing"};
    if (iter->index_name != index_name)
        throw database_inconsistency{"Index " + iter->index_name +
            " in wrong order, expected " + index_name};
    if (iter->unique != unique)
        throw database_inconsistency{"Index " + index_name +
            " has wrong uniqueness: " + std::to_string(iter->unique)};
    if (iter->creation_method != creation_method)
        throw database_inconsistency{"Index " + index_name +
            " has wrong creation method: \"" + iter->creation_method + "\""};
    if (iter->partial_index != partial_index)
        throw database_inconsistency{"Index " + index_name +
            " has wrong \"partiality\": " + std::to_string(iter->partial_index)};
}

void validate(index_info::const_iterator iter, index_info::const_iterator end,
        int col_index_id, const std::string &col_name)
{
    if (iter == end)
        throw database_inconsistency{"Col " + col_name + " missing from index"};
    if (iter->col_index_id != col_index_id)
        throw database_inconsistency{"Col " + col_name +
            " has wrong rank within the index: " + std::to_string(iter->col_index_id)};
    if (iter->col_name != col_name)
        throw database_inconsistency{"Col " + iter->col_name +
            " in wrong order, expected " + col_name};
}

template<typename Iterator>
void validate_no_more(const Iterator &iter, const Iterator &end,
        const std::string &validation_type, const std::string &item)
{
    if (iter != end)
        throw database_inconsistency{validation_type + " for " + item +
            " has more entries than expected"};
}

static void verify_album_art(sqlite::database &db)
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
        validate_no_more(iter, end, "table_info", "AlbumArt");
    }
    {
        index_list indices{db, "AlbumArt"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_AlbumArt_hash", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_AlbumArt_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "AlbumArt");
    }
    {
        index_info ii{db, "index_AlbumArt_hash"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "hash");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_AlbumArt_hash");
    }
    {
        index_info ii{db, "index_AlbumArt_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_AlbumArt_id");
    }
}

static void verify_copied_track(sqlite::database &db)
{
    {
        table_info cols{db, "CopiedTrack"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "idOfTrackInSourceDatabase", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "uuidOfSourceDatabase", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "CopiedTrack");
    }
    {
        index_list indices{db, "CopiedTrack"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_CopiedTrack_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "CopiedTrack");
    }
    {
        index_info ii{db, "index_CopiedTrack_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CopiedTrack_trackId");
    }
}

static void verify_crate(sqlite::database &db)
{
    {
        table_info cols{db, "Crate"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "path", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Crate");
    }
    {
        index_list indices{db, "Crate"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Crate_id", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Crate_path", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_Crate_title", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Crate");
    }
    {
        index_info ii{db, "index_Crate_path"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "path");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Crate_path");
    }
    {
        index_info ii{db, "index_Crate_title"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "title");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Crate_title");
    }
    {
        index_info ii{db, "index_Crate_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Crate_id");
    }
}

static void verify_crate_hierarchy(sqlite::database &db)
{
    {
        table_info cols{db, "CrateHierarchy"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "crateId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "crateIdChild", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "CrateHierarchy");
    }
    {
        index_list indices{db, "CrateHierarchy"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_CrateHierarchy_crateId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_CrateHierarchy_crateIdChild", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "CrateHierarchy");
    }
    {
        index_info ii{db, "index_CrateHierarchy_crateId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "crateId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateHierarchy_crateId");
    }
    {
        index_info ii{db, "index_CrateHierarchy_crateIdChild"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "crateIdChild");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateHierarchy_crateIdChild");
    }
}

static void verify_crate_parent_list(sqlite::database &db)
{
    {
        table_info cols{db, "CrateParentList"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "crateOriginId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "crateParentId", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "CrateParentList");
    }
    {
        index_list indices{db, "CrateParentList"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_CrateParentList_crateOriginId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_CrateParentList_crateParentId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "CrateParentList");
    }
    {
        index_info ii{db, "index_CrateParentList_crateOriginId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "crateOriginId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateParentList_crateOriginId");
    }
    {
        index_info ii{db, "index_CrateParentList_crateParentId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "crateParentId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateParentList_crateParentId");
    }
}

static void verify_crate_track_list(sqlite::database &db)
{
    {
        table_info cols{db, "CrateTrackList"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "crateId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "CrateTrackList");
    }
    {
        index_list indices{db, "CrateTrackList"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_CrateTrackList_crateId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_CrateTrackList_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "CrateTrackList");
    }
    {
        index_info ii{db, "index_CrateTrackList_crateId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "crateId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateTrackList_crateId");
    }
    {
        index_info ii{db, "index_CrateTrackList_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_CrateTrackList_trackId");
    }
}

static void verify_historylist(sqlite::database &db)
{
    {
        table_info cols{db, "Historylist"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Historylist");
    }
    {
        index_list indices{db, "Historylist"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Historylist_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Historylist");
    }
    {
        index_info ii{db, "index_Historylist_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Historylist");
    }
}

static void verify_historylist_track_list(sqlite::database &db)
{
    {
        table_info cols{db, "HistorylistTrackList"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "databaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "date", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "historylistId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackIdInOriginDatabase", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "HistorylistTrackList");
    }
    {
        index_list indices{db, "HistorylistTrackList"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_HistorylistTrackList_date", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_HistorylistTrackList_historylistId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_HistorylistTrackList_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "HistorylistTrackList");
    }
    {
        index_info ii{db, "index_HistorylistTrackList_date"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "date");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_HistorylistTrackList_date");
    }
    {
        index_info ii{db, "index_HistorylistTrackList_historylistId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "historylistId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_HistorylistTrackList_historylistId");
    }
    {
        index_info ii{db, "index_HistorylistTrackList_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_HistorylistTrackList_trackId");
    }
}

static void verify_information(sqlite::database &db)
{
    {
        table_info cols{db, "Information"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "currentPlayedIndiciator", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "id", "INTEGER", 0, "", 1);
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
        index_list indices{db, "Information"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Information_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Information");
    }
    {
        index_info ii{db, "index_Information_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Information_id");
    }
}

static void verify_metadata(sqlite::database &db)
{
    {
        table_info cols{db, "MetaData"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "text", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "type", "INTEGER", 0, "", 2);
        ++iter;
        validate_no_more(iter, end, "table_info", "MetaData");
    }
    {
        index_list indices{db, "MetaData"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_MetaData_id", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_MetaData_text", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_MetaData_type", 0, "c", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_MetaData_1", 1, "pk", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "MetaData");
    }
    {
        index_info ii{db, "index_MetaData_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaData_id");
    }
    {
        index_info ii{db, "index_MetaData_text"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "text");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaData_text");
    }
    {
        index_info ii{db, "index_MetaData_type"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "type");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaData_type");
    }
    {
        index_info ii{db, "sqlite_autoindex_MetaData_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate(iter, end, 1, "type");
        ++iter;
        validate_no_more(iter, end, "index_info", "sqlite_autoindex_MetaData_1");
    }
}

static void verify_metadata_integer(sqlite::database &db)
{
    {
        table_info cols{db, "MetaDataInteger"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "type", "INTEGER", 0, "", 2);
        ++iter;
        validate(iter, end, "value", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "MetaDataInteger");
    }
    {
        index_list indices{db, "MetaDataInteger"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_MetaDataInteger_id", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_MetaDataInteger_type", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_MetaDataInteger_value", 0, "c", 0);
        ++iter;
        validate(iter, end, "sqlite_autoindex_MetaDataInteger_1", 1, "pk", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "MetaDataInteger");
    }
    {
        index_info ii{db, "index_MetaDataInteger_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaDataInteger_id");
    }
    {
        index_info ii{db, "index_MetaDataInteger_type"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "type");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaDataInteger_type");
    }
    {
        index_info ii{db, "index_MetaDataInteger_value"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "value");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_MetaDataInteger_value");
    }
    {
        index_info ii{db, "sqlite_autoindex_MetaDataInteger_1"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate(iter, end, 1, "type");
        ++iter;
        validate_no_more(iter, end, "index_info", "sqlite_autoindex_MetaDataInteger_1");
    }
}

static void verify_playlist(sqlite::database &db)
{
    {
        table_info cols{db, "Playlist"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Playlist");
    }
    {
        index_list indices{db, "Playlist"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Playlist_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Playlist");
    }
    {
        index_info ii{db, "index_Playlist_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Playlist_id");
    }
}

static void verify_playlist_track_list(sqlite::database &db)
{
    {
        table_info cols{db, "PlaylistTrackList"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "databaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "playlistId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackIdInOriginDatabase", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackNumber", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "PlaylistTrackList");
    }
    {
        index_list indices{db, "PlaylistTrackList"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_PlaylistTrackList_playlistId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_PlaylistTrackList_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "PlaylistTrackList");
    }
    {
        index_info ii{db, "index_PlaylistTrackList_playlistId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "playlistId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PlaylistTrackList_playlistId");
    }
    {
        index_info ii{db, "index_PlaylistTrackList_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PlaylistTrackList_trackId");
    }
}

static void verify_preparelist(sqlite::database &db)
{
    {
        table_info cols{db, "Preparelist"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "id", "INTEGER", 0, "", 1);
        ++iter;
        validate(iter, end, "title", "TEXT", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "Preparelist");
    }
    {
        index_list indices{db, "Preparelist"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_Preparelist_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "Preparelist");
    }
    {
        index_info ii{db, "index_Preparelist_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Preparelist_id");
    }
}

static void verify_preparelist_track_list(sqlite::database &db)
{
    {
        table_info cols{db, "PreparelistTrackList"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "databaseUuid", "TEXT", 0, "", 0);
        ++iter;
        validate(iter, end, "playlistId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackId", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackIdInOriginDatabase", "INTEGER", 0, "", 0);
        ++iter;
        validate(iter, end, "trackNumber", "INTEGER", 0, "", 0);
        ++iter;
        validate_no_more(iter, end, "table_info", "PreparelistTrackList");
    }
    {
        index_list indices{db, "PreparelistTrackList"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_PreparelistTrackList_playlistId", 0, "c", 0);
        ++iter;
        validate(iter, end, "index_PreparelistTrackList_trackId", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "PreparelistTrackList");
    }
    {
        index_info ii{db, "index_PreparelistTrackList_playlistId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "playlistId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PreparelistTrackList_playlistId");
    }
    {
        index_info ii{db, "index_PreparelistTrackList_trackId"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "trackId");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PreparelistTrackList_trackId");
    }
}

static void verify_track(sqlite::database &db)
{
    {
        table_info cols{db, "Track"};
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
        index_list indices{db, "Track"};
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
        index_info ii{db, "index_Track_filename"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "filename");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_filename");
    }
    {
        index_info ii{db, "index_Track_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_id");
    }
    {
        index_info ii{db, "index_Track_idAlbumArt"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "idAlbumArt");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_idAlbumArt");
    }
    {
        index_info ii{db, "index_Track_idTrackInExternalDatabase"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "idTrackInExternalDatabase");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_idTrackInExternalDatabase");
    }
    {
        index_info ii{db, "index_Track_isExternalTrack"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "isExternalTrack");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_isExternalTrack");
    }
    {
        index_info ii{db, "index_Track_path"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "path");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_path");
    }
    {
        index_info ii{db, "index_Track_uuidOfExternalDatabase"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "uuidOfExternalDatabase");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_Track_uuidOfExternalDatabase");
    }
}

static void verify_performance_data(sqlite::database &db)
{
    {
        table_info cols{db, "PerformanceData"};
        auto iter = cols.begin(), end = cols.end();
        validate(iter, end, "beatData", "BLOB", 0, "", 0);
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
        index_list indices{db, "PerformanceData"};
        auto iter = indices.begin(), end = indices.end();
        validate(iter, end, "index_PerformanceData_id", 0, "c", 0);
        ++iter;
        validate_no_more(iter, end, "index_list", "PerformanceData");
    }
    {
        index_info ii{db, "index_PerformanceData_id"};
        auto iter = ii.begin(), end = ii.end();
        validate(iter, end, 0, "id");
        ++iter;
        validate_no_more(iter, end, "index_info", "index_PerformanceData_id");
    }
}

static schema_version get_version(sqlite::database &db)
{
    schema_version version;
    db
        << "SELECT schemaVersionMajor, schemaVersionMinor, schemaVersionPatch "
           "FROM Information"
        >> std::tie(version.maj, version.min, version.pat);
    return version;
}

bool is_supported(const schema_version &version)
{
    return (
        version == version_firmware_1_0_0 ||
        version == version_firmware_1_0_3);
}

schema_version verify_music_schema(sqlite::database &db)
{
    verify_information(db);
    auto version = get_version(db);

    // Note: the version can be used to verify schema differently, should it
    // ever change in future.
    verify_album_art(db);
    verify_copied_track(db);
    verify_crate(db);
    verify_crate_hierarchy(db);
    verify_crate_parent_list(db);
    verify_crate_track_list(db);
    verify_historylist(db);
    verify_historylist_track_list(db);
    verify_metadata(db);
    verify_metadata_integer(db);
    verify_playlist(db);
    verify_playlist_track_list(db);
    verify_preparelist(db);
    verify_preparelist_track_list(db);
    verify_track(db);

    return version;
}

schema_version verify_performance_schema(sqlite::database &db)
{
    verify_information(db);
    auto version = get_version(db);

    // Note: the version can be used to verify schema differently, should it
    // ever change in future.
    verify_performance_data(db);

    return version;
}

void create_music_schema(
        sqlite::database &db, const schema_version &version)
{
    // Information
    db << "DROP TABLE IF EXISTS Information";
    db << "CREATE TABLE Information ( "
          "[id] INTEGER, [uuid] TEXT , [schemaVersionMajor] INTEGER , "
          "[schemaVersionMinor] INTEGER , [schemaVersionPatch] INTEGER , "
          "[currentPlayedIndiciator] INTEGER , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Information_id ON Information ( id )";

    // AlbumArt
    db << "CREATE TABLE AlbumArt ( [id] INTEGER, [hash] TEXT , "
          "[albumArt] BLOB , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_AlbumArt_id ON AlbumArt ( id )";
    db << "CREATE INDEX index_AlbumArt_hash ON AlbumArt ( hash )";

    // CopiedTrack
    db << "CREATE TABLE CopiedTrack ( [trackId] INTEGER  "
          "REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[uuidOfSourceDatabase] TEXT , [idOfTrackInSourceDatabase] INTEGER , "
          "PRIMARY KEY ( [trackId] ) )";
    db << "CREATE INDEX index_CopiedTrack_trackId ON CopiedTrack ( trackId )";

    // Crate
    db << "CREATE TABLE Crate ( [id] INTEGER, [title] TEXT , [path] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Crate_id ON Crate ( id )";
    db << "CREATE INDEX index_Crate_title ON Crate ( title )";
    db << "CREATE INDEX index_Crate_path ON Crate ( path )";
    
    // CrateHierarchy
    db << "CREATE TABLE CrateHierarchy ( [crateId] INTEGER  "
          "REFERENCES Crate ( id )  ON DELETE CASCADE, [crateIdChild] INTEGER  "
          "REFERENCES Crate ( id )  ON DELETE CASCADE)";
    db << "CREATE INDEX index_CrateHierarchy_crateId ON "
          "CrateHierarchy ( crateId )";
    db << "CREATE INDEX index_CrateHierarchy_crateIdChild ON "
          "CrateHierarchy ( crateIdChild )";

    // CrateParentList
    db << "CREATE TABLE CrateParentList ( [crateOriginId] INTEGER  "
          "REFERENCES Crate ( id )  ON DELETE CASCADE, "
          "[crateParentId] INTEGER  REFERENCES Crate ( id )  "
          "ON DELETE CASCADE)";
    db << "CREATE INDEX index_CrateParentList_crateOriginId ON "
          "CrateParentList ( crateOriginId )";
    db << "CREATE INDEX index_CrateParentList_crateParentId ON "
          "CrateParentList ( crateParentId )";

    // CrateTrackList
    db << "CREATE TABLE CrateTrackList ( [crateId] INTEGER  "
          "REFERENCES Crate ( id )  ON DELETE CASCADE, [trackId] INTEGER  "
          "REFERENCES Track ( id )  ON DELETE CASCADE)";
    db << "CREATE INDEX index_CrateTrackList_crateId ON "
          "CrateTrackList ( crateId )";
    db << "CREATE INDEX index_CrateTrackList_trackId ON "
          "CrateTrackList ( trackId )";

    // Historylist
    db << "CREATE TABLE Historylist ( [id] INTEGER, [title] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Historylist_id ON Historylist ( id )";

    // HistorylistTrackList
    db << "CREATE TABLE HistorylistTrackList ( [historylistId] INTEGER  "
          "REFERENCES Historylist ( id )  ON DELETE CASCADE, "
          "[trackId] INTEGER  REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , "
          "[date] INTEGER )";
    db << "CREATE INDEX index_HistorylistTrackList_historylistId ON "
          "HistorylistTrackList ( historylistId )";
    db << "CREATE INDEX index_HistorylistTrackList_trackId ON "
          "HistorylistTrackList ( trackId )";
    db << "CREATE INDEX index_HistorylistTrackList_date ON "
          "HistorylistTrackList ( date )";

    // Metadata
    db << "CREATE TABLE MetaData ( [id] INTEGER  REFERENCES Track ( id )  "
          "ON DELETE CASCADE, [type] INTEGER, [text] TEXT , "
          "PRIMARY KEY ( [id], [type] ) )";
    db << "CREATE INDEX index_MetaData_id ON MetaData ( id )";
    db << "CREATE INDEX index_MetaData_type ON MetaData ( type )";
    db << "CREATE INDEX index_MetaData_text ON MetaData ( text )";

    // MetadataInteger
    db << "CREATE TABLE MetaDataInteger ( [id] INTEGER  "
          "REFERENCES Track ( id )  ON DELETE CASCADE, [type] INTEGER, "
          "[value] INTEGER , PRIMARY KEY ( [id], [type] ) )";
    db << "CREATE INDEX index_MetaDataInteger_id ON MetaDataInteger ( id )";
    db << "CREATE INDEX index_MetaDataInteger_type ON MetaDataInteger ( type )";
    db << "CREATE INDEX index_MetaDataInteger_value ON MetaDataInteger ( value )";

    // Playlist
    db << "CREATE TABLE Playlist ( [id] INTEGER, [title] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Playlist_id ON Playlist ( id )";

    // PlaylistTrackList
    db << "CREATE TABLE PlaylistTrackList ( [playlistId] INTEGER  "
          "REFERENCES Playlist ( id )  ON DELETE CASCADE, [trackId] INTEGER  "
          "REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , "
          "[trackNumber] INTEGER )";
    db << "CREATE INDEX index_PlaylistTrackList_playlistId ON "
          "PlaylistTrackList ( playlistId )";
    db << "CREATE INDEX index_PlaylistTrackList_trackId ON "
          "PlaylistTrackList ( trackId )";

    // Preparelist
    db << "CREATE TABLE Preparelist ( [id] INTEGER, [title] TEXT , "
          "PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Preparelist_id ON Preparelist ( id )";

    // PreparelistTrackList
    db << "CREATE TABLE PreparelistTrackList ( [playlistId] INTEGER  "
          "REFERENCES Preparelist ( id )  ON DELETE CASCADE, "
          "[trackId] INTEGER  REFERENCES Track ( id )  ON DELETE CASCADE, "
          "[trackIdInOriginDatabase] INTEGER , [databaseUuid] TEXT , "
          "[trackNumber] INTEGER )";
    db << "CREATE INDEX index_PreparelistTrackList_playlistId ON "
          "PreparelistTrackList ( playlistId )";
    db << "CREATE INDEX index_PreparelistTrackList_trackId ON "
          "PreparelistTrackList ( trackId )";

    // Track
    db << "CREATE TABLE Track ( [id] INTEGER, [playOrder] INTEGER , "
          "[length] INTEGER , [lengthCalculated] INTEGER , [bpm] INTEGER , "
          "[year] INTEGER , [path] TEXT , [filename] TEXT , "
          "[bitrate] INTEGER , [bpmAnalyzed] REAL , [trackType] INTEGER , "
          "[isExternalTrack] NUMERIC , [uuidOfExternalDatabase] TEXT , "
          "[idTrackInExternalDatabase] INTEGER , [idAlbumArt] INTEGER  "
          "REFERENCES AlbumArt ( id )  ON DELETE RESTRICT, PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Track_id ON Track ( id )";
    db << "CREATE INDEX index_Track_path ON Track ( path )";
    db << "CREATE INDEX index_Track_filename ON Track ( filename )";
    db << "CREATE INDEX index_Track_isExternalTrack ON Track ( isExternalTrack )";
    db << "CREATE INDEX index_Track_uuidOfExternalDatabase ON "
          "Track ( uuidOfExternalDatabase )";
    db << "CREATE INDEX index_Track_idTrackInExternalDatabase ON "
          "Track ( idTrackInExternalDatabase )";
    db << "CREATE INDEX index_Track_idAlbumArt ON Track ( idAlbumArt )";

    // Generate UUIDs for the Information table
    boost::uuids::uuid uuid{boost::uuids::random_generator()()};
    auto uuid_str = boost::uuids::to_string(uuid);

    // Insert row into Information
    db << "INSERT INTO Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator]) VALUES (?, ?, ?, ?, ?)"
       << uuid_str
       << version.maj
       << version.min
       << version.pat
       << 0;
}

void create_performance_schema(
        sqlite::database &db, const schema_version &version)
{
    // Information
    db << "DROP TABLE IF EXISTS Information";
    db << "CREATE TABLE Information ( "
          "[id] INTEGER, [uuid] TEXT , [schemaVersionMajor] INTEGER , "
          "[schemaVersionMinor] INTEGER , [schemaVersionPatch] INTEGER , "
          "[currentPlayedIndiciator] INTEGER , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_Information_id ON Information ( id )";

    // PerformanceData
    db << "DROP TABLE IF EXISTS PerformanceData";
    db << "CREATE TABLE PerformanceData ( [id] INTEGER, [isAnalyzed] NUMERIC , "
          "[isRendered] NUMERIC , [trackData] BLOB , "
          "[highResolutionWaveFormData] BLOB , [overviewWaveFormData] BLOB , "
          "[beatData] BLOB , [quickCues] BLOB , [loops] BLOB , "
          "[hasSeratoValues] NUMERIC , PRIMARY KEY ( [id] ) )";
    db << "CREATE INDEX index_PerformanceData_id ON PerformanceData ( id )";

    // Generate UUIDs for the Information table
    boost::uuids::uuid uuid{boost::uuids::random_generator()()};
    auto uuid_str = boost::uuids::to_string(uuid);

    // Insert row into Information
    db << "INSERT INTO Information ([uuid], [schemaVersionMajor], "
          "[schemaVersionMinor], [schemaVersionPatch], "
          "[currentPlayedIndiciator]) VALUES (?, ?, ?, ?, ?)"
       << uuid_str
       << version.maj
       << version.min
       << version.pat
       << 0;
}

} // engineprime
