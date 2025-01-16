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

#include <djinterop/config.hpp>
#include <djinterop/engine/base_engine_library.hpp>
#include <djinterop/engine/v2/change_log_table.hpp>
#include <djinterop/engine/v2/information_table.hpp>
#include <djinterop/engine/v2/playlist_entity_table.hpp>
#include <djinterop/engine/v2/playlist_table.hpp>
#include <djinterop/engine/v2/track_table.hpp>

namespace djinterop::engine::v2
{
/// Represents an Engine Library with schema version 2.x.
///
/// A library with this schema is achieved by having a specific directory
/// structure in which one or more SQLite databases are stored.  The top-level
/// directory is typically named `Engine Library`, and there must then be a
/// subdirectory beneath that named `Database2`.  The main SQLite database
/// resides in that directory, and is named `m.db`.
///
/// Note that the directory that should be passed to constructors and member
/// functions of this class must be the `Engine Library` directory, not the
/// `Database2` directory.
class DJINTEROP_PUBLIC engine_library : public base_engine_library
{
public:
    using base_engine_library::base_engine_library;

    /// Load an existing library from a directory.
    ///
    /// \param directory Directory to load from.
    static engine_library load(const std::string& directory)
    {
        return engine_library{base_engine_library::load(directory)};
    }

    /// Make a new, empty library of a given version.
    ///
    /// \param directory Directory in which to create the new library.
    /// \param schema Version to create.
    /// \return Returns the new Engine library.
    static engine_library create(
        const std::string& directory, const engine_schema& schema)
    {
        return engine_library{base_engine_library::create(directory, schema)};
    }

    /// Make a new, empty, in-memory library of a given version.
    ///
    /// Any changes made to the library will not persist beyond destruction
    /// of the class instance.
    ///
    /// \param schema Version to create.
    /// \return Returns the new temporary Engine library.
    static engine_library create_temporary(const engine_schema& schema)
    {
        return engine_library{base_engine_library::create_temporary(schema)};
    }

    /// Test whether an Engine Library already exists in the given directory.
    ///
    /// \param directory Directory to test.
    /// \return Returns a flag indicating whether an Engine library exists.
    static bool exists(const std::string& directory)
    {
        return base_engine_library::exists(directory);
    }

    /// Get a class representing the `ChangeLog` table.
    [[nodiscard]] change_log_table change_log() const
    {
        return change_log_table{context_};
    }

    /// Get a class representing the `Information` table.
    [[nodiscard]] information_table information() const
    {
        return information_table{context_};
    }

    /// Get a class representing the `PlaylistEntity` table.
    [[nodiscard]] playlist_entity_table playlist_entity() const
    {
        return playlist_entity_table{context_};
    }

    /// Get a class representing the `Playlist` table.
    [[nodiscard]] playlist_table playlist() const
    {
        return playlist_table{context_};
    }

    /// Get a class representing the `Track` table.
    [[nodiscard]] track_table track() const { return track_table{context_}; }

    /// Get the unified database interface for this Engine library.
    [[nodiscard]] djinterop::database database() const override;
};

}  // namespace djinterop::engine::v2
