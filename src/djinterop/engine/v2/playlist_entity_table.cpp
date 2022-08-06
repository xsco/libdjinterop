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

#include <djinterop/engine/v2/playlist_entity_table.hpp>

#include <utility>

#include "engine_library_context.hpp"

namespace djinterop::engine::v2
{

playlist_entity_table::playlist_entity_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}
std::vector<playlist_entity_row> playlist_entity_table::in_list(
    int64_t list_id) const
{
    std::vector<playlist_entity_row> results;
    context_->db << "SELECT id, listId, trackId, databaseUuid, nextEntityId, "
                    "membershipReference FROM PlaylistEntity WHERE listId = ?"
                 << list_id >>
        [&](int64_t id, int64_t list_id, int64_t track_id,
            std::string database_uuid, int64_t next_entity_id,
            int64_t membership_reference)
    {
        results.push_back(playlist_entity_row{
            id, list_id, track_id, std::move(database_uuid), next_entity_id,
            membership_reference});
    };

    return results;
}

}  // namespace djinterop::engine::v2
