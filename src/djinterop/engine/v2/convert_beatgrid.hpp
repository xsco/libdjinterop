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

#include <cstdint>
#include <vector>

#include <djinterop/engine/v2/beat_data_blob.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop::engine::v2::convert
{
namespace read
{
inline djinterop::beatgrid_marker beatgrid_marker(
    const beat_grid_marker_blob& beat_grid_marker)
{
    return djinterop::beatgrid_marker{
        static_cast<int32_t>(beat_grid_marker.beat_number),
        beat_grid_marker.sample_offset};
}

std::vector<djinterop::beatgrid_marker> beatgrid_markers(
    const beat_data_blob::beat_grid_marker_blobs_type& beat_grid)
{
    std::vector<djinterop::beatgrid_marker> converted;
    for (auto&& marker : beat_grid)
    {
        converted.push_back(beatgrid_marker(marker));
    }

    return converted;
}
}  // namespace read

namespace write
{
struct converted_beatgrid_fields
{
    uint8_t is_beatgrid_set;
    beat_data_blob::beat_grid_marker_blobs_type default_beat_grid;
    beat_data_blob::beat_grid_marker_blobs_type adjusted_beat_grid;
};

inline beat_data_blob::beat_grid_marker_blobs_type beatgrid_markers(
    const std::vector<djinterop::beatgrid_marker>& beatgrid_markers)
{
    beat_data_blob::beat_grid_marker_blobs_type converted;

    for (auto iter = std::begin(beatgrid_markers);
         iter != std::end(beatgrid_markers); ++iter)
    {
        if (!converted.empty())
        {
            auto& prev = converted.back();
            prev.number_of_beats = iter->index - prev.beat_number;
        }

        // Note: the `number_of_beats` field will be corrected in the next
        // loop iteration.
        converted.push_back(
            beat_grid_marker_blob{iter->sample_offset, iter->index, 0, 0});
    }

    return converted;
}

inline converted_beatgrid_fields beatgrid(
    const std::vector<djinterop::beatgrid_marker>& beatgrid)
{
    auto converted_beatgrid = beatgrid_markers(beatgrid);
    uint8_t is_beatgrid_set = converted_beatgrid.empty() ? 0 : 1;
    return {is_beatgrid_set, converted_beatgrid, converted_beatgrid};
}

inline converted_beatgrid_fields beatgrid(
    const std::vector<djinterop::beatgrid_marker>& default_beatgrid,
    const std::vector<djinterop::beatgrid_marker>& adjusted_beatgrid)
{
    auto converted_default_beatgrid = beatgrid_markers(default_beatgrid);
    auto converted_adjusted_beatgrid = beatgrid_markers(adjusted_beatgrid);
    uint8_t is_beatgrid_set = converted_adjusted_beatgrid.empty() ? 0 : 1;
    return {
        is_beatgrid_set, converted_default_beatgrid,
        converted_adjusted_beatgrid};
}
}  // namespace write
}  // namespace djinterop::engine::v2::convert
