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

#include <cstdint>
#include <memory>
#include <string>

#include <djinterop/config.hpp>

namespace djinterop::engine
{
struct engine_library_context;
}

namespace djinterop::engine::v2
{

/// Represents the single-row contents of the `Information` table.
struct DJINTEROP_PUBLIC information_row
{
    /// Auto-generated id column.
    ///
    /// Not especially meaningful when there is only a single row in the table.
    int64_t id;

    /// UUID for the database.
    std::string uuid;

    /// Major part of the database schema version number.
    int64_t schema_version_major;

    /// Minor part of the database schema version number.
    int64_t schema_version_minor;

    /// Patch part of the database schema version number.
    int64_t schema_version_patch;

    /// Current played indicator.
    ///
    /// Note that each track can have a play indicator number, which is a random
    /// 64-bit integer.  The indicator in the `Information` table will align
    /// with the played indicator number of the track that was most recently
    /// played.
    int64_t current_played_indicator;

    /// Last RekordBox library import read counter.
    int64_t last_rekord_box_library_import_read_counter;
};

inline bool operator==(const information_row& x, const information_row& y)
{
    return x.id == y.id && x.uuid == y.uuid &&
           x.schema_version_major == y.schema_version_major &&
           x.schema_version_minor == y.schema_version_minor &&
           x.schema_version_patch == y.schema_version_patch &&
           x.current_played_indicator == y.current_played_indicator &&
           x.last_rekord_box_library_import_read_counter ==
               y.last_rekord_box_library_import_read_counter;
}

inline bool operator!=(const information_row& x, const information_row& y)
{
    return !(x == y);
}

/// Represents the `Information` table in an Engine v2 database.
///
/// Note that the `Information` table in a valid database always has exactly
/// one row.  Zero rows or more than one is considered an invalid database.
class DJINTEROP_PUBLIC information_table
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit information_table(std::shared_ptr<engine_library_context> context);

    /// Update the current played indicator.
    ///
    /// \param played_indicator New played indicator value.
    void update_current_played_indicator(int64_t played_indicator);

    /// Get the information row.
    ///
    /// \return Returns the information row.
    [[nodiscard]] information_row get() const;

private:
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
