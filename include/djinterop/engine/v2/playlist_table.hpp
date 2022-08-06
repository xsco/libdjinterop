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
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/optional.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

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

    /// Get all playlists.
    ///
    /// \return Returns a vector of playlist rows.
    [[nodiscard]] std::vector<playlist_row> all() const;

    /// Get a playlist by id.
    ///
    /// \param id Id of playlist.
    /// \return Returns an optional playlist row.
    [[nodiscard]] djinterop::stdx::optional<playlist_row> get(int64_t id) const;

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
