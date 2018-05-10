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

/**
 * The `nonexistent_track` exception is thrown when a request is made for a
 * track using an identifier that does not exist in a given database
 */
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

/**
 * The `track` class represents a record of a given track in the database
 */
class track
{
public:
    /**
     * \brief Construct a new track, not yet saved in any database
     */
    track();

    /**
     * \brief Copy a track from another
     *
     * Note that the new copied track will not be considered to belong in any
     * database, and hence will have its `id` set to zero.
     */
    track(const track &other);

    /**
     * \brief Construct a track, loaded from a given database by its id
     */
    track(const database &database, int id);

    ~track();

    /**
     * \brief Copy from another track into this track
     *
     * Note that this track will not be considered to belong in any database
     * after copy assignment, and hence will have its `id` set to zero.
     */
    track &operator =(const track &other);

    /**
     * \brief Get the id of this track, as stored in the database.
     *
     * If the track is not yet stored in any database, the id will be zero.
     */
    int id() const;

    /**
     * \brief Get the track number
     */
    int track_number() const;

    /**
     * \brief Get the duration of this track
     *
     * Note that this field is just metadata, and does not strictly have to be
     * the track's actual duration.
     */
    std::chrono::seconds duration() const;

    /**
     * \brief Get the BPM of this track, rounded to the nearest integer
     *
     * Note that this field is just metadata, and does not strictly have to be
     * the track's actual tempo.
     */
    int bpm() const;

    /**
     * \brief Get the year that this track was recorded
     */
    int year() const;

    /**
     * \brief Get the title of this track
     */
    std::string title() const;

    /**
     * \brief Get the artist of this track
     */
    std::string artist() const;

    /**
     * \brief Get the name of the album of this track
     */
    std::string album() const;

    /**
     * \brief Get the genre of this track
     */
    std::string genre() const;

    /**
     * \brief Get the comment associated with this track
     */
    std::string comment() const;

    /**
     * \brief Get the publisher associated with this track
     */
    std::string publisher() const;
    
    /**
     * \brief Get the composer of this track
     */
    std::string composer() const;

    /**
     * \brief Get the path to this track's file on disk, relative to the music
     *        database.
     */
	std::string path() const;

    /**
     * \brief Get the filename of this track's file
     */
	std::string filename() const;

    /**
     * \brief Get the file extension of this track's file
     *
     * The extension will not contain any leading dot.
     */
	std::string file_extension() const;

    /**
     * \brief Get the time of last modification of this track's file
     */
    std::chrono::system_clock::time_point last_modified_at() const;

    /**
     * \brief Get the bitrate of this track
     */
	int bitrate() const;

    /**
     * \brief Return a `bool` indicating whether this track has ever been played
     */
	bool ever_played() const;

    /**
     * \brief Get the time at which this track was last played.
     */
	std::chrono::system_clock::time_point last_played_at() const;

    /**
     * \brief Get the time at which this track was last loaded into any player
     */
	std::chrono::system_clock::time_point last_loaded_at() const;

    /**
     * \brief Get a `bool` indicating whether this track was imported into the
     *        current database from another Engine Prime library
     */
	bool imported() const;

    /**
     * \brief Get the UUID of the external database from this track was
     *        imported, if it was imported
     */
	std::string external_database_uuid() const;

    /**
     * \brief Get the ID of this track in the external database from which it
     *        was imported, if it was imported
     */
	int track_id_in_external_database() const;

    /**
     * \brief Get the id of the album art for this track
     */
	int album_art_id() const;

    // TODO - introduce "bool has_album() const" etc. methods for anything that comes from MetaData(Integer)

	bool has_album_art() const { return album_art_id() != 1; }

    void set_track_number(int track_number);
    void set_duration(std::chrono::seconds duration);
    void set_bpm(int bpm);
    void set_year(int year);
    void set_title(const std::string &title);
    void set_artist(const std::string &artist);
    void set_album(const std::string &album);
    void set_genre(const std::string &genre);
    void set_comment(const std::string &comment);
    void set_publisher(const std::string &publisher);
    void set_composer(const std::string &composer);
    void set_path(const std::string &path);
    void set_filename(const std::string &filename);
    void set_file_extension(const std::string &file_extension);
    void set_last_modified_at(std::chrono::system_clock::time_point last_modified_at);
    void set_bitrate(int bitrate);
    void set_ever_played(bool ever_played);
    void set_last_played_at(std::chrono::system_clock::time_point last_played_at);
    void set_last_loaded_at(std::chrono::system_clock::time_point last_loaded_at);

    /**
     * \brief Set whether this track was imported from another Engine Prime
     *        library
     * 
     * If the value of `imported` is set to `false`, then the external database
     * UUID and track id in external database fields will be reset.
     */
    void set_imported(bool imported);

    void set_imported(
            bool imported,
            const std::string &external_database_uuid,
            int track_id_in_external_database);
    void set_album_art_id(int album_art_id);

    /**
     * Save a track to a given database
     *
     * If the track came from the supplied database originally, it is updated
     * in-place.  If the track does not already exist in the supplied database,
     * then it will be saved as a new track there.  The `id()` method will
     * return a new value after a new track is saved.
     */
    void save(const database &database);

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

std::vector<int> all_track_ids(const database &database);

} // engineprime

#endif // ENGINEPRIME_TRACK_HPP
