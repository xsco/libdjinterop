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

#include <filesystem>
#include <optional>
#include <string>

namespace djinterop::util
{
/// Converts a UTF-8 encoded path string into a `std::filesystem::path`.
///
/// Path strings are UTF-8 on all platforms.  On Windows, the narrow-string
/// constructor of `std::filesystem::path` would read them in the ANSI code
/// page instead, so any code that hands a path string to the operating system
/// must go through this function.
std::filesystem::path path_from_utf8(const std::string& path);

void create_dir(const std::string& directory);
bool path_exists(const std::string& directory);
std::string get_filename(const std::string& file_path);
std::optional<std::string> get_file_extension(const std::string& file_path);

}  // namespace djinterop::util
