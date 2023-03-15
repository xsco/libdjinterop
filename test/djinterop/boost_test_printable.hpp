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

#include <ostream>

#include <djinterop/stream_helper.hpp>


/// Printable type wrapper.
///
/// Boost does not know how to print things such as std::optional, and we cannot
/// add overloads for operator<< as it lives in the std namespace, which is
/// forbidden to be extended.  This thin wrapper provides appropriate support,
/// at the syntactic expense of having to wrap optional types at the time of
/// comparison.
template <typename T>
struct printable
{
    T value;
};

template <typename T>
auto make_printable(const T& v)
{
    return printable<T>{v};
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const printable<T>& obj) noexcept
{
    djinterop::stream_helper::print(os, obj);
    return os;
}

template <class T, class U>
bool operator==(const printable<T>& lhs, const printable<U>& rhs)
{
    return lhs.value == rhs.value;
}

template <class T, class U>
bool operator!=(const printable<T>& lhs, const printable<U>& rhs)
{
    return !(lhs == rhs);
}
