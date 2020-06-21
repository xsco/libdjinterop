/*
    Tdd!his file is part of libdjinterop.
dd!
    ldd!ibdjinterop is free software: you can redistribute it and/or modify
    idd!t under the terms of the GNU Lesser General Public License as published
by tdd!he Free Software Foundation, either version 3 of the License, or (dd!at
your option) any later version. dd! ldd!ibdjinterop is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef DJINTEROP_CRATE_HPP
#define DJINTEROP_CRATE_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop
{
class database;
class crate_impl;
class track;

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
class DJINTEROP_PUBLIC crate
{
public:
    /// Copy constructor
    crate(const crate& other) noexcept;

    /// Destructor
    ~crate();

    /// Copy assignment operator
    crate& operator=(const crate& other) noexcept;

    /// Adds a track to the crate.
    void add_track(int64_t track_id) const;

    /// Adds a track to the crate.
    ///
    /// A track can be contained in arbitrarily many (including zero) crates.
    void add_track(track tr) const;

    /// Add a range of tracks to the crate.
    template <typename InputIterator>
    void add_tracks(InputIterator first, InputIterator last)
    {
        for (auto iter = first; iter != last; ++iter)
            add_track(*iter);
    }

    /// Returns the (direct) children of this crate
    std::vector<crate> children() const;

    /// Removes all tracks from the crate
    ///
    /// Note that the tracks stay in the database even if they're contained in
    /// zero crates.
    void clear_tracks() const;

    /// Creates a new, empty crate as a child of this one, and returns it.
    crate create_sub_crate(std::string name);

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
    /// If the crate doesn't have a parent, then `djinterop::nullopt` is
    /// returned.
    std::optional<crate> parent() const;

    /// Removes a track from the crate
    ///
    /// Note that the track stays in the database even if it's contained in zero
    /// crates.
    void remove_track(track tr) const;

    /// Sets the crate's name
    void set_name(std::string name) const;

    /// Sets this crate's parent
    ///
    /// If `djinterop::nullopt` is given, then this crate will have no parent.
    /// That is, it becomes a root crate.
    void set_parent(std::optional<crate> parent) const;

    /// Gets the sub-crate of this one with a given name.
    ///
    /// If no such crate is found, then `djinterop::nullopt` is returned.
    std::optional<crate> sub_crate_by_name(const std::string& name) const;

    /// Returns the crate's contained tracks
    std::vector<track> tracks() const;

    // TODO (haslersn): non public?
    crate(std::shared_ptr<crate_impl> pimpl);

private:
    std::shared_ptr<crate_impl> pimpl_;

    friend class database;
    friend class track;
};

}  // namespace djinterop

#endif  // DJINTEROP_CRATE_HPP
