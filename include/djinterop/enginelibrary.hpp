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
#ifndef DJINTEROP_ENGINELIBRARY_HPP
#define DJINTEROP_ENGINELIBRARY_HPP

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <djinterop/performance_data.hpp>
#include <djinterop/semantic_version.hpp>

namespace djinterop
{
class database;

namespace enginelibrary
{
static constexpr semantic_version version_1_6_0{1, 6, 0};
static constexpr semantic_version version_1_7_1{1, 7, 1};
static constexpr semantic_version version_latest = version_1_7_1;

/// Loads a database from a given directory.
database load_database(std::string directory);

/// Loads a database from a given directory. If it doesn't exist, creates a new,
/// empty database in that directory. When a new database is created, then
/// the schema version given as default_version is used.
database make_database(
    std::string directory,
    const semantic_version& default_version = version_latest);

/// Given an enginelibrary database, returns the path to its m.db sqlite
/// database file
///
/// If the given database is not an enginelibrary, then the behaviour of this
/// function is undefined.
std::string music_db_path(const database& db);

/// Normalizes a beat-grid, so that the beat indexes are in the form normally
/// expected by Engine Prime.
///
/// By convention, the Engine Prime analyses tracks so that the first beat is
/// at index -4 (yes, negative!) and the last beat is the first beat past the
/// usable end of the track, which may not necessarily be aligned to the first
/// beat of a 4-beat bar.  Therefore, the sample offsets typically recorded by
/// Engine Prime do not lie within the actual track.
std::vector<beatgrid_marker> normalize_beatgrid(
    std::vector<beatgrid_marker> beatgrid, int64_t sample_count);

/// Given an enginelibrary database, returns the path to its p.db sqlite
/// database file
///
/// If the given database is not an enginelibrary, then the behaviour of this
/// function is undefined.
std::string perfdata_db_path(const database& db);

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_HPP
