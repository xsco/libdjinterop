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

#include <chrono>
#include <cstdint>
#include <string>

#include <djinterop/optional.hpp>

namespace djinterop::util
{
std::chrono::system_clock::time_point parse_ft(const std::string& str);
std::chrono::system_clock::time_point parse_iso8601(const std::string& str);
std::string to_iso8601(const std::chrono::system_clock::time_point& time);
std::string to_ft(const std::chrono::system_clock::time_point& time);

inline std::chrono::system_clock::time_point to_time_point(int64_t timestamp)
{
    return std::chrono::system_clock::time_point{
        std::chrono::seconds(timestamp)};
}

inline stdx::optional<std::chrono::system_clock::time_point> to_time_point(
    stdx::optional<int64_t> timestamp)
{
    stdx::optional<std::chrono::system_clock::time_point> result;
    if (timestamp)
        result = to_time_point(*timestamp);

    return result;
}

inline int64_t to_timestamp(std::chrono::system_clock::time_point time)
{
    return std::chrono::duration_cast<std::chrono::seconds>(
               time.time_since_epoch())
        .count();
}

inline stdx::optional<int64_t> to_timestamp(
    stdx::optional<std::chrono::system_clock::time_point> time)
{
    stdx::optional<int64_t> result;
    if (time)
        result = to_timestamp(*time);

    return result;
}

}  // namespace djinterop::util
