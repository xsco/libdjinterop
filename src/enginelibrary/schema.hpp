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

#if __cplusplus < 201402L && _MSVC_LANG < 201402L
  #error This library needs at least a C++14 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_SCHEMA_HPP
#define DJINTEROP_ENGINELIBRARY_SCHEMA_HPP

#include <stdexcept>
#include <string>
#include "sqlite_modern_cpp.h"

#include "djinterop/enginelibrary/schema_version.hpp"

namespace djinterop {
namespace enginelibrary {

bool is_supported(const schema_version &version);

schema_version verify_music_schema(sqlite::database &db);

schema_version verify_performance_schema(sqlite::database &db);

void create_music_schema(
        sqlite::database &db, const schema_version &version);

void create_performance_schema(
        sqlite::database &db, const schema_version &version);

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_SCHEMA_HPP
