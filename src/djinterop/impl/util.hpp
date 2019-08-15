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

#include <boost/optional.hpp>
#include <djinterop/optional/optional.hpp>

namespace djinterop
{
std::string get_filename(const std::string& file_path);
boost::optional<std::string> get_file_extension(const std::string& file_path);

template <typename T>
std::experimental::optional<T> from_boost_optional(boost::optional<T> opt)
{
    return opt
        ? std::experimental::optional<T>{*opt}
        : std::experimental::nullopt;
}

template <typename T>
boost::optional<T> to_boost_optional(std::experimental::optional<T> opt)
{
    return opt
        ? boost::optional<T>{*opt}
        : boost::none;
}

}  // namespace djinterop
