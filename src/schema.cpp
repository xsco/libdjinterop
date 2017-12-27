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

inline bool operator <(const table_info_entry &o1, const table_info_entry &o2)
{
    return o1.col_name < o2.col_name;
}

inline bool operator <(const index_list_entry &o1, const index_list_entry &o2)
{
    return o1.index_name < o2.index_name;
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

template<typename Iterator>
void validate_no_more(const Iterator &iter, const Iterator &end,
        const std::string &validation_type, const std::string &item)
{
    if (iter != end)
        throw database_inconsistency{validation_type + " for " + item +
            "has more entries than expected"};
}

static void create_track(sqlite::database &db)
{
    // TODO - create schema for the Track table, if it doesn't exist
}

static void verify_track(sqlite::database &db)
{
    table_info cols{db, "Track"};
    auto c_iter = cols.begin(), c_end = cols.end();
    validate(c_iter++, c_end, "bitrate", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "bpm", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "bpmAnalyzed", "REAL", 0, "", 0);
    validate(c_iter++, c_end, "filename", "TEXT", 0, "", 0);
    validate(c_iter++, c_end, "id", "INTEGER", 0, "", 1);
    validate(c_iter++, c_end, "idAlbumArt", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "idTrackInExternalDatabase", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "isExternalTrack", "NUMERIC", 0, "", 0);
    validate(c_iter++, c_end, "length", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "lengthCalculated", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "path", "TEXT", 0, "", 0);
    validate(c_iter++, c_end, "playOrder", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "trackType", "INTEGER", 0, "", 0);
    validate(c_iter++, c_end, "uuidOfExternalDatabase", "TEXT", 0, "", 0);
    validate(c_iter++, c_end, "year", "INTEGER", 0, "", 0);
    validate_no_more(c_iter, c_end, "table_info", "Track");

    index_list indices{db, "Track"};
    auto i_iter = indices.begin(), i_end = indices.end();
    validate(i_iter++, i_end, "index_Track_filename", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_id", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_idAlbumArt", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_idTrackInExternalDatabase", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_isExternalTrack", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_path", 0, "c", 0);
    validate(i_iter++, i_end, "index_Track_uuidOfExternalDatabase", 0, "c", 0);
    validate_no_more(i_iter, i_end, "index_list", "Track");
}

bool is_supported(const schema_version &version)
{
    return (
        version == version_firmware_1_0_0 ||
        version == version_firmware_1_0_3);
}

void verify_music_schema(sqlite::database &db)
{
    verify_track(db);
    // TODO - others
}

void verify_performance_schema(sqlite::database &db)
{
    // TODO
}

void create_music_schema(
        sqlite::database &db, const schema_version &version)
{
    // TODO - generate schema

    // Generate UUIDs for the Information table
    boost::uuids::uuid m_uuid{boost::uuids::random_generator()()};
    auto m_uuid_str = boost::uuids::to_string(m_uuid);
}

void create_performance_schema(
        sqlite::database &db, const schema_version &version)
{
    // TODO - generate schema

    // Generate UUIDs for the Information table
    boost::uuids::uuid p_uuid{boost::uuids::random_generator()()};
    auto p_uuid_str = boost::uuids::to_string(p_uuid);
}

} // engineprime
