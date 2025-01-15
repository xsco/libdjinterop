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

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine_schema.hpp>
#include <djinterop/pad_color.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop
{
class database;
struct beatgrid_marker;

namespace engine
{
namespace standard_pad_colors
{
constexpr pad_color pad_1{0xEA, 0xC5, 0x32, 0xFF};
constexpr pad_color pad_2{0xEA, 0x8F, 0x32, 0xFF};
constexpr pad_color pad_3{0xB8, 0x55, 0xBF, 0xFF};
constexpr pad_color pad_4{0xBA, 0x2A, 0x41, 0xFF};
constexpr pad_color pad_5{0x86, 0xC6, 0x4B, 0xFF};
constexpr pad_color pad_6{0x20, 0xC6, 0x7C, 0xFF};
constexpr pad_color pad_7{0x00, 0xA8, 0xB1, 0xFF};
constexpr pad_color pad_8{0x15, 0x8E, 0xE2, 0xFF};

constexpr std::array<pad_color, 8> pads{pad_1, pad_2, pad_3, pad_4,
                                        pad_5, pad_6, pad_7, pad_8};

}  // namespace standard_pad_colors

constexpr const char* default_database_dir_name = "Engine Library";

/// Creates a new, empty database in a directory using the version provided.
///
/// By convention, the last part of the directory path is "Engine Library".  If
/// a database already exists in the target directory, an exception will be
/// thrown.
database DJINTEROP_PUBLIC create_database(
    const std::string& directory, const engine_schema& schema = latest_schema);

/// Creates a new temporary database.
///
/// Any changes made to the database will not be persisted anywhere, and will
/// be lost upon destruction of the returned variable.
database DJINTEROP_PUBLIC
create_temporary_database(const engine_schema& schema = latest_schema);

/// Creates a new database from a set of SQL scripts.
///
/// The directory indicated by `script_directory` is expected to contain files
/// of the form "<dbname>.db.sql", which will be read and used to hydrate
/// SQLite databases with the name "<dbname>.db".  These hydrated SQLite
/// databases are then loaded into the returned `database` object.
///
/// \param db_directory Directory in which to create database.
/// \param script_directory Directory containing scripts.
/// \param loaded_schema Output reference parameter indicating the version of
///                       the created database.
/// \return Returns the created database.
database DJINTEROP_PUBLIC create_database_from_scripts(
    const std::string& db_directory, const std::string& script_directory,
    engine_schema& loaded_schema);

/// Creates a new database from a set of SQL scripts.
///
/// The directory indicated by `script_directory` is expected to contain files
/// of the form "<dbname>.db.sql", which will be read and used to hydrate
/// SQLite databases with the name "<dbname>.db".  These hydrated SQLite
/// databases are then loaded into the returned `database` object.
///
/// \param db_directory Directory in which to create database.
/// \param script_directory Directory containing scripts.
/// \return Returns the created database.
inline database DJINTEROP_PUBLIC create_database_from_scripts(
    const std::string& db_directory, const std::string& script_directory)
{
    engine_schema unused{};
    return create_database_from_scripts(db_directory, script_directory, unused);
}

/// Create or load an Engine Library database in a given directory.
///
/// If a database already exists in the directory, it will be loaded.  If not,
/// it will be created at the specified schema version.
///
/// \param directory Directory to create or load in.
/// \param schema Schema version, if the database does not already exist.
/// \param created Output reference parameter indicating whether a new database
///                was created or not.
/// \param loaded_schema Output reference parameter indicating the schema
/// version of any existing database that was loaded.  The value is not defined
/// if a new database was created.
/// \return Returns the created or loaded database.
database DJINTEROP_PUBLIC create_or_load_database(
    const std::string& directory, const engine_schema& schema, bool& created,
    engine_schema& loaded_schema);

/// Create or load an Engine Library database in a given directory.
///
/// If a database already exists in the directory, it will be loaded.  If not,
/// it will be created at the specified schema version.
///
/// \param directory Directory to create or load in.
/// \param schema Schema version, if the database does not already exist.
/// \param created Output reference parameter indicating whether a new database
///                was created or not.
/// \return Returns the created or loaded database.
inline database DJINTEROP_PUBLIC create_or_load_database(
    const std::string& directory, const engine_schema& schema, bool& created)
{
    engine_schema unused{};
    return create_or_load_database(directory, schema, created, unused);
}

/// Returns a boolean indicating whether an Engine Library already exists in a
/// given directory.
bool DJINTEROP_PUBLIC database_exists(const std::string& directory);

/// Loads an Engine Library database from a given directory.
///
/// \param directory Directory to load from.
/// \param loaded_schema Output reference parameter indicating the version of
///                       the loaded database.
/// \return Returns the loaded database.
database DJINTEROP_PUBLIC
load_database(const std::string& directory, engine_schema& loaded_schema);

/// Loads an Engine Library database from a given directory.
///
/// \param directory Directory to load from.
/// \return Returns the loaded database.
inline database DJINTEROP_PUBLIC load_database(const std::string& directory)
{
    engine_schema unused{};
    return load_database(directory, unused);
}

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

/// Calculate the recommended extents for an overview waveform expected by
/// Engine Prime.
///
/// The waveform for a track is provided merely as a set of waveform points,
/// and so the scale of it is only meaningful when a relationship between
/// the waveform and the samples it represents is known.  This function
/// provides the recommended size and number of samples per waveform entry that
/// should be understood when constructing or reading overview waveforms in
/// Engine Prime format.
///
/// Note that Engine v2 allows the user only to specify an overview waveform:
/// the high-resolution waveform is derived on-the-fly from the audio for v2
/// firmware/software.
///
/// \param sample_count Sample count.
/// \param sample_rate Sample rate.
/// \return Returns the calculated waveform extents.
waveform_extents DJINTEROP_PUBLIC calculate_overview_waveform_extents(
    unsigned long long sample_count, double sample_rate);

/// Calculate the recommended extents for a high resolution waveform expected
/// by Engine Prime.
///
/// The waveform for a track is provided merely as a set of waveform points,
/// and so the scale of it is only meaningful when a relationship between
/// the waveform and the samples it represents is known.  This function
/// provides the recommended size and number of samples per waveform entry that
/// should be understood when constructing or reading waveforms in Engine Prime
/// format.
///
/// Note that only Engine v1 allows the user to specify their own
/// high-resolution waveform for the audio.  Later versions only allow the user
/// to specify an overview waveform (with the high-resolution waveform being
/// derived on-the-fly from the audio for v2 firmware/software).
///
/// Note further that, when rendering the high-resolution waveform, each
/// individual band is scaled so that the largest value across the entire
/// waveform hits the top of the display.  Note also that the mid frequency is
/// always drawn over the low, and the high frequency is always drawn over the
/// low and mid, meaning that very loud high-frequency sounds will hide any low
/// or mid activity on the waveform rendering.  A further note is that when the
/// opacity is set to zero, this appears to translate into roughly 50% opacity
/// on a real rendering.
///
/// \param sample_count Sample count.
/// \param sample_rate Sample rate.
/// \return Returns the calculated waveform extents.
waveform_extents DJINTEROP_PUBLIC calculate_high_resolution_waveform_extents(
    unsigned long long sample_count, double sample_rate);

}  // namespace engine
}  // namespace djinterop

#endif  // DJINTEROP_ENGINE_ENGINE_HPP
