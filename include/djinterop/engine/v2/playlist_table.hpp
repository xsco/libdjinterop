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

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/optional.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

/// Thrown when the id on a playlist row is in an erroneous state for a given
/// operation.
struct DJINTEROP_PUBLIC playlist_row_id_error : public std::runtime_error
{
public:
    explicit playlist_row_id_error(const std::string& what_arg) noexcept
        : runtime_error{what_arg}
    {
    }
};

/// Special value for id to indicate that a given row is not currently persisted
/// in the database.
constexpr const int64_t PLAYLIST_ROW_ID_NONE = 0;

/// Special value for parent id to indicate that a given row is at the root
/// level, and does not have any parent playlist.
constexpr const int64_t PARENT_LIST_ID_NONE = 0;

/// Represents a row in the `PlayList` table.
struct DJINTEROP_PUBLIC playlist_row
{
    /// Auto-generated id column.
    int64_t id;

    /// Title.
    std::string title;

    /// Id of parent list, or zero if none.
    int64_t parent_list_id;

    /// Flag indicating whether the playlist is persisted or not.
    bool is_persisted;

    /// Id of next playlist, or zero if none.
    ///
    /// A value does not need to be provided for this field when writing data,
    /// as it will be calculated automatically via database triggers.
    int64_t next_list_id;

    /// Time at which playlist was last edited.
    std::chrono::system_clock::time_point last_edit_time;

    /// Flag indicating whether the playlist has been explicitly exported.
    bool is_explicitly_exported;
};

/// Represents the `Playlist` table in an Engine v2 database.
class DJINTEROP_PUBLIC playlist_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit playlist_table(std::shared_ptr<engine_library_context> context);

    /// Add a playlist row to the table.
    ///
    /// \param row Playlist row to add.
    /// \return Returns the id of the newly-added playlist row.
    /// \throws playlist_row_id_error If the row already has an id.
    int64_t add(const playlist_row& row);

    /// Get all playlist ids.
    ///
    /// \return Returns a vector of playlist ids.
    [[nodiscard]] std::vector<int64_t> all_ids() const;

    /// Get all root playlist ids, i.e. those not part of a parent playlist.
    ///
    /// \return Returns a vector of playlist ids.
    [[nodiscard]] std::vector<int64_t> all_root_ids() const;

    /// Get the ids of all child playlists of a given parent playlist.
    ///
    /// \param id Parent playlist id.
    /// \return Returns a list of playlist ids.
    std::vector<int64_t> children(int64_t id) const;

    /// Get the ids of all descendant playlists of a given parent playlist, i.e.
    /// those sub-playlists which have the given playlist as any parent,
    /// grandparent, great-grandparent, and so on.
    ///
    /// \param id Parent playlist id.
    /// \return Returns a list of playlist ids.
    std::vector<int64_t> descendants(int64_t id) const;

    /// Test whether a given playlist id exists.
    ///
    /// \param id Id of playlist to test.
    /// \return Returns `true` if the playlist exists, or `false` if not.
    [[nodiscard]] bool exists(int64_t id) const;

    /// Find all playlists with the given title.
    ///
    /// \param title Title of playlists to find.
    /// \return Returns a vector of playlist ids.
    [[nodiscard]] std::vector<int64_t> find(const std::string& title) const;

    /// Find a playlist, given its parent playlist and title.
    ///
    /// \param parent_id Id of parent playlist.
    /// \param title Title of playlist to find.
    /// \return Returns the id of the playlist, or none if not found.
    [[nodiscard]] stdx::optional<int64_t> find(
        int64_t parent_id, const std::string& title) const;

    /// Find a root playlist, given its title.
    ///
    /// \param title Title of playlist to find.
    /// \return Returns the id of the playlist, or none if not found.
    [[nodiscard]] stdx::optional<int64_t> find_root(
        const std::string& title) const;

    /// Get a playlist by id.
    ///
    /// \param id Id of playlist.
    /// \return Returns an optional playlist row.
    [[nodiscard]] djinterop::stdx::optional<playlist_row> get(int64_t id) const;

    /// Remove an entry from the table.
    ///
    /// \param id Id of playlist to remove.
    void remove(int64_t id);

    /// Update an entry in the table.
    ///
    /// \param row Row to update (the `id` field must be populated).
    void update(const playlist_row& row);

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
