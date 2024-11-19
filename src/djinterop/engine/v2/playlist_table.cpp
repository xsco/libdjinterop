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

#include <djinterop/engine/v2/playlist_table.hpp>

#include <cassert>
#include <unordered_map>
#include <utility>

#include <djinterop/exceptions.hpp>

#include "../../util/chrono.hpp"
#include "../../util/sqlite_transaction.hpp"
#include "../engine_library_context.hpp"

namespace djinterop::engine::v2
{
namespace
{
void ensure_valid_name(const std::string& name)
{
    if (name.empty())
    {
        throw djinterop::crate_invalid_name{
            "Crate names must be non-empty", name};
    }
    else if (name.find_first_of(';') != std::string::npos)
    {
        throw djinterop::crate_invalid_name{
            "Crate names must not contain semicolons", name};
    }
}

std::list<int64_t> sort_ids(
    const std::unordered_map<int64_t, int64_t>& next_list_id_to_id_map)
{
    std::list<int64_t> results;
    if (next_list_id_to_id_map.empty())
        return results;

    auto curr = next_list_id_to_id_map.find(PLAYLIST_NO_NEXT_LIST_ID);
    assert(curr != next_list_id_to_id_map.end());

    do
    {
        auto id = curr->second;
        results.push_front(id);
        curr = next_list_id_to_id_map.find(id);
    } while (curr != next_list_id_to_id_map.end());

    return results;
}
}  // namespace

playlist_table::playlist_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

int64_t playlist_table::add(const playlist_row& row)
{
    if (row.id != PLAYLIST_ROW_ID_NONE)
    {
        throw playlist_row_id_error{
            "The provided playlist row already pertains to a persisted "
            "playlist, and so it cannot be created again"};
    }

    ensure_valid_name(row.title);

    context_->db << "INSERT INTO Playlist (title, parentListId, isPersisted, "
                    "nextListId, lastEditTime, isExplicitlyExported) "
                    "VALUES (?, ?, ?, ?, ?, ?)"
                 << row.title << row.parent_list_id << row.is_persisted
                 << row.next_list_id
                 << djinterop::util::to_ft(row.last_edit_time)
                 << row.is_explicitly_exported;

    return context_->db.last_insert_rowid();
}

std::vector<int64_t> playlist_table::all_ids() const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM Playlist" >> [&](int64_t id)
    { results.push_back(id); };

    return results;
}

std::list<int64_t> playlist_table::child_ids(int64_t id) const
{
    std::unordered_map<int64_t, int64_t> next_list_id_to_id_map;
    context_->db << "SELECT id, nextListId FROM Playlist WHERE parentListId = ?"
                 << id >>
        [&](int64_t id, int64_t next_list_id)
    { next_list_id_to_id_map[next_list_id] = id; };

    return sort_ids(next_list_id_to_id_map);
}

std::vector<int64_t> playlist_table::descendant_ids(int64_t id) const
{
    std::vector<int64_t> results;
    context_->db << "SELECT childListId FROM PlaylistAllChildren WHERE id = ?"
                 << id >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

bool playlist_table::exists(int64_t id) const
{
    int64_t result = 0;
    context_->db << "SELECT COUNT(*) FROM Playlist WHERE id = ?" << id >>
        [&](int64_t count) { result = count; };

    return result > 0;
}

std::vector<int64_t> playlist_table::find_ids(const std::string& title) const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM Playlist WHERE title = ?" << title >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

std::optional<int64_t> playlist_table::find_id(
    int64_t parent_id, const std::string& title) const
{
    std::optional<int64_t> result;
    context_->db
            << "SELECT id FROM Playlist WHERE title = ? AND parentListId = ?"
            << title << parent_id >>
        [&](int64_t id)
    {
        assert(!result);
        result = id;
    };

    return result;
}

std::optional<int64_t> playlist_table::find_root_id(
    const std::string& title) const
{
    std::optional<int64_t> result;
    context_->db
            << "SELECT id FROM Playlist WHERE title = ? AND parentListId = 0"
            << title >>
        [&](int64_t id)
    {
        assert(!result);
        result = id;
    };

    return result;
}

std::optional<playlist_row> playlist_table::get(int64_t id) const
{
    std::optional<playlist_row> result;
    context_->db << "SELECT id, title, parentListId, isPersisted, nextListId, "
                    "lastEditTime, isExplicitlyExported "
                    "FROM Playlist WHERE id = ?"
                 << id >>
        [&](int64_t id, std::string title, int64_t parent_list_id,
            bool is_persisted, int64_t next_list_id,
            const std::string& last_edited_time, bool is_explicitly_exported)
    {
        assert(!result);
        result = playlist_row{
            id,
            std::move(title),
            parent_list_id,
            is_persisted,
            next_list_id,
            djinterop::util::parse_ft(last_edited_time),
            is_explicitly_exported};
    };

    return result;
}

void playlist_table::remove(int64_t id)
{
    context_->db << "DELETE FROM Playlist WHERE id = ?" << id;
}

std::list<int64_t> playlist_table::root_ids() const
{
    std::unordered_map<int64_t, int64_t> next_list_id_to_id_map;
    context_->db
            << "SELECT id, nextListId FROM Playlist WHERE parentListId = 0" >>
        [&](int64_t id, int64_t next_list_id)
    { next_list_id_to_id_map[next_list_id] = id; };

    return sort_ids(next_list_id_to_id_map);
}

void playlist_table::update(const playlist_row& row)
{
    if (row.id == PLAYLIST_ROW_ID_NONE)
    {
        throw playlist_row_id_error{
            "The playlist row to update does not contain a playlist id"};
    }

    ensure_valid_name(row.title);

    util::sqlite_transaction trans{context_->db};

    int64_t old_parent_list_id, old_next_list_id;
    context_->db << "SELECT parentListId, nextListId FROM Playlist WHERE Id = ?"
                 << row.id >>
        std::tie(old_parent_list_id, old_next_list_id);

    if (old_next_list_id == row.next_list_id &&
        old_parent_list_id == row.parent_list_id)
    {
        // When the relative ordering of the playlist is not changing, the
        // operation is a simple update.
        context_->db
            << "UPDATE Playlist SET title = ?, isPersisted = ?, "
               "lastEditTime = ?, isExplicitlyExported = ? WHERE Id = ?"
            << row.title << row.is_persisted
            << djinterop::util::to_ft(row.last_edit_time)
            << row.is_explicitly_exported << row.id;
    }
    else
    {
        // A database trigger ensures that `nextListId` is populated
        // consistently upon INSERT or DELETE, but there is no such trigger for
        // an UPDATE operation.  Accordingly, perform actions manually to keep
        // the sequencing correct, in case the parent list is modified.
        //
        // Assume that `row.id` is the subject, `row.next_list_id` is the
        // target:
        //
        // 1. Detach subject from next list using negative inversion trick.
        // 2. Attach subject's previous list to subject's original next list.
        // 3. Attach target's original previous list to subject.
        // 4. Attach subject to target next list, changing other fields
        //    (potentially including parent) at the same time.
        context_->db << "UPDATE Playlist SET nextListId = -(1 + nextListId) "
                        "WHERE id = ?"
                     << row.id;
        context_->db << "UPDATE Playlist SET nextListId = ? "
                        "WHERE nextListId = ? AND parentListId = ?"
                     << old_next_list_id << row.id << old_parent_list_id;

        context_->db << "UPDATE Playlist SET nextListId = ? "
                        "WHERE nextListId = ? AND parentListId = ?"
                     << row.id << row.next_list_id << row.parent_list_id;

        context_->db
            << "UPDATE Playlist SET title = ?, parentListId = ?, isPersisted = "
               "?, "
               "nextListId = ?, lastEditTime = ?, isExplicitlyExported = ? "
               "WHERE Id = ?"
            << row.title << row.parent_list_id << row.is_persisted
            << row.next_list_id << djinterop::util::to_ft(row.last_edit_time)
            << row.is_explicitly_exported << row.id;
    }

    trans.commit();
}

}  // namespace djinterop::engine::v2
