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

#include <memory>
#include <string>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine_version.hpp>
#include <djinterop/engine/v2/change_log_table.hpp>
#include <djinterop/engine/v2/information_table.hpp>
#include <djinterop/engine/v2/playlist_entity_table.hpp>
#include <djinterop/engine/v2/playlist_table.hpp>
#include <djinterop/engine/v2/track_table.hpp>

namespace djinterop::engine::v2
{
struct engine_library_context;

/// Represents a version 2 Engine Library.
///
/// A version 2 Engine Library is achieved by having a specific directory
/// structure in which one or more SQLite databases are stored.  The top-level
/// directory is typically named `Engine Library`, and there must then be a
/// sub-directory beneath that named `Database2`.  The main SQLite database
/// resides in that directory, and is named `m.db`.
///
/// Note that the directory that should be passed to constructors and member
/// functions of this class must be the `Engine Library` directory, not the
/// `Database2` directory.
class DJINTEROP_PUBLIC engine_library
{
public:
    /// Construct by loading from an existing directory.
    ///
    /// \param directory Directory to load from.
    explicit engine_library(const std::string& directory);

    /// Make a new, empty library of a given version.
    ///
    /// \param directory Directory in which to create the new library.
    /// \param version Version to create.
    /// \return Returns the new Engine library.
    static engine_library create(
        const std::string& directory, const engine_version& version);

    /// Make a new, empty, in-memory library of a given version.
    ///
    /// Any changes made to the library will not persist beyond destruction
    /// of the class instance.
    ///
    /// \param version Version to create.
    /// \return Returns the new temporary Engine library.
    static engine_library create_temporary(const engine_version& version);

    /// Test whether an Engine Library already exists in the given directory.
    ///
    /// \param directory Directory to test.
    /// \return Returns a flag indicating whether an Engine library exists.
    static bool exists(const std::string& directory);

    /// Verify the correctness of the Engine library database schema for its
    /// stated version.
    void verify() const;

    /// Get the unified database interface for this Engine library.
    [[nodiscard]] djinterop::database database() const;

    /// Get the Engine library top-level directory.
    ///
    /// \return Returns the top-level directory.
    [[nodiscard]] std::string directory() const;

    /// Get the version of the Engine library.
    ///
    /// \return Returns the Engine version.
    [[nodiscard]] engine_version version() const;

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

private:
    explicit engine_library(std::shared_ptr<engine_library_context> context);

    // Pimpl-like idiom, also used by other classes.
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine::v2
