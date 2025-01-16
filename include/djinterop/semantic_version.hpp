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
#ifndef DJINTEROP_SEMANTIC_VERSION_HPP
#define DJINTEROP_SEMANTIC_VERSION_HPP

#include <cstring>
#include <ostream>
#include <sstream>
#include <string>

#include <djinterop/config.hpp>

namespace djinterop
{
/// Simplified semantic version structure (major, minor, and patch).
struct semantic_version
{
    int maj;
    int min;
    int pat;
};

inline std::ostream& operator<<(
    std::ostream& os, const semantic_version& version)
{
    return os << version.maj << "." << version.min << "." << version.pat;
}

inline std::string to_string(const semantic_version& version)
{
    std::ostringstream oss;
    oss << version;
    return oss.str();
}

inline bool operator==(const semantic_version& a, const semantic_version& b)
{
    return a.maj == b.maj && a.min == b.min && a.pat == b.pat;
}

inline bool operator!=(const semantic_version& a, const semantic_version& b)
{
    return !(a == b);
}

inline bool operator>=(const semantic_version& a, const semantic_version& b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat >= b.pat;
}

inline bool operator>(const semantic_version& a, const semantic_version& b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat > b.pat;
}

inline bool operator<=(const semantic_version& a, const semantic_version& b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat <= b.pat;
}

inline bool operator<(const semantic_version& a, const semantic_version& b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat < b.pat;
}

}  // namespace djinterop

#endif  // DJINTEROP_SEMANTIC_VERSION_HPP
