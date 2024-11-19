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

#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "../../util/sqlite_transaction.hpp"
#include "../engine_library_context.hpp"

namespace djinterop::engine::v2
{

playlist_entity_table::playlist_entity_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

int64_t playlist_entity_table::add_back(
    const playlist_entity_row& row, bool throw_if_duplicate)
{
    if (row.id != PLAYLIST_ENTITY_ROW_ID_NONE)
    {
        throw playlist_entity_row_id_error{
            "The provided playlist entity row already pertains to a persisted "
            "playlist entity, and so it cannot be created again"};
    }

    std::optional<int64_t> existing_id;
    context_->db << "SELECT id FROM PlaylistEntity WHERE listId = ? AND "
                    "trackId = ? AND databaseUuid = ?"
                 << row.list_id << row.track_id << row.database_uuid >>
        [&](int64_t id) { existing_id = id; };

    if (existing_id)
    {
        if (throw_if_duplicate)
        {
            throw std::invalid_argument{
                "The provided playlist entity already exists for the given "
                "playlist, and duplicate entries are not permitted"};
        }

        return *existing_id;
    }

    // The last entity in a playlist always has next entity id zero.
    int64_t next_entity_id = 0;

    util::sqlite_transaction trans{context_->db};
    context_->db
        << "INSERT INTO PlaylistEntity ("
           "listId, trackId, databaseUuid, nextEntityId, membershipReference)"
           "VALUES (?, ?, ?, ?, ?)"
        << row.list_id << row.track_id << row.database_uuid << next_entity_id
        << row.membership_reference;

    auto id = context_->db.last_insert_rowid();

    // The entity that was previously last in the playlist must now point to
    // the new entity as its 'next entity'.
    context_->db << "UPDATE PlaylistEntity SET nextEntityId = ? WHERE listId = "
                    "? AND nextEntityId = 0 AND id <> ?"
                 << id << row.list_id << id;
    trans.commit();

    return id;
}

void playlist_entity_table::clear(int64_t list_id)
{
    context_->db << "DELETE FROM PlaylistEntity WHERE listId = ?" << list_id;
}

std::optional<playlist_entity_row> playlist_entity_table::get(
    int64_t list_id, int64_t track_id) const
{
    std::optional<playlist_entity_row> result;
    context_->db << "SELECT id, listId, trackId, databaseUuid, nextEntityId, "
                    "membershipReference FROM PlaylistEntity WHERE listId = ? "
                    "AND trackId = ?"
                 << list_id << track_id >>
        [&](int64_t id, int64_t list_id, int64_t track_id,
            std::string database_uuid, int64_t next_entity_id,
            int64_t membership_reference)
    {
        assert(!result);
        result = playlist_entity_row{
            id,
            list_id,
            track_id,
            std::move(database_uuid),
            next_entity_id,
            membership_reference};
    };

    return result;
}

std::list<playlist_entity_row> playlist_entity_table::get_for_list(
    int64_t list_id) const
{
    std::unordered_map<int64_t, playlist_entity_row> next_entity_id_map;
    context_->db << "SELECT id, listId, trackId, databaseUuid, nextEntityId, "
                    "membershipReference FROM PlaylistEntity WHERE listId = ? "
                 << list_id >>
        [&](int64_t id, int64_t list_id, int64_t track_id,
            std::string database_uuid, int64_t next_entity_id,
            int64_t membership_reference)
    {
        next_entity_id_map[next_entity_id] = playlist_entity_row{
            id,
            list_id,
            track_id,
            std::move(database_uuid),
            next_entity_id,
            membership_reference};
    };

    std::list<playlist_entity_row> results;
    if (next_entity_id_map.empty())
        return results;

    auto curr = next_entity_id_map.find(PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID);
    assert(curr != next_entity_id_map.end());

    do
    {
        auto id = curr->second.id;
        results.push_front(std::move(curr->second));
        curr = next_entity_id_map.find(id);
    } while (curr != next_entity_id_map.end());

    return results;
}

std::vector<int64_t> playlist_entity_table::track_ids(int64_t list_id) const
{
    std::vector<int64_t> results;

    auto entities = get_for_list(list_id);
    for (auto&& entity : entities)
        results.push_back(entity.track_id);

    return results;
}

void playlist_entity_table::remove(int64_t list_id, int64_t entity_id)
{
    context_->db << "DELETE FROM PlaylistEntity WHERE listId = ? AND id = ?"
                 << list_id << entity_id;
}

}  // namespace djinterop::engine::v2
