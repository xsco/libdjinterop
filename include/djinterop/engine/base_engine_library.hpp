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

#include <memory>
#include <string>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine_schema.hpp>

namespace djinterop::engine
{
struct engine_library_context;

/// Abstract base class for an Engine Library.
///
class DJINTEROP_PUBLIC base_engine_library
{
public:
    /// Construct an instance of the class using an Engine library context.
    ///
    /// \param context Engine library context.
    explicit base_engine_library(
        std::shared_ptr<engine_library_context> context);

    virtual ~base_engine_library() = default;

    /// Verify the correctness of the Engine library database schema for its
    /// stated version.
    void verify() const;

    /// Get the Engine library top-level directory.
    ///
    /// \return Returns the top-level directory.
    [[nodiscard]] std::string directory() const;

    /// Get the schema version of the Engine library.
    ///
    /// \return Returns the Engine schema version.
    [[nodiscard]] engine_schema schema() const;

    /// Get the unified database interface for this Engine library.
    [[nodiscard]] virtual djinterop::database database() const = 0;

protected:
    static std::shared_ptr<engine_library_context> create(
        const std::string& directory, const engine_schema& schema);

    static std::shared_ptr<engine_library_context> create_temporary(
        const engine_schema& schema);

    static bool exists(const std::string& directory);

    static std::shared_ptr<engine_library_context> load(
        const std::string& directory);

    // Pimpl-like idiom, also used by other classes.
    std::shared_ptr<engine_library_context> context_;
};

}  // namespace djinterop::engine
