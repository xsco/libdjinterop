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

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/database.hpp>

namespace djinterop
{
// Artwork for tracks
class album_art
{
public:
    typedef uint_least8_t image_data_type;

    std::string hash_hex_str;
    std::vector<image_data_type> image_data;

    // TODO - implement rest of album_art class
};

}  // namespace djinterop

#endif  // DJINTEROP_ALBUM_ART_HPP
