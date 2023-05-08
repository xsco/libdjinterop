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
#include <utility>

#include <djinterop/exceptions.hpp>

#include "../../util/chrono.hpp"
#include "engine_library_context.hpp"

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
                    "lastEditTime, isExplicitlyExported) VALUES (?, ?, ?, ?, ?)"
                 << row.title << row.parent_list_id << row.is_persisted
                 << djinterop::util::to_iso8601(row.last_edit_time)
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

std::vector<int64_t> playlist_table::all_root_ids() const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM Playlist WHERE parentListId = 0" >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

std::vector<int64_t> playlist_table::children(int64_t id) const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM Playlist WHERE parentListId = ?" << id >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

std::vector<int64_t> playlist_table::descendants(int64_t id) const
{
    std::vector<int64_t> results;
    context_->db << "SELECT childListId FROM PlaylistAllChildren WHERE id = ?"
                 << id >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

bool playlist_table::exists(int64_t id) const
{
    int64_t result;
    context_->db << "SELECT COUNT(*) FROM Playlist WHERE id = ?" << id >>
        [&](int64_t count) { result = count; };

    return result > 0;
}

std::vector<int64_t> playlist_table::find(const std::string& title) const
{
    std::vector<int64_t> results;
    context_->db << "SELECT id FROM Playlist WHERE title = ?" << title >>
        [&](int64_t id) { results.push_back(id); };

    return results;
}

stdx::optional<int64_t> playlist_table::find(
    int64_t parent_id, const std::string& title) const
{
    stdx::optional<int64_t> result;
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

stdx::optional<int64_t> playlist_table::find_root(
    const std::string& title) const
{
    stdx::optional<int64_t> result;
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

djinterop::stdx::optional<playlist_row> playlist_table::get(int64_t id) const
{
    djinterop::stdx::optional<playlist_row> result;
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
            djinterop::util::parse_iso8601(last_edited_time),
            is_explicitly_exported};
    };

    return result;
}

void playlist_table::remove(int64_t id)
{
    context_->db << "DELETE FROM Playlist WHERE id = ?" << id;
}

void playlist_table::update(const playlist_row& row)
{
    if (row.id == PLAYLIST_ROW_ID_NONE)
    {
        throw playlist_row_id_error{
            "The playlist row to update does not contain a playlist id"};
    }

    ensure_valid_name(row.title);

    context_->db
        << "UPDATE Playlist SET title = ?, parentListId = ?, isPersisted = ?, "
           "lastEditTime = ?, isExplicitlyExported = ? WHERE Id = ?"
        << row.title << row.parent_list_id << row.is_persisted
        << djinterop::util::to_iso8601(row.last_edit_time)
        << row.is_explicitly_exported << row.id;
}

}  // namespace djinterop::engine::v2
