/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_SCHEMA_HPP
#define ENGINEPRIME_SCHEMA_HPP

#include <stdexcept>
#include <string>
#include "sqlite_modern_cpp.h"

#include "engineprime/schema_version.hpp"

namespace engineprime {

bool is_supported(const schema_version &version);

schema_version verify_music_schema(sqlite::database &db);

schema_version verify_performance_schema(sqlite::database &db);

void create_music_schema(
        sqlite::database &db, const schema_version &version);

void create_performance_schema(
        sqlite::database &db, const schema_version &version);

} // engineprime

#endif // ENGINEPRIME_SCHEMA_HPP
