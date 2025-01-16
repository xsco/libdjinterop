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

#pragma once
#ifndef DJINTEROP_DATABASE_HPP
#define DJINTEROP_DATABASE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop
{
// TODO (mr-smidge): Review use of fwd decls here.  A user should be able to
//  include any public header and get to work!
class crate;
class database_impl;
class track;
struct track_snapshot;

class DJINTEROP_PUBLIC database
{
public:
    /// Copy constructor
    database(const database& db);

    /// Destructor
    ~database();

    /// Copy assignment operator
    database& operator=(const database& db);

    /// Returns the crate with the given ID
    ///
    /// If no such crate exists in the database, then `std::nullopt`
    /// is returned.
    std::optional<crate> crate_by_id(int64_t id) const;

    /// Returns all crates contained in the database
    std::vector<crate> crates() const;

    /// Returns all crates with the given name
    std::vector<crate> crates_by_name(const std::string& name) const;

    /// Create a new root crate with the given name.  The created crate has no parent.
    crate create_root_crate(const std::string& name);

    /// Create a new root crate with the given name, after the given crate in order.
    crate create_root_crate_after(const std::string& name, const crate& after);

    /// Create a new track in the database, given a pre-populated track
    /// snapshot.
    track create_track(const track_snapshot& snapshot);

    /// Returns the path directory of the database
    ///
    /// This is the same as the directory passed to the `database` constructor.
    std::string directory() const;

    /// Returns the UUID of the database
    std::string uuid() const;

    /// Verifies the consistency of the internal storage of the database.
    ///
    /// A `database_inconsistency` (or some exception derived from it) is thrown
    /// if any kind of inconsistency is found.
    void verify() const;

    /// Returns a descriptive name for the database version.
    std::string version_name() const;

    /// Removes a crate from the database
    ///
    /// All handles to that crate become invalid.
    void remove_crate(crate cr) const;

    /// Removes a track from the database
    ///
    /// All handles to that track become invalid.
    void remove_track(track tr) const;

    /// Returns the root-level crate with the given name.
    ///
    /// If no such crate exists, then `std::nullopt` is returned.
    std::optional<crate> root_crate_by_name(const std::string& name) const;

    /// Returns all root crates contained in the database
    ///
    /// A root crate is a crate that has no parent.
    std::vector<crate> root_crates() const;

    /// Returns the track with the given id
    ///
    /// If no such track exists in the database, then `std::nullopt`
    /// is returned.
    std::optional<track> track_by_id(int64_t id) const;

    /// Returns all tracks whose `relative_path` attribute in the database
    /// matches the given string
    // TODO (mr-smidge): Change to optional<track>, not vector.
    std::vector<track> tracks_by_relative_path(
        const std::string& relative_path) const;

    /// Returns all tracks contained in the database
    std::vector<track> tracks() const;

    // TODO (haslersn): non public?
    database(std::shared_ptr<database_impl> pimpl);

private:
    std::shared_ptr<database_impl> pimpl_;
};

}  // namespace djinterop

#endif  // DJINTEROP_DATABASE_HPP
