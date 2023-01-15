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
#ifndef DJINTEROP_PAD_COLOR_HPP
#define DJINTEROP_PAD_COLOR_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <cstdint>

namespace djinterop
{
/// The `pad_color` struct holds information about the color that a given
/// hot cue/loop/etc. pad may be lit up as.
struct pad_color
{
    /**
     * \brief Construct a `pad_color` with a default black color
     */
    constexpr pad_color() : r{0x00}, g{0x00}, b{0x00}, a{0x00} {}

    /**
     * \brief Construct a `pad_color` from RGBA values
     *
     * Values for each color range from 0 (full darkness), to 255 (full
     * brightness).
     */
    constexpr pad_color(
        uint_least8_t r, uint_least8_t g, uint_least8_t b, uint_least8_t a) :
        r{r},
        g{g}, b{b}, a{a}
    {
    }

    /// The red component of this pad color, from 0 to 255.
    uint_least8_t r;

    /// The green component of this pad color, from 0 to 255.
    uint_least8_t g;

    /// The blue component of this pad color, from 0 to 255.
    uint_least8_t b;

    /// The alpha component of this pad color, from 0 to 255.
    ///
    /// For most pad colors, this is usually set to full opaqueness, 255.
    uint_least8_t a;
};

inline bool operator==(const pad_color& x, const pad_color& y)
{
    return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
}

}  // namespace djinterop

#endif  // DJINTEROP_PAD_COLOR_HPP
