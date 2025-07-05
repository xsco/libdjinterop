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

#include "chrono.hpp"

#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>

#if (__cpp_lib_chrono < 201907L)
#include <date.h>
#else
namespace date {
    using std::chrono::sys_time;
    using std::chrono::parse;
    using std::format;
}
#endif

namespace djinterop::util
{
std::chrono::system_clock::time_point parse_ft(const std::string& save)
{
    std::istringstream in{save};
    date::sys_time<std::chrono::seconds> tp;
    in >> date::parse("%F %T", tp);
    if (in.fail())
    {
        throw std::invalid_argument{
            "Cannot parse string '" + save +
            "' according to date/time format 'YYYY-MM-DD HH:MM:SS'"};
    }

    return tp;
}

std::chrono::system_clock::time_point parse_iso8601(const std::string& save)
{
    std::istringstream in{save};
    date::sys_time<std::chrono::seconds> tp;
    in >> date::parse("%FT%TZ", tp);
    if (in.fail())
    {
        in.clear();
        in.exceptions(std::ios::failbit);
        in.str(save);
        in >> date::parse("%FT%T", tp);

        if (in.fail())
        {
            throw std::invalid_argument{
                "Cannot parse string '" + save +
                "' according to ISO-8601 date/time format"};
        }
    }
    return tp;
}

std::string to_iso8601(const std::chrono::system_clock::time_point& time)
{
    return date::format("%FT%TZ", time);
}

std::string to_ft(const std::chrono::system_clock::time_point& time)
{
    return date::format("%F %T", time);  // i.e. yyyy-mm-dd hh:mm:ss
}

}  // namespace djinterop::util
