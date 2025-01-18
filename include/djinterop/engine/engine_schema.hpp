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
#ifndef DJINTEROP_ENGINE_ENGINE_SCHEMA_HPP
#define DJINTEROP_ENGINE_ENGINE_SCHEMA_HPP

#include <array>
#include <ostream>
#include <string>

#include <djinterop/config.hpp>

namespace djinterop::engine
{
/// Enumeration of Engine database schemas.
enum class DJINTEROP_PUBLIC engine_schema
{
    schema_1_6_0,
    schema_1_7_1,
    schema_1_9_1,
    schema_1_11_1,
    schema_1_13_0,
    schema_1_13_1,
    schema_1_13_2,
    schema_1_15_0,
    schema_1_17_0,
    schema_1_18_0_desktop,
    schema_1_18_0_os,

    schema_2_18_0,
    schema_2_20_1,
    schema_2_20_2,
    schema_2_20_3,
    schema_2_21_0,
    schema_2_21_1,
    schema_2_21_2,

    schema_3_0_0,
};

/// Set of supported schema versions.
constexpr std::array supported_schemas{
    engine_schema::schema_1_6_0,     engine_schema::schema_1_7_1,
    engine_schema::schema_1_9_1,     engine_schema::schema_1_11_1,
    engine_schema::schema_1_13_0,    engine_schema::schema_1_13_1,
    engine_schema::schema_1_13_2,    engine_schema::schema_1_15_0,
    engine_schema::schema_1_17_0,    engine_schema::schema_1_18_0_desktop,
    engine_schema::schema_1_18_0_os, engine_schema::schema_2_18_0,
    engine_schema::schema_2_20_1,    engine_schema::schema_2_20_2,
    engine_schema::schema_2_20_3,    engine_schema::schema_2_21_0,
    engine_schema::schema_2_21_1,    engine_schema::schema_2_21_2,
    engine_schema::schema_3_0_0};

/// Set of supported schema 1.x versions.
constexpr std::array supported_v1_schemas{
    engine_schema::schema_1_6_0,    engine_schema::schema_1_7_1,
    engine_schema::schema_1_9_1,    engine_schema::schema_1_11_1,
    engine_schema::schema_1_13_0,   engine_schema::schema_1_13_1,
    engine_schema::schema_1_13_2,   engine_schema::schema_1_15_0,
    engine_schema::schema_1_17_0,   engine_schema::schema_1_18_0_desktop,
    engine_schema::schema_1_18_0_os};

/// Set of supported schema 2.x versions.
constexpr std::array supported_v2_schemas{
    engine_schema::schema_2_18_0, engine_schema::schema_2_20_1,
    engine_schema::schema_2_20_2, engine_schema::schema_2_20_3,
    engine_schema::schema_2_21_0, engine_schema::schema_2_21_1,
    engine_schema::schema_2_21_2};

/// Set of supported schema 3.x versions.
constexpr std::array supported_v3_schemas{
    engine_schema::schema_3_0_0};

/// The most recent schema version supported by the library.
constexpr engine_schema latest_schema = engine_schema::schema_3_0_0;

/// The most recent schema 1.x version supported by the library.
constexpr engine_schema latest_v1_schema = engine_schema::schema_1_18_0_os;

/// The most recent schema 2.x version supported by the library.
constexpr engine_schema latest_v2_schema = engine_schema::schema_2_21_2;

/// The most recent schema 3.x version supported by the library.
constexpr engine_schema latest_v3_schema = engine_schema::schema_3_0_0;

/// Get a string representation of the schema.
inline std::string to_string(const engine_schema& v)
{
    switch (v)
    {
        case engine_schema::schema_1_6_0: return "1.6.0";
        case engine_schema::schema_1_7_1: return "1.7.1";
        case engine_schema::schema_1_9_1: return "1.9.1";
        case engine_schema::schema_1_11_1: return "1.11.1";
        case engine_schema::schema_1_13_0: return "1.13.0";
        case engine_schema::schema_1_13_1: return "1.13.1";
        case engine_schema::schema_1_13_2: return "1.13.2";
        case engine_schema::schema_1_15_0: return "1.15.0";
        case engine_schema::schema_1_17_0: return "1.17.0";
        case engine_schema::schema_1_18_0_desktop: return "1.18.0 (Desktop)";
        case engine_schema::schema_1_18_0_os: return "1.18.0 (OS)";
        case engine_schema::schema_2_18_0: return "2.18.0";
        case engine_schema::schema_2_20_1: return "2.20.1";
        case engine_schema::schema_2_20_2: return "2.20.2";
        case engine_schema::schema_2_20_3: return "2.20.3";
        case engine_schema::schema_2_21_0: return "2.21.0";
        case engine_schema::schema_2_21_1: return "2.21.1";
        case engine_schema::schema_2_21_2: return "2.21.2";
        case engine_schema::schema_3_0_0: return "3.0.0";
    }

    return "(unknown schema with ordinal " +
           std::to_string(static_cast<int>(v)) + ")";
}

/// Get a human-readable string informing of the Engine OS/DJ versions that
/// this schema can be used with, e.g. "Engine DJ/OS 3.1.0 to 3.4.0".
inline std::string to_application_version_string(const engine_schema& v)
{
    switch (v)
    {
        case engine_schema::schema_1_6_0: return "Engine DJ OS 1.0.0";
        case engine_schema::schema_1_7_1: return "Engine DJ OS 1.0.3";
        case engine_schema::schema_1_9_1: return "Engine Prime 1.1.1";
        case engine_schema::schema_1_11_1: return "Engine DJ OS 1.2.0";
        case engine_schema::schema_1_13_0: return "Engine DJ OS 1.2.2";
        case engine_schema::schema_1_13_1: return "Engine Prime 1.2.2";
        case engine_schema::schema_1_13_2: return "Engine DJ OS 1.3.1";
        case engine_schema::schema_1_15_0: return "Engine DJ OS 1.4.0";
        case engine_schema::schema_1_17_0: return "Engine DJ OS 1.5.1/1.5.2";
        case engine_schema::schema_1_18_0_desktop:
            return "Engine Prime 1.5.1/1.6.0/1.6.1";
        case engine_schema::schema_1_18_0_os:
            return "Engine DJ OS 1.6.0/1.6.1/1.6.2";
        case engine_schema::schema_2_18_0:
            return "Engine DJ Desktop/OS 2.0.x to 2.1.x";
        case engine_schema::schema_2_20_1:
            return "Engine DJ Desktop/OS 2.2.x to 2.3.x";
        case engine_schema::schema_2_20_2: return "Engine DJ Desktop/OS 2.4.0";
        case engine_schema::schema_2_20_3:
            return "Engine DJ Desktop/OS 3.0.0 to 3.0.1";
        case engine_schema::schema_2_21_0:
            return "Engine DJ Desktop/OS 3.1.0 to 3.4.0";
        case engine_schema::schema_2_21_1: return "Engine DJ Desktop/OS 4.0.0";
        case engine_schema::schema_2_21_2: return "Engine DJ Desktop/OS 4.0.1";
        case engine_schema::schema_3_0_0: return "Engine DJ Desktop/OS 4.1.0 to 4.2.1";
    }

    return "Engine versions unknown (" + to_string(v) + ")";
}

inline std::ostream& operator<<(std::ostream& os, const engine_schema& v)
{
    return os << to_string(v);
}

}  // namespace djinterop::engine

#endif  // DJINTEROP_ENGINE_ENGINE_SCHEMA_HPP
