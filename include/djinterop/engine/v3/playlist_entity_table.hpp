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

#include <djinterop/engine/v2/playlist_entity_table.hpp>

namespace djinterop::engine::v3
{
/// Thrown when the id on a playlist entity row is in an erroneous state for a
/// given operation.
using playlist_entity_row_id_error = djinterop::engine::v2::playlist_entity_row_id_error;

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr int64_t PLAYLIST_ENTITY_ROW_ID_NONE = djinterop::engine::v2::PLAYLIST_ENTITY_ROW_ID_NONE;

/// Special value for next entity id to indicate that there is no next entity.
constexpr int64_t PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID = djinterop::engine::v2::PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID;

/// Default value for the membership reference field in a playlist entity row.
constexpr int64_t PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE = djinterop::engine::v2::PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE;

/// Represents a row in the `PlayListEntity` table.
using playlist_entity_row = djinterop::engine::v2::playlist_entity_row;

/// Represents the `PlaylistEntity` table in an Engine v2 database.
using playlist_entity_table = djinterop::engine::v2::playlist_entity_table;

}  // namespace djinterop::engine::v3
