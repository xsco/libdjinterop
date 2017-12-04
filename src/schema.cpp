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

struct table_info
{
    int col_id;
    std::string col_name;
    std::string col_type;
    int nullable;
    std::string default_value;
    int part_of_pk;
};

inline bool operator <(const table_info &o1, const table_info &o2)
{
    return o1.col_name < o2.col_name;
}

template<typename TableInfoIterator>
void validate(const TableInfoIterator iter, const TableInfoIterator end,
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
    std::set<table_info> cols;
    db << "PRAGMA TABLE_INFO('Track')"
       >> [&cols](int col_id, std::string col_name, std::string col_type,
               int nullable, std::string default_value, int part_of_pk)
       {
           // Note that emplace() does not support aggregate initialisation
           cols.insert(table_info{col_id, col_name, col_type, nullable,
                   default_value, part_of_pk});
       };

    auto iter = cols.begin(), end = cols.end();
    validate(iter++, end, "bitrate", "INTEGER", 0, "", 0);
    validate(iter++, end, "bpm", "INTEGER", 0, "", 0);
    validate(iter++, end, "bpmAnalyzed", "REAL", 0, "", 0);
    validate(iter++, end, "filename", "TEXT", 0, "", 0);
    validate(iter++, end, "id", "INTEGER", 0, "", 1);
    validate(iter++, end, "idAlbumArt", "INTEGER", 0, "", 0);
    validate(iter++, end, "idTrackInExternalDatabase", "INTEGER", 0, "", 0);
    validate(iter++, end, "isExternalTrack", "NUMERIC", 0, "", 0);
    validate(iter++, end, "length", "INTEGER", 0, "", 0);
    validate(iter++, end, "lengthCalculated", "INTEGER", 0, "", 0);
    validate(iter++, end, "path", "TEXT", 0, "", 0);
    validate(iter++, end, "playOrder", "INTEGER", 0, "", 0);
    validate(iter++, end, "trackType", "INTEGER", 0, "", 0);
    validate(iter++, end, "uuidOfExternalDatabase", "TEXT", 0, "", 0);
    validate(iter++, end, "year", "INTEGER", 0, "", 0);
    validate_no_more(iter, end, "table_info", "Track");

    // TODO - verify rest of the schema for Track table
    // and PRAGMA INDEX_LIST('table_name')
    // and PRAGMA INDEX_INFO('index_name')
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
