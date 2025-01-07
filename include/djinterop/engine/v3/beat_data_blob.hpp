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

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <djinterop/engine/v2/beat_data_blob.hpp>

namespace djinterop::engine::v3
{
/// Represents a marker within a beat grid, part of the beat data blob.
///
/// Two or more markers form a beat grid, identifying the position of beats
/// within a track, with the assumption that beats between each pair of markers
/// occur at a constant tempo.
///
/// In Engine databases, the first marker is customarily at beat -4 (i.e. before
/// the start of the track), and the last marker is at beat N+1 (i.e. the first
/// beat after the end of the track).  Any markers in between are custom
/// downbeats and can occur at any point in the track.
using beat_grid_marker_blob = djinterop::engine::v2::beat_grid_marker_blob;

/// Represents the beat data blob.
using beat_data_blob = djinterop::engine::v2::beat_data_blob;

}  // namespace djinterop::engine::v3
