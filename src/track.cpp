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
#include <vector>
#include "sqlite_modern_cpp.h"

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

struct metadata_types
{
	static const int title          = 1;
	static const int artist         = 2;
	static const int album          = 3;
	static const int genre          = 4;
	static const int comment        = 5;
	static const int publisher      = 6;
	static const int composer       = 7;
	static const int duration_mm_ss = 10;
	static const int ever_played    = 12;
	static const int file_extension = 13;
};

struct metadata_row
{
	int id;
	int type;
	std::string text;
};

typedef std::vector<metadata_row> str_metadata_vec;

struct metadata_integer_types
{
	static const int last_played_ts      = 1;
	static const int last_modified_ts    = 2;
	static const int last_played_date_ts = 3;
	static const int musical_key         = 4;
	static const int hash                = 10;
};

struct metadata_integer_row
{
	int id;
	int type;
	int value;
};

typedef std::vector<metadata_integer_row> int_metadata_vec;


// Select a row from the Track table
track_row select_track_row(const std::string &music_db_path, int id)
{
	sqlite::database m_db{music_db_path};
	track_row row;
	int rows_found = 0;
	m_db
		<< "SELECT id, playOrder, length, lengthCalculated, bpm, year, "
		   "path, filename, bitrate, bpmAnalyzed, trackType, "
		   "isExternalTrack, uuidOfExternalDatabase, "
		   "idTrackInExternalDatabase, idAlbumArt "
		   "FROM Track WHERE id = :1"
		<< id
		>> [&](int id, int play_order, int length, int length_calculated,
			  int bpm, int year, std::string path, std::string filename,
			  int bitrate, double bpm_analysed, int track_type,
			  double is_external_track, std::string uuid_of_external_database,
			  int id_track_in_external_database, int id_album_art)
		{
			row = track_row{
				play_order, length, length_calculated, bpm, year, path,
				filename, bitrate, bpm_analysed, track_type, is_external_track,
				uuid_of_external_database, id_track_in_external_database,
				id_album_art};
			++rows_found;
		};

	if (rows_found == 0)
		throw nonexistent_track{id};

	return row;
}

str_metadata_vec select_metadata_rows(const std::string &music_db_path, int id)
{
	sqlite::database m_db{music_db_path};
	str_metadata_vec results{17};
	m_db
		<< "SELECT id, type, text FROM Metadata WHERE id = ?"
		<< id
		>> [&results](int id, int type, std::string text)
		{
			if (type > 16)
				// Some new metadata that we don't know about yet!
				return;

			results[type].id = id;
			results[type].type = type;
			results[type].text = text;
		};

	return results;
}

int_metadata_vec select_int_metadata_rows(
		const std::string &music_db_path, int id)
{
	sqlite::database m_db{music_db_path};
	int_metadata_vec results{12};
	m_db
		<< "SELECT id, type, value FROM MetadataInteger WHERE id = ?"
		<< id
		>> [&results](int id, int type, int value)
		{
			if (type > 11)
				// Some new metadata that we don't know about yet!
				return;

			results[type].id    = id;
			results[type].type  = type;
			results[type].value = value;
		};

	return results;
}

struct track::impl
{
    impl(const database &db, int id) :
		id_{id},
		track_row_{select_track_row(db.music_db_path(), id)},
		str_metadata_vec_{select_metadata_rows(db.music_db_path(), id)},
		int_metadata_vec_{select_int_metadata_rows(db.music_db_path(), id)},
		duration_{track_row_.length},
		last_modified_at_{std::chrono::seconds{
			int_metadata_vec_[metadata_integer_types::last_modified_ts].value}},
		last_played_at_{std::chrono::seconds{
			int_metadata_vec_[metadata_integer_types::last_played_ts].value}},
		last_loaded_at_{std::chrono::seconds{
			int_metadata_vec_[metadata_integer_types::last_played_date_ts].value}}
    {}
    
    int id_;
	track_row track_row_;
	str_metadata_vec str_metadata_vec_;
	int_metadata_vec int_metadata_vec_;

	std::chrono::seconds duration_;
    std::chrono::system_clock::time_point last_modified_at_;
    std::chrono::system_clock::time_point last_played_at_;
    std::chrono::system_clock::time_point last_loaded_at_;
};


track::track(const database &database, int id) : pimpl_{new impl{database, id}}
{}

track::~track() = default;

int track::id() const { return pimpl_->id_; }
int track::track_number() const { return pimpl_->track_row_.play_order; }
std::chrono::seconds track::duration() const { return pimpl_->duration_; }
int track::bpm() const { return pimpl_->track_row_.bpm; }
int track::year() const { return pimpl_->track_row_.year; }
const std::string &track::title() const { return pimpl_->str_metadata_vec_[metadata_types::title].text; }
const std::string &track::artist() const { return pimpl_->str_metadata_vec_[metadata_types::artist].text; }
const std::string &track::album() const { return pimpl_->str_metadata_vec_[metadata_types::album].text; }
const std::string &track::genre() const { return pimpl_->str_metadata_vec_[metadata_types::genre].text; }
const std::string &track::comment() const { return pimpl_->str_metadata_vec_[metadata_types::comment].text; }
const std::string &track::publisher() const { return pimpl_->str_metadata_vec_[metadata_types::publisher].text; }
const std::string &track::composer() const { return pimpl_->str_metadata_vec_[metadata_types::composer].text; }
const std::string &track::path() const { return pimpl_->track_row_.path; }
const std::string &track::filename() const { return pimpl_->track_row_.filename; }
const std::string &track::file_extension() const { return pimpl_->str_metadata_vec_[metadata_types::file_extension].text; }
const std::chrono::system_clock::time_point track::last_modified_at() const { return pimpl_->last_modified_at_; }
int track::bitrate() const { return pimpl_->track_row_.bitrate; }
bool track::ever_played() const { return pimpl_->str_metadata_vec_[metadata_types::ever_played].text == "1"; }
const std::chrono::system_clock::time_point track::last_played_at() const { return pimpl_->last_played_at_; }
const std::chrono::system_clock::time_point track::last_loaded_at() const { return pimpl_->last_loaded_at_; }
bool track::is_imported() const { return pimpl_->track_row_.is_external_track != 0; }
const std::string &track::external_database_id() const { return pimpl_->track_row_.uuid_of_external_database; }
int track::track_id_in_external_database() const { return pimpl_->track_row_.id_track_in_external_database; }
int track::album_art_id() const { return pimpl_->track_row_.id_album_art; }

std::vector<int> all_track_ids(const database &database)
{
	sqlite::database m_db{database.music_db_path()};

	std::vector<int> ids;
	m_db
		<< "SELECT id FROM Track ORDER BY id"
		>> [&ids](int id) { ids.push_back(id); };

	return ids;
}

} // namespace engineprime
