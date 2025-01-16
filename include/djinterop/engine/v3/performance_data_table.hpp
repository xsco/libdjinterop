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

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/engine/v3/beat_data_blob.hpp>
#include <djinterop/engine/v3/loops_blob.hpp>
#include <djinterop/engine/v3/overview_waveform_data_blob.hpp>
#include <djinterop/engine/v3/quick_cues_blob.hpp>
#include <djinterop/engine/v3/track_data_blob.hpp>
#include <djinterop/stream_helper.hpp>

namespace djinterop::engine
{
struct engine_library_context;
}

namespace djinterop::engine::v3
{

/// Thrown when the id on a track row is in an erroneous state for a given
/// operation.
struct DJINTEROP_PUBLIC performance_data_row_id_error : public std::runtime_error
{
public:
    explicit performance_data_row_id_error(const std::string& what_arg) noexcept :
        runtime_error{what_arg}
    {
    }
};

/// Represents a row in the `PerformanceData` table.
struct DJINTEROP_PUBLIC performance_data_row
{
    /// `trackId` column, containing id of track to which this row relates.
    int64_t track_id;

    /// `trackData` column.
    track_data_blob track_data;

    /// `overviewWaveFormData` column.
    overview_waveform_data_blob overview_waveform_data;

    /// `beatData` column.
    beat_data_blob beat_data;

    /// `quickCues` column.
    quick_cues_blob quick_cues;

    /// `loops` column.
    loops_blob loops;

    /// `thirdPartySourceId` column.
    std::optional<int64_t> third_party_source_id;

    /// `activeOnLoadLoops` column.
    std::optional<int64_t> active_on_load_loops;

    friend bool operator==(const performance_data_row& lhs, const performance_data_row& rhs) noexcept
    {
        return std::tie(
                   lhs.track_id, lhs.track_data,
                   lhs.overview_waveform_data, lhs.beat_data, lhs.quick_cues,
                   lhs.loops, lhs.third_party_source_id, lhs.active_on_load_loops) ==
               std::tie(
                   rhs.track_id, rhs.track_data,
                   rhs.overview_waveform_data, rhs.beat_data, rhs.quick_cues,
                   rhs.loops, rhs.third_party_source_id, rhs.active_on_load_loops);
    }

    friend bool operator!=(const performance_data_row& lhs, const performance_data_row& rhs) noexcept
    {
        return !(rhs == lhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const performance_data_row& obj)
    {
#define PRINT_FIELD(field) \
    os << ", " #field "="; \
    stream_helper::print(os, obj.field)

        os << "performance_data_row{id=" << obj.track_id;
        PRINT_FIELD(track_data);
        PRINT_FIELD(overview_waveform_data);
        PRINT_FIELD(beat_data);
        PRINT_FIELD(quick_cues);
        PRINT_FIELD(loops);
        PRINT_FIELD(third_party_source_id);
        PRINT_FIELD(active_on_load_loops);
        os << "}";
        return os;
#undef PRINT_FIELD
    }
};

/// Represents the `PerformanceData` table in an Engine v3 database.
///
/// \note Note that there is no need to explicitly add to the `PerformanceData`
/// table, as a blank placeholder row will be automatically added for each track
/// added to the `Track` table via a database trigger.
class DJINTEROP_PUBLIC performance_data_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit performance_data_table(std::shared_ptr<engine_library_context> context);

    /// Check whether a given performance data row exists.
    ///
    /// \param id Id of track to check.
    /// \return Returns `true` if the performance data exists, or `false` if not.
    [[nodiscard]] bool exists(int64_t track_id) const;

    /// Get an entire performance data row.
    ///
    /// \param id Id of track.
    /// \return Returns a performance data row, or none if not found.
    std::optional<performance_data_row> get(int64_t track_id) const;

    /// Get the `trackData` column for a given performance data row.
    track_data_blob get_track_data(int64_t track_id);

    /// Set the `trackData` column for a given performance data row.
    void set_track_data(int64_t track_id, const track_data_blob& track_data);

    /// Get the `overviewWaveFormData` column for a given performance data row.
    overview_waveform_data_blob get_overview_waveform_data(int64_t track_id);

    /// Set the `overviewWaveFormData` column for a given performance data row.
    void set_overview_waveform_data(
        int64_t track_id, const overview_waveform_data_blob& overview_waveform_data);

    /// Get the `beatData` column for a given performance data row.
    beat_data_blob get_beat_data(int64_t track_id);

    /// Set the `beatData` column for a given performance data row.
    void set_beat_data(int64_t track_id, const beat_data_blob& beat_data);

    /// Get the `quickCues` column for a given performance data row.
    quick_cues_blob get_quick_cues(int64_t track_id);

    /// Set the `quickCues` column for a given performance data row.
    void set_quick_cues(int64_t track_id, const quick_cues_blob& quick_cues);

    /// Get the `loops` column for a given performance data row.
    loops_blob get_loops(int64_t track_id);

    /// Set the `loops` column for a given performance data row.
    void set_loops(int64_t track_id, const loops_blob& loops);

    /// Get the `thirdPartySourceId` column for a given performance data row.
    std::optional<int64_t> get_third_party_source_id(int64_t track_id);

    /// Set the `thirdPartySourceId` column for a given performance data row.
    void set_third_party_source_id(
        int64_t track_id, const std::optional<int64_t>& third_party_source_id);

    /// Get the `activeOnLoadLoops` column for a given performance data row.
    std::optional<int64_t> get_active_on_load_loops(int64_t track_id);

    /// Set the `activeOnLoadLoops` column for a given performance data row.
    void set_active_on_load_loops(
        int64_t track_id, std::optional<int64_t> active_on_load_loops);

    /// Update an existing performance data row in the table.
    ///
    /// \param row Performance data row to update.
    void update(const performance_data_row& row);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v3
