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

#include "util.hpp"

namespace djinterop
{
namespace enginelibrary
{
boost::string_view get_filename(boost::string_view file_path)
{
    // TODO (haslersn): How to handle Windows path separator?
    auto slash_pos = file_path.rfind('/');  // returns -1 in case of no match
    return file_path.substr(slash_pos + 1);
}

boost::optional<boost::string_view> get_file_extension(
    boost::string_view file_path)
{
    auto filename = get_filename(file_path);
    boost::optional<boost::string_view> file_extension;
    auto dot_pos = filename.rfind('.');
    if (dot_pos != boost::string_view::npos)
    {
        file_extension = filename.substr(dot_pos + 1);
    }
    return file_extension;
}

}  // namespace enginelibrary
}  // namespace djinterop
