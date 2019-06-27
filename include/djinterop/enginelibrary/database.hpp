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

#ifndef DJINTEROP_ENGINELIBRARY_DATABASE_HPP
#define DJINTEROP_ENGINELIBRARY_DATABASE_HPP

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <djinterop/enginelibrary/crate.hpp>
#include <djinterop/enginelibrary/schema_version.hpp>
#include <djinterop/enginelibrary/track.hpp>

namespace sqlite
{
class database;
}

namespace djinterop
{
namespace enginelibrary
{
class database_impl;

class database_not_found : public std::runtime_error
{
public:
    explicit database_not_found(const std::string& what_arg) noexcept
        : runtime_error{what_arg}
    {
    }
};

class database
{
public:
    /// Constructs an Engine Prime database, given the path to the Engine
    /// library directory
    explicit database(const std::string& directory);

    /// Copy constructor
    database(const database& db);

    /// Destructor
    ~database();

    /// Copy assignment operator
    database& operator=(const database& db);

    /// Returns the crate with the given ID
    ///
    /// If no such crate exists in the database, then boost::none is returned.
    boost::optional<crate> crate_by_id(int64_t id) const;

    /// Returns all crates contained in the database
    std::vector<crate> crates() const;

    /// Returns all crates with the given name
    std::vector<crate> crates_by_name(const std::string& name) const;

    /// Creates a new crate with the given name
    ///
    /// The created crate has no parent.
    crate create_crate(const std::string& name) const;

    /// Creates a new track associated to a given music file
    ///
    /// The music file is given by its relative path from the Engine library
    /// directory. The created track is not contained in any crates.
    track create_track(const std::string& relative_path) const;

    /// Returns the path to the Engine library directory of the database
    std::string directory() const;

    /// Returns true iff the database version is supported by this version of
    /// `libdjinterop` or not
    bool is_supported() const;

    /// Returns the path to the music database, i.e. m.db
    std::string music_db_path() const;

    /// Returns the path to the performance data database, i.e. p.db
    std::string perfdata_db_path() const;

    /// Returns the UUID of the database
    std::string uuid() const;

    /// Verifies the schema of an Engine Prime database and throws an exception
    /// if there is any kind of inconsistency
    void verify() const;

    /// Returns the schema version of the database
    schema_version version() const;

    /// Removes a crate from the database
    ///
    /// All handles to that crate become invalid.
    void remove_crate(crate cr) const;

    /// Removes a track from the database
    ///
    /// All handles to that track become invalid.
    void remove_track(track tr) const;

    /// Returns all root crates contained in the database
    ///
    /// A root crate is a crate that has no parent.
    std::vector<crate> root_crates() const;

    /// Returns the track with the given id
    ///
    /// If no such track exists in the database, then boost::none is returned.
    boost::optional<track> track_by_id(int64_t id) const;

    /// Returns all tracks whose `relative_path` attribute in the database
    /// matches the given string
    std::vector<track> tracks_by_relative_path(
        const std::string& relative_path) const;

    /// Returns all tracks contained in the database
    std::vector<track> tracks() const;

private:
    std::shared_ptr<database_impl> pimpl_;

    friend class crate;
    friend class track;
};

/// Creates a new, empty database in a given directory and using a specified
/// schema version
database make_database(
    const std::string& dir_path,
    const schema_version& default_version = version_latest);

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_DATABASE_HPP
