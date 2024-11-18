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

#include <string>

#include <sqlite_modern_cpp.h>

namespace djinterop::engine
{
bool detect_is_database2(const std::string& directory);

sqlite::database create_legacy_sqlite_database(const std::string& directory);

sqlite::database create_database2_sqlite_database(const std::string& directory);

sqlite::database create_temporary_legacy_sqlite_database();

sqlite::database create_temporary_database2_sqlite_database();

sqlite::database load_legacy_sqlite_database(const std::string& directory);

sqlite::database load_database2_sqlite_database(const std::string& directory);

bool legacy_database_exists(const std::string& directory);

bool database2_database_exists(const std::string& directory);

}  // namespace djinterop::engine
