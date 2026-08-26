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

#include "playlist_table.hpp"

#include <utility>

namespace djinterop::onelibrary
{
namespace
{
// rekordbox marks a root playlist either by leaving the parent unset or by
// writing zero, and nothing in the schema forces one or the other, so both
// spellings have to be matched.
constexpr const char* select_roots =
    "SELECT playlist_id FROM playlist "
    "WHERE (playlist_id_parent IS NULL OR playlist_id_parent = 0) "
    "ORDER BY sequenceNo, playlist_id";

constexpr const char* find_root_by_name =
    "SELECT playlist_id FROM playlist "
    "WHERE (playlist_id_parent IS NULL OR playlist_id_parent = 0) "
    "AND name = ? ORDER BY sequenceNo, playlist_id LIMIT 1";

/// A parent identifier of zero means the same as none.
std::optional<int64_t> normalise_parent(std::optional<int64_t> parent_id)
{
    if (!parent_id.has_value() || *parent_id == 0)
        return std::nullopt;

    return parent_id;
}

}  // anonymous namespace

playlist_table::playlist_table(std::shared_ptr<onelibrary_context> context) :
    context_{std::move(context)}
{
}

std::optional<playlist_row> playlist_table::get(int64_t id) const
{
    std::optional<playlist_row> result;

    context_->db << "SELECT playlist_id, name, playlist_id_parent, sequenceNo "
                    "FROM playlist WHERE playlist_id = ?"
                 << id >>
        [&](int64_t row_id, std::optional<std::string> name,
            std::optional<int64_t> parent_id,
            std::optional<int64_t> sequence_number)
    {
        result = playlist_row{
            row_id, name.value_or(std::string{}), normalise_parent(parent_id),
            sequence_number};
    };

    return result;
}

bool playlist_table::exists(int64_t id) const
{
    return any_row(
        *context_, "SELECT 1 FROM playlist WHERE playlist_id = ? LIMIT 1", id);
}

std::vector<int64_t> playlist_table::root_ids() const
{
    return collect_ids(*context_, select_roots);
}

std::vector<int64_t> playlist_table::child_ids(int64_t id) const
{
    return collect_ids(
        *context_,
        "SELECT playlist_id FROM playlist WHERE playlist_id_parent = ? "
        "ORDER BY sequenceNo, playlist_id",
        id);
}

std::vector<int64_t> playlist_table::descendant_ids(int64_t id) const
{
    // One recursive query rather than one per node.  `depth` keeps the result
    // breadth first, and `sequenceNo` keeps siblings in their own order.
    return collect_ids(
        *context_,
        "WITH RECURSIVE descendant(playlist_id, sequenceNo, depth) AS ("
        "SELECT playlist_id, sequenceNo, 0 FROM playlist "
        "WHERE playlist_id_parent = ? "
        "UNION ALL "
        "SELECT p.playlist_id, p.sequenceNo, descendant.depth + 1 "
        "FROM playlist AS p "
        "JOIN descendant ON p.playlist_id_parent = descendant.playlist_id) "
        "SELECT playlist_id FROM descendant "
        "ORDER BY depth, sequenceNo, playlist_id",
        id);
}

std::optional<int64_t> playlist_table::find_root(const std::string& name) const
{
    return first_id(*context_, find_root_by_name, name);
}

std::optional<int64_t> playlist_table::find_child(
    int64_t parent_id, const std::string& name) const
{
    return first_id(
        *context_,
        "SELECT playlist_id FROM playlist "
        "WHERE playlist_id_parent = ? AND name = ? "
        "ORDER BY sequenceNo, playlist_id LIMIT 1",
        parent_id, name);
}

std::vector<int64_t> playlist_table::track_ids(int64_t id) const
{
    return collect_ids(
        *context_,
        "SELECT content_id FROM playlist_content WHERE playlist_id = ? "
        "ORDER BY sequenceNo, rowid",
        id);
}

std::vector<int64_t> playlist_table::playlists_containing(
    int64_t track_id) const
{
    return collect_ids(
        *context_,
        "SELECT DISTINCT playlist_id FROM playlist_content "
        "WHERE content_id = ? ORDER BY playlist_id",
        track_id);
}

}  // namespace djinterop::onelibrary
