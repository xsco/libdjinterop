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

#include <djinterop/config.hpp>
#include <djinterop/engine/v2/playlist_table.hpp>

namespace djinterop::engine::v3
{
/// Thrown when the id on a playlist row is in an erroneous state for a given
/// operation.
using playlist_row_id_error = djinterop::engine::v2::playlist_row_id_error;

/// Thrown when the title of a playlist row is invalid in some way.
using playlist_row_invalid_title =
    djinterop::engine::v2::playlist_row_invalid_title;

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr int64_t PLAYLIST_ROW_ID_NONE = djinterop::engine::v2::PLAYLIST_ROW_ID_NONE;

/// Special value for parent id to indicate that a given row is at the root
/// level, and does not have any parent playlist.
constexpr int64_t PARENT_LIST_ID_NONE = djinterop::engine::v2::PARENT_LIST_ID_NONE;

/// Special value for next list id to indicate that there is no next list.
constexpr int64_t PLAYLIST_NO_NEXT_LIST_ID = djinterop::engine::v2::PLAYLIST_NO_NEXT_LIST_ID;

/// Represents a row in the `PlayList` table.
using playlist_row = djinterop::engine::v2::playlist_row;

/// Represents the `Playlist` table in an Engine v2 database.
using playlist_table = djinterop::engine::v2::playlist_table;

}  // namespace djinterop::engine::v3
