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

#include <sqlite_modern_cpp.h>

namespace djinterop
{
namespace enginelibrary
{
void verify_music_schema_1_7_1(sqlite::database &db);

void verify_performance_schema_1_7_1(sqlite::database &db);

void create_music_schema_1_7_1(sqlite::database &db);

void create_performance_schema_1_7_1(sqlite::database &db);

}  // namespace enginelibrary
}  // namespace djinterop
