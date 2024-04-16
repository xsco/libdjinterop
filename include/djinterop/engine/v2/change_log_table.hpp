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

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

/// Represents a single row in the `ChangeLog` table.
///
/// Whenever a track is modified in some way within the Engine library, an entry
/// is expected to be added to the `ChangeLog` table referencing that track's
/// id.  No further information is recorded, other than the fact that the track
/// has somehow been modified.
struct DJINTEROP_PUBLIC change_log_row
{
    /// Changelog id.
    int64_t id;

    /// Track id.
    int64_t track_id;
};

inline bool operator==(const change_log_row& lhs, const change_log_row& rhs)
{
    return std::tie(lhs.id, lhs.track_id) == std::tie(rhs.id, rhs.track_id);
}

inline bool operator!=(const change_log_row& lhs, const change_log_row& rhs)
{
    return !(rhs == lhs);
}

/// Represents the `ChangeLog` table in an Engine v2 database.
class DJINTEROP_PUBLIC change_log_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit change_log_table(std::shared_ptr<engine_library_context> context);

    /// Add a new changelog entry.
    ///
    /// \param track_id Track id.
    /// \return Returns the new changelog id.
    int64_t add(int track_id);

    /// Get a list of all changelog entries.
    ///
    /// \return Returns a `std::vector` of all rows.
    [[nodiscard]] std::vector<change_log_row> all() const;

    /// Get a list of all changelog entries after a given id.
    ///
    /// \param id Changelog id after which to look for entries.
    /// \return Returns a `std::vector` of all rows.
    [[nodiscard]] std::vector<change_log_row> after(int64_t id) const;

    /// Get the most recent entry in the changelog.
    ///
    /// \return Returns the last entry.
    [[nodiscard]] std::optional<change_log_row> last() const;

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
