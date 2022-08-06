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
#ifndef DJINTEROP_ENGINE_ENGINE_HPP
#define DJINTEROP_ENGINE_ENGINE_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine_version.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/pad_color.hpp>

namespace djinterop
{
class database;
struct beatgrid_marker;

namespace engine
{
/// Engine DJ OS 1.0.0.
constexpr const engine_version os_1_0_0{
    semantic_version{1, 0, 0}, engine_database_type::os,
    "Engine DJ OS 1.0.0", semantic_version{1, 6, 0}};

/// Engine DJ OS 1.0.3.
constexpr const engine_version os_1_0_3{
    semantic_version{1, 0, 3}, engine_database_type::os,
    "Engine DJ OS 1.0.3", semantic_version{1, 7, 1}};

/// Engine Prime 1.1.1.
constexpr const engine_version desktop_1_1_1{
    semantic_version{1, 1, 1}, engine_database_type::desktop,
    "Engine Prime 1.1.1", semantic_version{1, 9, 1}};

/// Engine DJ OS 1.2.0.
constexpr const engine_version os_1_2_0{
    semantic_version{1, 2, 0}, engine_database_type::os,
    "Engine DJ OS 1.2.0", semantic_version{1, 11, 1}};

/// Engine DJ OS 1.2.2.
constexpr const engine_version os_1_2_2{
    semantic_version{1, 2, 2}, engine_database_type::os,
    "Engine DJ OS 1.2.2", semantic_version{1, 13, 0}};

/// Engine Prime 1.2.2.
constexpr const engine_version desktop_1_2_2{
    semantic_version{1, 2, 2}, engine_database_type::desktop,
    "Engine Prime 1.2.2", semantic_version{1, 13, 1}};

/// Engine DJ OS 1.3.1.
constexpr const engine_version os_1_3_1{
    semantic_version{1, 3, 1}, engine_database_type::os,
    "Engine DJ OS 1.3.1", semantic_version{1, 13, 2}};

/// Engine DJ OS 1.4.0.
constexpr const engine_version os_1_4_0{
    semantic_version{1, 4, 0}, engine_database_type::os,
    "Engine DJ OS 1.4.0", semantic_version{1, 15, 0}};

/// Engine DJ OS 1.5.1/1.5.2.
constexpr const engine_version os_1_5_1{
    semantic_version{1, 5, 1}, engine_database_type::os,
    "Engine DJ OS 1.5.1/1.5.2", semantic_version{1, 17, 0}};

/// Engine Prime 1.5.1/1.6.0/1.6.1.
constexpr const engine_version desktop_1_5_1{
    semantic_version{1, 5, 1}, engine_database_type::desktop,
    "Engine Prime 1.5.1/1.6.0/1.6.1", semantic_version{1, 18, 0}};

/// Engine DJ OS 1.6.0/1.6.1/1.6.2.
constexpr const engine_version os_1_6_0{
    semantic_version{1, 6, 0}, engine_database_type::os,
    "Engine DJ OS 1.6.0/1.6.1/1.6.2", semantic_version{1, 18, 0}};

/// Engine DJ Desktop 2.0.0
constexpr const engine_version desktop_2_0_0{
    semantic_version{2, 0, 0}, engine_database_type::desktop,
    "Engine DJ Desktop 2.0.0", semantic_version{2, 18, 0}};

/// Engine DJ OS 2.0.0
constexpr const engine_version os_2_0_0{
    semantic_version{2, 0, 0}, engine_database_type::os,
    "Engine DJ OS 2.0.0", semantic_version{2, 18, 0}};

/// Set of available versions.
constexpr const std::array<engine_version, 11> all_versions{
    os_1_0_0, os_1_0_3, desktop_1_1_1, os_1_2_0,      os_1_2_2, desktop_1_2_2,
    os_1_3_1, os_1_4_0, os_1_5_1,      desktop_1_5_1, os_1_6_0};

/// Set of available V1 versions.
constexpr const std::array<engine_version, 11> all_v1_versions{
    os_1_0_0, os_1_0_3, desktop_1_1_1, os_1_2_0,      os_1_2_2, desktop_1_2_2,
    os_1_3_1, os_1_4_0, os_1_5_1,      desktop_1_5_1, os_1_6_0};

/// Set of available V2 versions.
constexpr const std::array<engine_version, 2> all_v2_versions{
    desktop_2_0_0, os_2_0_0};

/// The most recent version supported by the library.
constexpr engine_version latest = os_1_6_0;

/// The most recent V2 version supported by the library.
constexpr engine_version latest_v2 = os_2_0_0;

/// The most recent OS-type version supported by the library.
constexpr engine_version latest_os = os_1_6_0;

namespace standard_pad_colors
{
constexpr const pad_color pad_1{0xEA, 0xC5, 0x32, 0xFF};
constexpr const pad_color pad_2{0xEA, 0x8F, 0x32, 0xFF};
constexpr const pad_color pad_3{0xB8, 0x55, 0xBF, 0xFF};
constexpr const pad_color pad_4{0xBA, 0x2A, 0x41, 0xFF};
constexpr const pad_color pad_5{0x86, 0xC6, 0x4B, 0xFF};
constexpr const pad_color pad_6{0x20, 0xC6, 0x7C, 0xFF};
constexpr const pad_color pad_7{0x00, 0xA8, 0xB1, 0xFF};
constexpr const pad_color pad_8{0x15, 0x8E, 0xE2, 0xFF};

constexpr const std::array<pad_color, 8> pads{pad_1, pad_2, pad_3, pad_4,
                                              pad_5, pad_6, pad_7, pad_8};

}  // namespace standard_pad_colors

constexpr const char* default_database_dir_name = "Engine Library";

/// The `unsupported_engine_database` exception is thrown when an Engine
/// database schema version is encountered that is not yet supported by this
/// version of the library.
class DJINTEROP_PUBLIC unsupported_engine_database : public unsupported_database
{
public:
    explicit unsupported_engine_database(const semantic_version& version)
        noexcept : unsupported_database{std::string{
                       "Unsupported database version " + to_string(version)}},
                   version{version}
    {
    }

    explicit unsupported_engine_database(
        const std::string& what_arg, const semantic_version& version) noexcept
        : unsupported_database{what_arg},
          version{version}
    {
    }

    const semantic_version version;
};

/// Creates a new, empty database in a directory using the version provided.
///
/// By convention, the last part of the directory path is "Engine Library".  If
/// a database already exists in the target directory, an exception will be
/// thrown.
database DJINTEROP_PUBLIC create_database(
    const std::string& directory,
    const engine_version& version = latest);

/// Creates a new temporary database.
///
/// Any changes made to the database will not be persisted anywhere, and will
/// be lost upon destruction of the returned variable.
database DJINTEROP_PUBLIC create_temporary_database(
    const engine_version& version = latest);

/// Creates a new database from a set of SQL scripts.
///
/// The directory indicated by `script_directory` is expected to contain files
/// of the form "<dbname>.db.sql", which will be read and used to hydrate
/// SQLite databases with the name "<dbname>.db".  These hydrated SQLite
/// databases are then loaded into the returned `database` object.
database DJINTEROP_PUBLIC create_database_from_scripts(
    const std::string& db_directory, const std::string& script_directory);

/// Create or load an Engine Library database in a given directory.
///
/// If a database already exists in the directory, it will be loaded.  If not,
/// it will be created at the specified schema version.  In both cases, the
/// database is returned.  The boolean reference parameter `created` can be used
/// to determine whether the database was created or merely loaded.
database DJINTEROP_PUBLIC create_or_load_database(
    const std::string& directory, const engine_version& version, bool& created);

/// Returns a boolean indicating whether an Engine Library already exists in a
/// given directory.
bool DJINTEROP_PUBLIC database_exists(const std::string& directory);

/// Loads an Engine Library database from a given directory.
database DJINTEROP_PUBLIC load_database(const std::string& directory);

/// Given an Engine Library database, returns the path to its m.db sqlite
/// database file
///
/// If the given database is not an enginelibrary, then the behaviour of this
/// function is undefined.
std::string DJINTEROP_PUBLIC music_db_path(const database& db);

/// Normalizes a beat-grid, so that the beat indexes are in the form normally
/// expected by Engine Prime.
///
/// By convention, the Engine Prime analyses tracks so that the first beat is
/// at index -4 (yes, negative!) and the last beat is the first beat past the
/// usable end of the track, which may not necessarily be aligned to the first
/// beat of a 4-beat bar.  Therefore, the sample offsets typically recorded by
/// Engine Prime do not lie within the actual track.
std::vector<beatgrid_marker> DJINTEROP_PUBLIC
normalize_beatgrid(std::vector<beatgrid_marker> beatgrid, int64_t sample_count);

/// Calculate the required number of samples per waveform entry expected by
/// Engine Prime.
///
/// The waveform for a track is provided merely as a set of waveform points,
/// and so the scale of it is only meaningful when a relationship between
/// the waveform and the samples it represents is known.  This function
/// provides the required number of samples per waveform entry that should
/// be understood when constructing or reading waveforms in Engine Prime format.
int64_t DJINTEROP_PUBLIC
required_waveform_samples_per_entry(double sample_rate);

/// Given an enginelibrary database, returns the path to its p.db sqlite
/// database file
///
/// If the given database is not an enginelibrary, then the behaviour of this
/// function is undefined.
std::string DJINTEROP_PUBLIC perfdata_db_path(const database& db);

}  // namespace engine
}  // namespace djinterop

#endif  // DJINTEROP_ENGINE_ENGINE_HPP
