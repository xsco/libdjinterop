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

#include <engineprime/track.hpp>

#include <chrono>
#include <string>
#include "sqlite3_db_raii.hpp"
#include <iostream>

namespace engineprime {

struct track::impl
{
    impl(const database &database, int id)
    {
        id_ = id;
        
        sqlite3_db_raii db_m{database.music_db_path()};
        
        // TODO - read from Track
        // TODO - read from Metadata
        // TODO - read from MetadataInteger
        // TODO - read from PerformanceData
    }
    
    int id_;
    track_metadata metadata_;
    track_analysis analysis_;
    std::string path_;
    std::string filename_;
    std::string file_extension_;
    std::chrono::system_clock::time_point last_modified_at_;
    int bitrate_;
    bool ever_played_;
    std::chrono::system_clock::time_point last_played_at_;
    std::chrono::system_clock::time_point last_loaded_at_;
    bool is_imported_;
    std::string external_database_id_;
    int track_id_in_external_database_;
    int album_art_id_;
};


track::track(const database &database, int id) : pimpl_{new impl{database, id}}
{}

track::~track() = default;

int track::id() const
{
    return pimpl_->id_;
}


std::vector<int> all_track_ids(const database &database)
{
    sqlite3_db_raii m{database.music_db_path()};
    
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(m.db,
        "SELECT id FROM Track ORDER BY id",
        -1,
        &stmt,
        0) != SQLITE_OK)
    {
        std::string err_msg_str{sqlite3_errmsg(m.db)};
        throw std::runtime_error{err_msg_str};
    }
    
    std::vector<int> results;
    int rc;
    while (true)
    {
        rc = sqlite3_step(stmt);
        if (rc == SQLITE_DONE)
            break;
    
        if (rc != SQLITE_ROW)
        {
            std::string err_msg_str{sqlite3_errmsg(m.db)};
            throw std::runtime_error{err_msg_str};
        }
        
        results.push_back(sqlite3_column_int(stmt, 0));
    }
    
    sqlite3_finalize(stmt);
    
    return results;
}

} // namespace engineprime
