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

#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::engine
{
struct engine_library_context;
}

namespace djinterop::engine::v2
{

/// Thrown when the id on a playlist entity row is in an erroneous state for a
/// given operation.
struct DJINTEROP_PUBLIC playlist_entity_row_id_error : public std::runtime_error
{
public:
    explicit playlist_entity_row_id_error(const std::string& what_arg) noexcept
        : runtime_error{what_arg}
    {
    }
};

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr const int64_t PLAYLIST_ENTITY_ROW_ID_NONE = 0;

/// Special value for next entity id to indicate that there is no next entity.
constexpr const int64_t PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID = 0;

/// Default value for the membership reference field in a playlist entity row.
constexpr const int64_t PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE = 0;

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

    /// Id of next entity, or `PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID` if none.
    ///
    /// This field provides a mechanism to order the entities within a given
    /// playlist, in a manner similar to that of a singly-linked list.  When
    /// adding new rows to the table, there is no need to populate this field,
    /// as appropriate values will be calculated depending on the desired order.
    int64_t next_entity_id;

    /// Membership reference.
    ///
    /// The exact meaning of this field is currently unknown, but a value of
    /// zero is usually safe.  Where a track resides in both a playlist and one
    /// or more child playlists, the value has sometimes been observed to be
    /// `1`.
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

    /// Add a new entity to a playlist.
    ///
    /// A track may not be added to a playlist more than once.  If the track
    /// already exists in the playlist and `throw_if_duplicate` is `true`, an
    /// exception will be thrown.  If the track already exists in the playlist
    /// and `throw_if_duplicate` is `false`, then no action is taken, and the
    /// value of the existing row is returned.
    ///
    /// This function will take care of modifying the `next_entity_id` field
    /// of any preceding playlist entity.
    ///
    /// \param row Playlist entity row to add.
    /// \param throw_if_duplicate Flag indicating whether to throw an exception
    ///                           if the same track is added to the same
    ///                           playlist more than once (as opposed to doing
    ///                           nothing and returning the existing row).
    /// \return Returns the `id` column of the newly-added row.
    int64_t add(
        const playlist_entity_row& row, bool throw_if_duplicate = false);

    /// Remove all entities in a given playlist.
    ///
    /// \param list_id Id of playlist.
    void clear(int64_t list_id);

    /// Get the entity row for a given playlist and track.
    ///
    /// \param list_id Id of playlist.
    /// \param track_id Id of track in playlist.
    /// \return Returns the optional playlist entity row.
    [[nodiscard]] std::optional<playlist_entity_row> get(
        int64_t list_id, int64_t track_id) const;

    /// Get a list of entity rows for a given playlist, in playlist order.
    ///
    /// \param list_id Id of playlist.
    /// \return Returns a list of playlist entity rows.
    [[nodiscard]] std::list<playlist_entity_row> get_for_list(
        int64_t list_id) const;

    /// Get the ids of all tracks in a given playlist, in playlist order.
    ///
    /// \param list_id Id of playlist.
    /// \return Returns a vector of playlist entity ids.
    [[nodiscard]] std::vector<int64_t> track_ids(int64_t list_id) const;

    /// Remove an entity from a playlist.
    ///
    /// \param list_id Id of list to remove from.
    /// \param track_id Id of track to remove.
    void remove(int64_t list_id, int64_t track_id);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
