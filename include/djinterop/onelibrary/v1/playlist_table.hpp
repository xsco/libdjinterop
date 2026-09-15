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
#ifndef DJINTEROP_ONELIBRARY_V1_PLAYLIST_TABLE_HPP
#define DJINTEROP_ONELIBRARY_V1_PLAYLIST_TABLE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::onelibrary
{
struct onelibrary_context;

namespace v1
{
/// Special value for id to indicate that a given row is not a row of the
/// database, and the value a root playlist carries as its parent.
constexpr int64_t PLAYLIST_ROW_ID_NONE = 0;

/// One row of the `playlist` table.
struct playlist_row
{
    int64_t id = PLAYLIST_ROW_ID_NONE;
    std::string name;

    /// The playlist this one sits under.  A root has either no parent
    /// recorded or a parent of `PLAYLIST_ROW_ID_NONE`; rekordbox writes both.
    std::optional<int64_t> parent_id;

    /// Position among siblings, counting from one.
    std::optional<int64_t> sequence_number;
};

/// Read access to the `playlist` tree and the `playlist_content` membership.
class DJINTEROP_PUBLIC playlist_table
{
public:
    explicit playlist_table(std::shared_ptr<onelibrary_context> context);

    [[nodiscard]] std::optional<playlist_row> get(int64_t id) const;

    [[nodiscard]] bool exists(int64_t id) const;

    /// Identifiers of the playlists with no parent, in sibling order.
    [[nodiscard]] std::vector<int64_t> root_ids() const;

    /// Identifiers of the children of a playlist, in sibling order.
    [[nodiscard]] std::vector<int64_t> child_ids(int64_t id) const;

    /// Identifiers of every playlist beneath one, breadth first.
    [[nodiscard]] std::vector<int64_t> descendant_ids(int64_t id) const;

    /// Find a root playlist by name.
    [[nodiscard]] std::optional<int64_t> find_root(
        const std::string& name) const;

    /// Find a child of a playlist by name.
    [[nodiscard]] std::optional<int64_t> find_child(
        int64_t parent_id, const std::string& name) const;

    /// Identifiers of the tracks in a playlist, in playlist order.
    [[nodiscard]] std::vector<int64_t> track_ids(int64_t id) const;

    /// Identifiers of the playlists that hold a given track.
    [[nodiscard]] std::vector<int64_t> playlists_containing(
        int64_t track_id) const;

private:
    std::shared_ptr<onelibrary_context> context_;
};

}  // namespace v1
}  // namespace djinterop::onelibrary

#endif  // DJINTEROP_ONELIBRARY_V1_PLAYLIST_TABLE_HPP
