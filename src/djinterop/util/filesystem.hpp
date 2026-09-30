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
/// Returns a path as a UTF-8 encoded string, on all platforms.
///
/// Use this for anything handed to SQLite or put in a message.
/// `std::filesystem::path::string()` returns the ANSI code page on Windows.
std::string path_to_utf8(const std::filesystem::path& path);

std::string get_filename(const std::string& file_path);
std::optional<std::string> get_file_extension(const std::string& file_path);

}  // namespace djinterop::util
