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

#include <string>

#include <djinterop/optional.hpp>

namespace djinterop::util
{
void create_dir(const std::string& directory);
bool path_exists(const std::string& directory);
std::string get_filename(const std::string& file_path);
stdx::optional<std::string> get_file_extension(const std::string& file_path);

}  // namespace djinterop::util
