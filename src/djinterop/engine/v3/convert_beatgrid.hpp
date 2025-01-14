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

#include <vector>

#include <djinterop/engine/v3/beat_data_blob.hpp>
#include <djinterop/performance_data.hpp>

#include "../v2/convert_beatgrid.hpp"

namespace djinterop::engine::v3::convert
{
namespace read
{
inline beatgrid_marker beatgrid_marker(
    const beat_grid_marker_blob& beat_grid_marker)
{
    return v2::convert::read::beatgrid_marker(beat_grid_marker);
}

inline std::vector<djinterop::beatgrid_marker> beatgrid_markers(
    const beat_data_blob::beat_grid_marker_blobs_type& beat_grid)
{
    return v2::convert::read::beatgrid_markers(beat_grid);
}
}  // namespace read

namespace write
{
inline beat_data_blob::beat_grid_marker_blobs_type beatgrid_markers(
    const std::vector<beatgrid_marker>& beatgrid_markers)
{
    return v2::convert::write::beatgrid_markers(beatgrid_markers);
}

inline v2::convert::write::converted_beatgrid_fields beatgrid(
    const std::vector<beatgrid_marker>& beatgrid)
{
    return v2::convert::write::beatgrid(beatgrid);
}
}  // namespace write
}  // namespace djinterop::engine::v3::convert
