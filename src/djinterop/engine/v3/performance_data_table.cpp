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

#include <djinterop/engine/v3/performance_data_table.hpp>

#include <cassert>
#include <cstddef>
#include <utility>

#include <djinterop/exceptions.hpp>

#include "../../util/chrono.hpp"
#include "../../util/convert.hpp"
#include "../engine_library_context.hpp"

namespace djinterop::engine::v3
{
namespace
{
template <typename ColumnType>
ColumnType get_column(
    sqlite::database& db, int64_t track_id, const std::string& column_name)
{
    std::optional<ColumnType> result;
    auto sql = "SELECT " + column_name + " FROM PerformanceData WHERE trackId = ?";
    db << sql << track_id >> [&](ColumnType value) { result = value; };

    if (result)
        return *result;

    throw performance_data_row_id_error{"No row found for given id"};
}

template <typename ColumnType>
void set_column(
    sqlite::database& db, int64_t track_id, const std::string& column_name,
    const ColumnType& value)
{
    auto sql = "UPDATE PerformanceData SET " + column_name + " = ? WHERE trackId = ?";
    db << sql << value << track_id;

    if (db.rows_modified() > 0)
        return;

    throw performance_data_row_id_error{"No row found for given track id"};
}

}  // anonymous namespace

performance_data_table::performance_data_table(std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

bool performance_data_table::exists(int64_t track_id) const
{
    bool result = false;
    context_->db << "SELECT COUNT(*) FROM PerformanceData WHERE trackId = ?" << track_id >>
        [&](int64_t count)
    {
        if (count == 1)
        {
            result = true;
        }
        else if (count > 1)
        {
            throw track_database_inconsistency{
                "More than one performance data row with the same ID", track_id};
        }
    };
    return result;
}

std::optional<performance_data_row> performance_data_table::get(int64_t id) const
{
    std::optional<performance_data_row> result;

    context_->db
            << "SELECT trackId, "
               "trackData, "
               "overviewWaveFormData, beatData, quickCues, loops, "
               "thirdPartySourceId, activeOnLoadLoops "
               "FROM PerformanceData WHERE trackId = ?"
            << id >>
        [&](int64_t track_id,
            const std::vector<std::byte>& track_data,
            const std::vector<std::byte>& overview_waveform_data,
            const std::vector<std::byte>& beat_data,
            const std::vector<std::byte>& quick_cues,
            const std::vector<std::byte>& loops,
            std::optional<int64_t> third_party_source_id,
            std::optional<int64_t> active_on_load_loops)
    {
        assert(!result);

        result = performance_data_row{
            track_id,
            track_data_blob::from_blob(track_data),
            overview_waveform_data_blob::from_blob(overview_waveform_data),
            beat_data_blob::from_blob(beat_data),
            quick_cues_blob::from_blob(quick_cues),
            loops_blob::from_blob(loops),
            third_party_source_id,
            active_on_load_loops};
    };

    return result;
}

void performance_data_table::update(const performance_data_row& row)
{
    context_->db << "UPDATE PerformanceData SET "
                    "trackData = ?, "
                    "overviewWaveFormData = ?, "
                    "beatData = ?, quickCues = ?, "
                    "loops = ?, thirdPartySourceId = ?, "
                    "activeOnLoadLoops = ?"
                    "WHERE trackId = ?"
                 << row.track_data.to_blob()
                 << row.overview_waveform_data.to_blob()
                 << row.beat_data.to_blob() << row.quick_cues.to_blob()
                 << row.loops.to_blob() << row.third_party_source_id
                 << row.active_on_load_loops
                 << row.track_id;
}

track_data_blob performance_data_table::get_track_data(int64_t track_id)
{
    return track_data_blob::from_blob(
        get_column<std::vector<std::byte>>(context_->db, track_id, "trackData"));
}

void performance_data_table::set_track_data(int64_t track_id, const track_data_blob& track_data)
{
    set_column<std::vector<std::byte>>(
        context_->db, track_id, "trackData", track_data.to_blob());
}

overview_waveform_data_blob performance_data_table::get_overview_waveform_data(int64_t track_id)
{
    return overview_waveform_data_blob::from_blob(
        get_column<std::vector<std::byte>>(
            context_->db, track_id, "overviewWaveFormData"));
}

void performance_data_table::set_overview_waveform_data(
    int64_t track_id, const overview_waveform_data_blob& overview_waveform_data)
{
    set_column<std::vector<std::byte>>(
        context_->db, track_id, "overviewWaveFormData",
        overview_waveform_data.to_blob());
}

beat_data_blob performance_data_table::get_beat_data(int64_t track_id)
{
    return beat_data_blob::from_blob(
        get_column<std::vector<std::byte>>(context_->db, track_id, "beatData"));
}

void performance_data_table::set_beat_data(int64_t track_id, const beat_data_blob& beat_data)
{
    set_column<std::vector<std::byte>>(
        context_->db, track_id, "beatData", beat_data.to_blob());
}

quick_cues_blob performance_data_table::get_quick_cues(int64_t track_id)
{
    return quick_cues_blob::from_blob(
        get_column<std::vector<std::byte>>(context_->db, track_id, "quickCues"));
}

void performance_data_table::set_quick_cues(int64_t track_id, const quick_cues_blob& quick_cues)
{
    set_column<std::vector<std::byte>>(
        context_->db, track_id, "quickCues", quick_cues.to_blob());
}

loops_blob performance_data_table::get_loops(int64_t track_id)
{
    return loops_blob::from_blob(
        get_column<std::vector<std::byte>>(context_->db, track_id, "loops"));
}

void performance_data_table::set_loops(int64_t track_id, const loops_blob& loops)
{
    set_column<std::vector<std::byte>>(
        context_->db, track_id, "loops", loops.to_blob());
}

std::optional<int64_t> performance_data_table::get_third_party_source_id(int64_t track_id)
{
    return get_column<std::optional<int64_t>>(
        context_->db, track_id, "thirdPartySourceId");
}

void performance_data_table::set_third_party_source_id(
    int64_t track_id, const std::optional<int64_t>& third_party_source_id)
{
    set_column<std::optional<int64_t>>(
        context_->db, track_id, "thirdPartySourceId", third_party_source_id);
}

std::optional<int64_t> performance_data_table::get_active_on_load_loops(int64_t track_id)
{
    return get_column<std::optional<int64_t>>(
        context_->db, track_id, "activeOnLoadLoops");
}

void performance_data_table::set_active_on_load_loops(
    int64_t track_id, std::optional<int64_t> active_on_load_loops)
{
    set_column<std::optional<int64_t>>(
        context_->db, track_id, "activeOnLoadLoops", active_on_load_loops);
}

}  // namespace djinterop::engine::v3
