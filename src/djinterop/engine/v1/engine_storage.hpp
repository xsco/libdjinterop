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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/engine_schema.hpp>
#include <djinterop/exceptions.hpp>

#include "djinterop/engine/metadata_types.hpp"
#include "performance_data_format.hpp"
#include "../schema/schema.hpp"

namespace djinterop::engine::v1
{
/// The `track_row` struct represents a row from the `Track` table.
struct track_row
{
    std::optional<int64_t> play_order;
    std::optional<int64_t> length;
    std::optional<int64_t> length_calculated;
    std::optional<int64_t> bpm;
    std::optional<int64_t> year;
    std::optional<std::string> relative_path;
    std::optional<std::string> filename;
    std::optional<int64_t> bitrate;
    std::optional<double> bpm_analyzed;
    std::optional<int64_t> track_type;
    std::optional<int64_t> is_external_track;
    std::optional<std::string> uuid_of_external_database;
    std::optional<int64_t> id_track_in_external_database;
    std::optional<int64_t> album_art_id;
    std::optional<int64_t> file_bytes;
    std::optional<int64_t> pdb_import_key;
    std::optional<std::string> uri;
    std::optional<int64_t> is_beatgrid_locked;
};

/// The `meta_data_row` struct represents a row from the `MetaData` table.
struct meta_data_row
{
    int64_t id;
    metadata_str_type type;
    std::string value;
};

/// The `meta_data_integer_row` struct represents a row from the
/// `MetaDataInteger` table.
struct meta_data_integer_row
{
    int64_t id;
    metadata_int_type type;
    int64_t value;
};

/// The `performance_data_row` struct represents a row from the
/// `PerformanceData` table.
struct performance_data_row
{
    int64_t id;
    int64_t is_analyzed;
    int64_t is_rendered;
    std::optional<track_data> track_performance_data;
    std::optional<high_res_waveform_data> high_res_waveform;
    std::optional<overview_waveform_data> overview_waveform;
    std::optional<beat_data> beats;
    std::optional<quick_cues_data> quick_cues;
    std::optional<loops_data> loops;
    int64_t has_serato_values;
    int64_t has_rekordbox_values;
    int64_t has_traktor_values;
};

/// The `engine_storage` class provides access to persistent storage for Engine
/// data.
class engine_storage
{
public:
    engine_storage(
        const std::string& directory, const engine_schema& schema,
        sqlite::database db);

    /// Construct by loading from an existing DB directory.
    static std::shared_ptr<engine_storage> load(const std::string& directory);

    /// Make a new, empty DB of a given version.
    static std::shared_ptr<engine_storage> create(
        const std::string& directory, const engine_schema& schema);

    /// Make a new, empty, in-memory DB of a given version.
    ///
    /// Any changes made to the database will not persist beyond destruction
    /// of the class instance.
    static std::shared_ptr<engine_storage> create_temporary(
        const engine_schema& schema);

    /// Create an entry in the `Track` table.
    int64_t create_track(
        std::optional<int64_t> play_order, std::optional<int64_t> length,
        std::optional<int64_t> length_calculated, std::optional<int64_t> bpm,
        std::optional<int64_t> year,
        const std::optional<std::string>& relative_path,
        const std::optional<std::string>& filename,
        std::optional<int64_t> bitrate, std::optional<double> bpm_analyzed,
        std::optional<int64_t> track_type,
        std::optional<int64_t> is_external_track,
        const std::optional<std::string>& uuid_of_external_database,
        std::optional<int64_t> id_track_in_external_database,
        std::optional<int64_t> album_art_id,
        std::optional<int64_t> file_bytes,
        std::optional<int64_t> pdb_import_key,
        const std::optional<std::string>& uri,
        std::optional<int64_t> is_beatgrid_locked);

    /// Get a row from the `Track` table.
    track_row get_track(int64_t id);

    /// Get the value of a given column in the `Track` table.
    template <typename T>
    T get_track_column(int64_t id, const char* column_name)
    {
        std::optional<T> result;
        db << (std::string{"SELECT "} + column_name +
               " FROM Track WHERE id = ?")
           << id >>
            [&](T cell) {
                if (!result)
                {
                    result = cell;
                }
                else
                {
                    throw track_database_inconsistency{
                        "More than one track with the same ID", id};
                }
            };
        if (!result)
        {
            throw track_deleted{id};
        }
        return *result;
    }

    /// Update a row in the `Track` table.
    void update_track(
        int64_t id, std::optional<int64_t> play_order,
        std::optional<int64_t> length,
        std::optional<int64_t> length_calculated, std::optional<int64_t> bpm,
        std::optional<int64_t> year,
        const std::optional<std::string>& relative_path,
        const std::optional<std::string>& filename,
        std::optional<int64_t> bitrate, std::optional<double> bpm_analyzed,
        std::optional<int64_t> track_type,
        std::optional<int64_t> is_external_track,
        const std::optional<std::string>& uuid_of_external_database,
        std::optional<int64_t> id_track_in_external_database,
        std::optional<int64_t> album_art_id,
        std::optional<int64_t> file_bytes,
        std::optional<int64_t> pdb_import_key,
        const std::optional<std::string>& uri,
        std::optional<int64_t> is_beatgrid_locked);

    /// Set the value of a given column in the `Track` table.
    template <typename T>
    void set_track_column(int64_t id, const char* column_name, const T& content)
    {
        db << (std::string{"UPDATE Track SET "} + column_name +
               " = ? WHERE id = ?")
           << content << id;
    }

    /// Get all string meta-data for a track from the `MetaData` table.
    std::vector<meta_data_row> get_all_meta_data(int64_t id);

    /// Get string meta-data from the `MetaData` table.
    std::optional<std::string> get_meta_data(
        int64_t id, metadata_str_type type);

    /// Set string meta-data in the `MetaData` table.
    void set_meta_data(
        int64_t id, metadata_str_type type,
        std::optional<std::string> content);

    /// Set string meta-data in the `MetaData` table.
    void set_meta_data(
        int64_t id, metadata_str_type type, const std::string& content);

    /// Bulk-set entries in the `MetaData` table.
    void set_meta_data(
        int64_t id, const std::optional<std::string>& title,
        const std::optional<std::string>& artist,
        const std::optional<std::string>& album,
        const std::optional<std::string>& genre,
        const std::optional<std::string>& comment,
        const std::optional<std::string>& publisher,
        const std::optional<std::string>& composer,
        const std::optional<std::string>& duration_mm_ss,
        const std::optional<std::string>& ever_played,
        const std::optional<std::string>& file_extension);

    /// Get all integer meta-data for a track from the `MetaDataInteger` table.
    std::vector<meta_data_integer_row> get_all_meta_data_integer(int64_t id);

    /// Get integer meta-data from the `MetaDataInteger` table.
    std::optional<int64_t> get_meta_data_integer(
        int64_t id, metadata_int_type type);

    /// Set integer meta-data in the `MetaDataInteger` table.
    void set_meta_data_integer(
        int64_t id, metadata_int_type type, std::optional<int64_t> content);

    /// Bulk-set entries in the `MetaDataInteger` table.
    void set_meta_data_integer(
        int64_t id, std::optional<int64_t> musical_key,
        std::optional<int64_t> rating,
        std::optional<int64_t> last_played_timestamp,
        std::optional<int64_t> last_modified_timestamp,
        std::optional<int64_t> last_accessed_timestamp,
        std::optional<int64_t> last_play_hash);

    /// Remove an existing entry in the `PerformanceData` table, if it exists.
    void clear_performance_data(int64_t id);

    /// Get a row from the `PerformanceData` table.
    performance_data_row get_performance_data(int64_t id);

    /// Get the value of a given column in the `PerformanceData` table.
    template <typename T>
    T get_performance_data_column(int64_t id, const char* column_name)
    {
        std::optional<T> result;
        db << (std::string{"SELECT "} + column_name +
               " FROM PerformanceData WHERE id = ?")
           << id >>
            [&](const std::vector<std::byte>& encoded_data) {
                if (!result)
                {
                    result = T::decode(encoded_data);
                }
                else
                {
                    throw track_database_inconsistency{
                        "More than one PerformanceData entry for the same "
                        "track",
                        id};
                }
            };
        return result.value_or(T{});
    }

    /// Set (create or update) an entry in the `PerformanceData` table.
    void set_performance_data(
        int64_t id, int64_t is_analyzed, int64_t is_rendered,
        const track_data& track_data,
        const high_res_waveform_data& high_res_waveform_data,
        const overview_waveform_data& overview_waveform_data,
        const beat_data& beat_data, const quick_cues_data& quick_cues_data,
        const loops_data& loops_data, int64_t has_serato_values,
        int64_t has_rekordbox_values, int64_t has_traktor_values);

    /// Set the value of a given column in the `PerformanceData` table.
    template <typename T>
    void set_performance_data_column(
        int64_t id, const char* column_name, const T& content)
    {
        auto encoded_content = content.encode();
        // Check that subsequent reads can correctly decode what we are about to
        // write.
        if (!(T::decode(encoded_content) == content))
        {
            // TODO (haslersn): As soon as warnings are implemented, add the
            // wording similar to "Either you got a warning above which tells
            // you what is wrong, or this is a bug in libdjinterop."
            throw std::logic_error{
                "Data supplied for column " + std::string(column_name) +
                " is not invariant under encoding and subsequent decoding. "
                "This is a bug in libdjinterop."};
        }

        bool found = false;
        db << "SELECT COUNT(*) FROM PerformanceData WHERE id = ?" << id >>
            [&](int32_t count) {
                if (count == 1)
                {
                    found = true;
                }
                else if (count > 1)
                {
                    throw track_database_inconsistency{
                        "More than one PerformanceData entry for the same "
                        "track",
                        id};
                }
            };

        if (!found)
        {
            db << "INSERT INTO PerformanceData (id, isAnalyzed, isRendered, "
                  "trackData, highResolutionWaveFormData, "
                  "overviewWaveFormData, beatData, quickCues, loops, "
                  "hasSeratoValues) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
               << id                                 //
               << 1.0                                // isAnalyzed
               << 0.0                                // isRendered
               << track_data{}.encode()              //
               << high_res_waveform_data{}.encode()  //
               << overview_waveform_data{}.encode()  //
               << beat_data{}.encode()               //
               << quick_cues_data{}.encode()         //
               << loops_data{}.encode()              //
               << 0.0;                               // hasSeratoValues

            if (schema >= engine_schema::schema_1_7_1)
            {
                db << "UPDATE PerformanceData SET hasRekordboxValues = 0 "
                      "WHERE id = ?"
                   << id;
            }
        }

        db << (std::string{"UPDATE PerformanceData SET "} + column_name +
               " = ?, isAnalyzed = 1 WHERE id = ?")
           << encoded_content << id;
    }

    /// The directory in which the Engine DB files reside.
    const std::string directory;

    /// SQLite database handle, with both music and performance DBs attached.
    sqlite::database db;

    /// The schema version of the Engine database.
    const engine_schema schema;

    /// Pointer to the schema creator/validator.
    const std::unique_ptr<schema::schema_creator_validator>
        schema_creator_validator;
};

}  // namespace djinterop::engine::v1
