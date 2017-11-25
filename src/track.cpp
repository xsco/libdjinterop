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

namespace engineprime {

struct track_row
{
	int play_order;
	int length;
	int length_calculated;
	int bpm;
	int year;
	std::string path;
	std::string filename;
	int bitrate;
	double bpm_analysed;
	int track_type;
	double is_external_track;
	std::string uuid_of_external_database;
	int id_track_in_external_database;
	int id_album_art;
};

struct metadata_row
{
	int id;
	int type;
	std::string value;
};

struct metadata_integer_row
{
	int id;
	int type;
	int value;
};

struct performance_data_row
{
};

// Utility method for getting std::string out of a stmt
std::string sqlite3_column_str(sqlite3_stmt *stmt, int index)
{
	auto uchar_ptr = sqlite3_column_text(stmt, index);
	if (uchar_ptr == nullptr)
		return "";
	return reinterpret_cast<const char *>(uchar_ptr);
}

// Select a row from the Track table
track_row select_track_row(const std::string &music_db_path, int id)
{
    sqlite3_db_raii m{music_db_path};

	// Read from Track
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(m.db,
		"SELECT id, playOrder, length, lengthCalculated, bpm, year, "
		    "path, filename, bitrate, bpmAnalyzed, trackType, "
			"isExternalTrack, uuidOfExternalDatabase, "
			"idTrackInExternalDatabase, idAlbumArt "
		"FROM Track WHERE id = :1",
		-1, &stmt, 0) != SQLITE_OK)
	{
       	std::string err_msg_str{sqlite3_errmsg(m.db)};
	    throw std::runtime_error{err_msg_str};
	}
	sqlite3_bind_int(stmt, 1, id);
	if (sqlite3_step(stmt) != SQLITE_ROW)
	{
        std::string err_msg_str{sqlite3_errmsg(m.db)};
	    throw std::runtime_error{err_msg_str};
	}

	// Set various fields
	track_row row{
		sqlite3_column_int(stmt, 1), // playOrder
		sqlite3_column_int(stmt, 2), // length
		sqlite3_column_int(stmt, 3), // lengthCalculated
		sqlite3_column_int(stmt, 4), // bpm
	    sqlite3_column_int(stmt, 5), // year
		sqlite3_column_str(stmt, 6), // path
		sqlite3_column_str(stmt, 7), // filename
		sqlite3_column_int(stmt, 8), // bitrate
		sqlite3_column_double(stmt, 9), // bpmAnalyzed
		sqlite3_column_int(stmt, 10), // trackType
		sqlite3_column_double(stmt, 11), // isExternalTrack
		sqlite3_column_str(stmt, 12), // uuidOfExternalDatabase
		sqlite3_column_int(stmt, 13), // idTrackInExternalDatabase
		sqlite3_column_int(stmt, 14) // idAlbumArt
	};
	sqlite3_finalize(stmt);
	return row;
}

struct track::impl
{
    impl(const database &database, int id) :
		id_{id},
		track_row_{select_track_row(database.music_db_path(), id)}
    {
        // TODO - read from Track
        // TODO - read from Metadata
        // TODO - read from MetadataInteger
        // TODO - read from PerformanceData
    }
    
    int id_;
	track_row track_row_;
    track_analysis analysis_;
    std::chrono::system_clock::time_point last_modified_at_;
    std::chrono::system_clock::time_point last_played_at_;
    std::chrono::system_clock::time_point last_loaded_at_;
};


track::track(const database &database, int id) : pimpl_{new impl{database, id}}
{}

track::~track() = default;

int track::id() const { return pimpl_->id_; }
const std::string &track::path() const { return pimpl_->track_row_.path; }
const std::string &track::filename() const { return pimpl_->track_row_.filename; }

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
