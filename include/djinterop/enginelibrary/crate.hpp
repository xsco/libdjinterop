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

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/optional.hpp>

#include <djinterop/enginelibrary/schema_version.hpp>

namespace djinterop
{
namespace enginelibrary
{
class database;
class track;

/// The `crate_deleted` exception is thrown when an invalid `crate` object is
/// used, i.e. one that does not exist in the database anymore.
class crate_deleted : public std::runtime_error
{
public:
    /// Constructs the exception for a given crate ID
    explicit crate_deleted(int64_t id) noexcept
        : runtime_error{"Crate does not exist in database anymore"}, id_{id}
    {
    }

    /// Returns the crate ID that was deemed non-existent
    int64_t id() const noexcept { return id_; }

private:
    int64_t id_;
};

/// The `crate_database_inconsistency` exception is thrown when a database
/// inconsistency is found that correlates to a crate.
class crate_database_inconsistency : public database_inconsistency
{
public:
    /// Construct the exception for a given crate ID
    explicit crate_database_inconsistency(
        const std::string& what_arg, int64_t id) noexcept
        : database_inconsistency{what_arg.c_str()}, id_{id}
    {
    }

    /// Get the crate ID that was deemed inconsistent
    int64_t id() const noexcept { return id_; }

private:
    int64_t id_;
};

/// A `crate` object is a handle to a crate stored in a database. As long as it
/// lives, the corresponding database connection is kept open.
///
/// `crate` objects can be copied and assigned cheaply, resulting in multiple
/// handles to the same actual crate.
///
/// The read/write operations provided by this class directly access the
/// database.
///
/// A `crate` object becomes invalid if the crate gets deleted by
/// `database::remove_crate()`. After that, you must not call any methods on the
/// `crate` object, except for destructing it, or assigning to it.
class crate
{
public:
    /// Copy constructor
    crate(const crate& other) noexcept;

    /// Destructor
    ~crate();

    /// Copy assignment operator
    crate& operator=(const crate& other) noexcept;

    /// Adds a track to the crate
    ///
    /// A track can be contained in arbitrarily many (including zero) crates.
    void add_track(track tr) const;

    /// Returns the (direct) children of this crate
    std::vector<crate> children() const;

    /// Removes all tracks from the crate
    ///
    /// Note that the tracks stay in the database even if they're contained in
    /// zero crates.
    void clear_tracks() const;

    /// Returns the database containing the crate
    database db() const;

    /// Returns the descendants of this crate
    ///
    /// A descendant is a direct or indirect child of this crate.
    std::vector<crate> descendants() const;

    /// Returns the ID of this crate
    ///
    /// The ID is used internally in the database and is unique for crates
    /// contained in the same database.
    int64_t id() const;

    /// Returns `true` iff `*this` is valid as described in the class comment
    bool is_valid() const;

    /// Returns the crate's name
    std::string name() const;

    /// Returns the parent crate, if this crate has one
    ///
    /// If the crate doesn't have a parent, then an `boost::none` is returned.
    boost::optional<crate> parent() const;

    /// Removes a track from the crate
    ///
    /// Note that the track stays in the database even if it's contained in zero
    /// crates.
    void remove_track(track tr) const;

    /// Sets the crate's name
    void set_name(const std::string& name) const;

    /// Sets this crate's parent
    ///
    /// If an empty `boost::optional` is given, then this crate will have no
    /// parent. That is, it becomes a root crate.
    void set_parent(boost::optional<crate> parent) const;

    /// Returns the crate's contained tracks
    std::vector<track> tracks() const;

private:
    crate(database db, int64_t id);

    struct impl;
    std::shared_ptr<impl> pimpl_;

    friend class database;
    friend class track;
};

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_CRATE_HPP
