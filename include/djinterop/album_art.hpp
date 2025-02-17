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
#ifndef DJINTEROP_ALBUM_ART_HPP
#define DJINTEROP_ALBUM_ART_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop
{
// Artwork for tracks
class DJINTEROP_PUBLIC album_art
{
public:
    using image_data_type = uint_least8_t;

    std::string hash_hex_str;
    std::vector<image_data_type> image_data;

    // TODO - implement rest of album_art class
};

}  // namespace djinterop

#endif  // DJINTEROP_ALBUM_ART_HPP
