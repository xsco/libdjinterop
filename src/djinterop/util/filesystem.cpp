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

#include "filesystem.hpp"

#include <stdexcept>

#include <sys/stat.h>
#if defined(_WIN32)
#include <direct.h>
#endif

#include <djinterop/optional.hpp>

namespace djinterop::util
{
void create_dir(const std::string& directory)
{
#if defined(_WIN32)
    if (_mkdir(directory.c_str()) != 0)
#else
    if (mkdir(directory.c_str(), 0755) != 0)
#endif
    {
        throw std::runtime_error{"Failed to create directory"};
    }
}

bool path_exists(const std::string& directory)
{
    struct stat buf;
    return (stat(directory.c_str(), &buf) == 0);
}

std::string get_filename(const std::string& file_path)
{
    // TODO (haslersn): How to handle Windows path separator?
    auto slash_pos = file_path.rfind('/');  // returns -1 in case of no match
    return file_path.substr(slash_pos + 1);
}

stdx::optional<std::string> get_file_extension(const std::string& file_path)
{
    auto filename = get_filename(file_path);
    stdx::optional<std::string> file_extension;
    auto dot_pos = filename.rfind('.');
    if (dot_pos != std::string::npos)
    {
        file_extension = filename.substr(dot_pos + 1);
    }
    return file_extension;
}

}  // namespace djinterop::util
