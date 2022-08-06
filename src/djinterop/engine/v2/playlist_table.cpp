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

#include "../../util.hpp"
#include "engine_library_context.hpp"

namespace djinterop::engine::v2
{

playlist_table::playlist_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}
std::vector<playlist_row> playlist_table::all() const
{
    std::vector<playlist_row> results;
    context_->db << "SELECT id, title, parentListId, isPersisted, nextListId, "
                    "lastEditTime, isExplicitlyExported "
                    "FROM Playlist" >>
        [&](int64_t id, std::string title, int64_t parent_list_id,
            bool is_persisted, int64_t next_list_id,
            const std::string& last_edited_time, bool is_explicitly_exported)
    {
        results.push_back(playlist_row{
            id, std::move(title), parent_list_id, is_persisted, next_list_id,
            parse_iso8601(last_edited_time), is_explicitly_exported});
    };

    return results;
}

djinterop::stdx::optional<playlist_row> playlist_table::get(
    int64_t id) const
{
    djinterop::stdx::optional<playlist_row> result;
    context_->db << "SELECT id, title, parentListId, isPersisted, nextListId, "
                    "lastEditTime, isExplicitlyExported "
                    "FROM Playlist WHERE id = ?" << id >>
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
            parse_iso8601(last_edited_time),
            is_explicitly_exported};
    };

    return result;
}

}  // namespace djinterop::engine::v2
