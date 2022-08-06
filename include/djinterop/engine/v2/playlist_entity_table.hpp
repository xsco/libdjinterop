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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

/// Represents a row in the `PlayListEntity` table.
struct DJINTEROP_PUBLIC playlist_entity_row
{
    /// Auto-generated id column.
    int64_t id;

    /// Id of playlist to which this entity belongs.
    int64_t list_id;

    /// Id of track in the playlist.
    int64_t track_id;

    /// UUID of database in which the track resides.
    std::string database_uuid;

    /// Id of next entity, or zero if none.
    ///
    /// A value does not need to be provided for this field when writing data,
    /// as it will be calculated automatically via database triggers.
    int64_t next_entity_id;

    /// Membership reference.
    int64_t membership_reference;
};

/// Represents the `PlaylistEntity` table in an Engine v2 database.
class DJINTEROP_PUBLIC playlist_entity_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit playlist_entity_table(
        std::shared_ptr<engine_library_context> context);

    /// Get all entities for a given playlist.
    ///
    /// \return Returns a vector of playlist rows.
    [[nodiscard]] std::vector<playlist_entity_row> in_list(
        int64_t list_id) const;

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
