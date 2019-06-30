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

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_CRATE_HPP
#define DJINTEROP_ENGINELIBRARY_CRATE_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include "djinterop/enginelibrary/database.hpp"

namespace djinterop
{
namespace enginelibrary
{
/**
 * The `nonexistent_crate` exception is thrown when a request is made for
 * a crate using an identifier that does not exist in a given database
 */
class nonexistent_crate : public std::invalid_argument
{
public:
    /**
     * \brief Construct the exception for a given crate id
     */
    explicit nonexistent_crate(int id) noexcept
        : invalid_argument{"Crate does not exist in database"}, id_{id}
    {
    }

    /**
     * \brief Destructor
     */
    virtual ~nonexistent_crate() = default;

    /**
     * \brief Get the crate id that was deemed non-existent
     */
    int id() const noexcept { return id_; }

private:
    int id_;
};

/**
 * The `crate_database_inconsistency` exception is thrown when there is an
 * internal database inconsistency with regard to how a crate is stored
 */
class crate_database_inconsistency : public std::logic_error
{
public:
    /**
     * \brief Construct the exception for a given crate id and custom message
     */
    explicit crate_database_inconsistency(
        const std::string &what_arg, int id) noexcept
        : logic_error{what_arg.c_str()}, id_{id}
    {
    }

    /**
     * \brief Construct the exception for a given crate id
     */
    explicit crate_database_inconsistency(int id) noexcept
        : logic_error{"Crate has internal database inconsistency"}, id_{id}
    {
    }

    /**
     * \brief Destructor
     */
    virtual ~crate_database_inconsistency() = default;

    /**
     * \brief Get the crate id that was deemed inconsistent
     */
    int id() const noexcept { return id_; }

private:
    int id_;
};

/**
 * The `crate` class represents a record of a given crate of tracks (or indeed
 * other crates) in the database
 */
class crate
{
public:
    typedef std::vector<int>::const_iterator crate_id_iterator;
    typedef std::unordered_set<int>::const_iterator track_id_iterator;

    /**
     * \brief Construct a new crate, not yet saved in any database
     */
    crate();

    /**
     * \brief Copy a crate from another
     *
     * Note that the new copied crate will not be considered to belong in any
     * database, and hence will have its `id` set to zero.
     */
    crate(const crate &other);

    /**
     * \brief Construct a crate, loaded from a given database by its id
     */
    crate(const database &database, int id);

    /**
     * \brief Destructor
     */
    ~crate();

    /**
     * \brief Copy from another crate into this crate
     *
     * Note that this crate will not be considered to belong in any database
     * after copy assignment, and hence will have its `id` set to zero.
     */
    crate &operator=(const crate &other);

    /**
     * \brief Get the id of this crate, as stored in the database.
     *
     * If the crate is not yet stored in any database, the id will be zero.
     */
    int id() const;

    /**
     * \brief Get the crate's name
     */
    std::string name() const;

    /**
     * \brief Get a `bool` indicating if this crate has a parent crate
     */
    bool has_parent() const;

    /**
     * \brief Get the parent crate id for this crate, if it has one
     *
     * If the crate does not have a parent, the return value of this method is
     * undefined.
     */
    int parent_id() const;

    /**
     * \brief Get iterator to the start of a list of child crates of this one
     *
     * Note that if child crates are simultaneously modified in the background,
     * this list may no longer reflect the current reality in the database.
     */
    crate_id_iterator child_crates_begin() const;

    /**
     * \brief Get iterator beyond the last element of a list of child crates of
     * this one
     *
     * Note that if child crates are simultaneously modified in the background,
     * this list may no longer reflect the current reality in the database.
     */
    crate_id_iterator child_crates_end() const;

    /**
     * \brief Get iterator to the beginning of a list of track ids that are part
     * of this crate
     */
    track_id_iterator tracks_begin() const;

    /**
     * \brief Get iterator beyond the last element of a list of track ids that
     * are part of this crate
     */
    track_id_iterator tracks_end() const;

    void set_name(const std::string &name);
    void set_parent_id(int parent_crate_id);
    void set_no_parent();
    void add_tracks(track_id_iterator begin, track_id_iterator end);
    void add_track(int track_id);
    void set_tracks(track_id_iterator begin, track_id_iterator end);
    void clear_tracks();

    /**
     * \brief Save a crate to a given database
     *
     * If the crate came from the supplied database originally, it is updated
     * in-place.  If the crate does not already exist in the supplied database,
     * then it will be saved as a new track there, and the `id()` method will
     * return a new value after a new track is saved.
     *
     * If you are moving a crate to a new database, you may wish to double-check
     * the validity of the parent crate id, if one has been set.  If a parent
     * crate id is taken from a previous database and accidentally left on a
     * crate that is to be saved in a different database, the id may be invalid
     * in the context of the new database!
     *
     * The same argument applies for the list of tracks in the crate: a crate
     * may only contain tracks from the same database in which the crate lives.
     */
    void save(const database &database);

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

/**
 * \brief Get a list of all crate ids in a given database
 */
std::vector<int> all_crate_ids(const database &database);

/**
 * \brief Get a list of all "root" crate ids in a given database, i.e. those
 * that are not a sub-crate of another
 */
std::vector<int> all_root_crate_ids(const database &database);

/**
 * \brief Try to find a crate by its (unique) name
 *
 * If the crate is found, its id will be written to the provided reference
 * variable.
 */
bool find_crate_by_name(
    const database &database, const std::string &name, int &crate_id);

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_CRATE_HPP
