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
#ifndef DJINTEROP_ENGINE_ENGINE_VERSION_HPP
#define DJINTEROP_ENGINE_ENGINE_VERSION_HPP

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <ostream>

#include <djinterop/config.hpp>
#include <djinterop/semantic_version.hpp>

namespace djinterop::engine
{
/// Enumeration of types of Engine database.
enum class engine_database_type
{
    /// Engine database for a desktop application, such as Engine Prime.
    desktop,

    /// Engine database for a hardware device, such as DJ players.
    os,
};

/// The `engine_version` struct represents a way to identify the version
/// of an Engine database.
struct DJINTEROP_PUBLIC engine_version
{
    /// Headline version.
    semantic_version version;

    /// Engine database type (Desktop or OS).
    engine_database_type database_type;

    /// Descriptive version name.
    const char* name;

    /// Database schema version.
    semantic_version schema_version;

    /// Test whether the version has a version 2 database schema.
    ///
    /// \return Returns `true` if the version has v2 schema, or `false` if not.
    [[nodiscard]] bool is_v2_schema() const noexcept
    {
        return schema_version.maj == 2;
    }
};

inline std::ostream& operator<<(std::ostream& os, const engine_version& v)
{
    os << v.name;
    return os;
}

inline bool operator==(const engine_version& a, const engine_version& b)
{
    return a.version == b.version && a.database_type == b.database_type;
}

inline bool operator!=(const engine_version& a, const engine_version& b)
{
    return !(a == b);
}

inline bool operator>=(const engine_version& a, const engine_version& b)
{
    if (a.version == b.version)
        return a.database_type >= b.database_type;
    return a.version >= b.version;
}

inline bool operator>(const engine_version& a, const engine_version& b)
{
    if (a.version == b.version)
        return a.database_type > b.database_type;
    return a.version > b.version;
}

inline bool operator<=(const engine_version& a, const engine_version& b)
{
    if (a.version == b.version)
        return a.database_type <= b.database_type;
    return a.version <= b.version;
}

inline bool operator<(const engine_version& a, const engine_version& b)
{
    if (a.version == b.version)
        return a.database_type < b.database_type;
    return a.version < b.version;
}

}  // namespace djinterop::engine

#endif  // DJINTEROP_ENGINE_ENGINE_VERSION_HPP
