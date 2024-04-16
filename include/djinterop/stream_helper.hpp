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
#ifndef DJINTEROP_STREAM_HELPER_HPP
#define DJINTEROP_STREAM_HELPER_HPP

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <array>
#include <chrono>
#include <optional>
#include <ostream>
#include <vector>

namespace djinterop::stream_helper
{
template <typename T> std::ostream& print(std::ostream& os, const T& obj)
{
    return os << obj;
}

template <typename T>
std::ostream& print(std::ostream& os, const std::optional<T>& obj)
{
    if (obj)
        return print(os, *obj);
    else
        return os << "nullopt";
}

template <typename Rep, typename Period>
std::ostream& print(
    std::ostream& os,
    const std::optional<std::chrono::duration<Rep, Period>>& obj)
{
    if (obj)
        return os << obj->count();
    else
        return os << "nullopt";
}

template <typename Rep, typename Period>
std::ostream& print(
    std::ostream& os, const std::chrono::duration<Rep, Period>& obj)
{
    return os << obj.count();
}

template <typename Clock, typename Duration>
std::ostream& print(
    std::ostream& os,
    const std::optional<std::chrono::time_point<Clock, Duration>>& obj)
{
    if (obj)
        return os << obj->time_since_epoch().count();
    else
        return os << "nullopt";
}

template <typename Clock, typename Duration>
std::ostream& print(
    std::ostream& os, const std::chrono::time_point<Clock, Duration>& obj)
{
    return os << obj.time_since_epoch().count();
}

template <typename T, std::size_t N>
std::ostream& print(std::ostream& os, const std::array<T, N>& obj)
{
    os << "[";
    for (auto iter = obj.begin(); iter != obj.end(); ++iter)
    {
        if (iter != obj.begin())
            os << ", ";

        print(os, *iter);
    }
    os << "]";
    return os;
}

template <typename T>
std::ostream& print(std::ostream& os, const std::vector<T>& obj)
{
    os << "[";
    for (auto iter = obj.begin(); iter != obj.end(); ++iter)
    {
        if (iter != obj.begin())
            os << ", ";

        print(os, *iter);
    }
    os << "]";
    return os;
}
}  // namespace djinterop::stream_helper

#endif  // DJINTEROP_STREAM_HELPER_HPP
