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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_TRACK_HPP
#define ENGINEPRIME_TRACK_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "database.hpp"

namespace engineprime {

class nonexistent_track : public std::invalid_argument
{
public:
	explicit nonexistent_track(int id) noexcept :
		invalid_argument{"Track does not exist in database"},
		id_{id}
	{}
	virtual ~nonexistent_track() = default;
	int id() const noexcept { return id_; }
private:
	int id_;
};

class track_database_inconsistency : public database_inconsistency
{
public:
	explicit track_database_inconsistency(const std::string &what_arg)
		noexcept :
		database_inconsistency{what_arg}
	{}
	explicit track_database_inconsistency(const std::string &what_arg, int id)
		noexcept :
		database_inconsistency{what_arg},
		id_{id}
	{}
	virtual ~track_database_inconsistency() = default;
	int id() const noexcept { return id_; }
private:
	int id_;
};

class track
{
public:
    track(const database &database, int id);
    ~track();
    
    int id() const;
    int track_number() const;
    std::chrono::seconds duration() const;
    int bpm() const;
    int year() const;
    const std::string &title() const;
    const std::string &artist() const;
    const std::string &album() const;
    const std::string &genre() const;
    const std::string &comment() const;
    const std::string &publisher() const;
    const std::string &composer() const;
	const std::string &path() const;
	const std::string &filename() const;
	const std::string &file_extension() const;
    const std::chrono::system_clock::time_point last_modified_at() const;
	int bitrate() const;
	bool ever_played() const;
	const std::chrono::system_clock::time_point last_played_at() const;
	const std::chrono::system_clock::time_point last_loaded_at() const;
	bool is_imported() const;
	const std::string &external_database_id() const;
	int track_id_in_external_database() const;
	int album_art_id() const;

	bool has_album_art() const { return album_art_id() != 1; }
    
private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

std::vector<int> all_track_ids(const database &database);

} // engineprime

#endif // ENGINEPRIME_TRACK_HPP
