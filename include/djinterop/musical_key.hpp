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
#ifndef DJINTEROP_MUSICAL_KEY_HPP
#define DJINTEROP_MUSICAL_KEY_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <ostream>

namespace djinterop
{
/**
 * The `musical_key` enumeration contains a list all known musical keys that a
 * track may be detected to initially follow.
 */
enum class musical_key
{
    c_major = 0,
    a_minor,
    g_major,
    e_minor,
    d_major,
    b_minor,
    a_major,
    f_sharp_minor,
    e_major,
    d_flat_minor,
    b_major,
    a_flat_minor,
    f_sharp_major,
    e_flat_minor,
    d_flat_major,
    b_flat_minor,
    a_flat_major,
    f_minor,
    e_flat_major,
    c_minor,
    b_flat_major,
    g_minor,
    f_major,
    d_minor,
};

inline std::ostream& operator<<(
    std::ostream& os, const djinterop::musical_key& obj) noexcept
{
    switch (obj)
    {
        case musical_key::c_major: os << "C"; break;
        case musical_key::a_minor: os << "Am"; break;
        case musical_key::g_major: os << "G"; break;
        case musical_key::e_minor: os << "Em"; break;
        case musical_key::d_major: os << "D"; break;
        case musical_key::b_minor: os << "Bm"; break;
        case musical_key::a_major: os << "A"; break;
        case musical_key::f_sharp_minor: os << "F♯m"; break;
        case musical_key::e_major: os << "E"; break;
        case musical_key::d_flat_minor: os << "D♭m"; break;
        case musical_key::b_major: os << "B"; break;
        case musical_key::a_flat_minor: os << "A♭m"; break;
        case musical_key::f_sharp_major: os << "F♯"; break;
        case musical_key::e_flat_minor: os << "E♭m"; break;
        case musical_key::d_flat_major: os << "D♭m"; break;
        case musical_key::b_flat_minor: os << "B♭m"; break;
        case musical_key::a_flat_major: os << "A♭"; break;
        case musical_key::f_minor: os << "F"; break;
        case musical_key::e_flat_major: os << "E♭"; break;
        case musical_key::c_minor: os << "Cm"; break;
        case musical_key::b_flat_major: os << "B♭"; break;
        case musical_key::g_minor: os << "Gm"; break;
        case musical_key::f_major: os << "F"; break;
        case musical_key::d_minor: os << "Dm"; break;
    }

    return os;
}

}  // namespace djinterop

#endif  // DJINTEROP_MUSICAL_KEY_HPP
