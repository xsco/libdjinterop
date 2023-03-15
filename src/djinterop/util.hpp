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
#include <ostream>
#include <string>

#include <djinterop/optional.hpp>

namespace djinterop
{
void create_dir(const std::string& directory);
bool path_exists(const std::string& directory);
int64_t generate_random_int64();
std::string generate_random_uuid();
std::string get_filename(const std::string& file_path);
stdx::optional<std::string> get_file_extension(const std::string& file_path);
std::chrono::system_clock::time_point parse_iso8601(const std::string& str);

inline stdx::optional<std::chrono::system_clock::time_point> to_time_point(
    stdx::optional<int64_t> timestamp)
{
    stdx::optional<std::chrono::system_clock::time_point> result;
    if (timestamp)
    {
        result = std::chrono::system_clock::time_point{
            std::chrono::seconds(*timestamp)};
    }
    return result;
}

inline stdx::optional<int64_t> to_timestamp(
    stdx::optional<std::chrono::system_clock::time_point> time)
{
    stdx::optional<int64_t> result;
    if (time)
    {
        result = std::chrono::duration_cast<std::chrono::seconds>(
                     time->time_since_epoch())
                     .count();
    }
    return result;
}

}  // namespace djinterop
