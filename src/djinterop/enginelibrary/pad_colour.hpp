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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_PAD_COLOUR_HPP
#define DJINTEROP_ENGINELIBRARY_PAD_COLOUR_HPP

namespace djinterop {
namespace enginelibrary {

/**
 * The `pad_colour` struct holds information about the colour that a given
 * hot cue / loop / etc. pad on the Denon SC5000 prime deck may be lit up as.
 *
 * Note that the alpha channel is typically not used, and is usually set to
 * full brightness.
 */
struct pad_colour
{
    /**
     * \brief Construct a `pad_colour` with a default black colour
     */
    constexpr pad_colour() : r{0x00}, g{0x00}, b{0x00}, a{0x00}
    {}

    /**
     * \brief Construct a `pad_colour` from RGBA values
     *
     * Values for each colour range from 0 (full darkness), to 255 (full
     * brightness).
     */
    constexpr pad_colour(
            uint_least8_t r, uint_least8_t g, uint_least8_t b,
            uint_least8_t a) :
        r{r},
        g{g},
        b{b},
        a{a}
    {}

    /**
     * \brief The red component of this pad colour, from 0 to 255
     */
    uint_least8_t r;

    /**
     * \brief The green component of this pad colour, from 0 to 255
     */
    uint_least8_t g;

    /**
     * \brief The blue component of this pad colour, from 0 to 255
     */
    uint_least8_t b;

    /**
     * \brief The alpha component of this pad colour, from 0 to 255
     *
     * For most pad colours, this is usually set to full opaqueness, 255.
     */
    uint_least8_t a;
};

inline bool operator ==(const pad_colour &x, const pad_colour &y)
{
	return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
}

namespace standard_pad_colours
{
    constexpr pad_colour pad_1{ 0xEA, 0xC5, 0x32, 0xFF };
    constexpr pad_colour pad_2{ 0xEA, 0x8F, 0x32, 0xFF };
    constexpr pad_colour pad_3{ 0xB8, 0x55, 0xBF, 0xFF };
    constexpr pad_colour pad_4{ 0xBA, 0x2A, 0x41, 0xFF };
    constexpr pad_colour pad_5{ 0x86, 0xC6, 0x4B, 0xFF };
    constexpr pad_colour pad_6{ 0x20, 0xC6, 0x7C, 0xFF };
    constexpr pad_colour pad_7{ 0x00, 0xA8, 0xB1, 0xFF };
    constexpr pad_colour pad_8{ 0x15, 0x8E, 0xE2, 0xFF };

    const pad_colour pads[] = {
        pad_1,
        pad_2,
        pad_3,
        pad_4,
        pad_5,
        pad_6,
        pad_7,
        pad_8};
}

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_PAD_COLOUR_HPP
