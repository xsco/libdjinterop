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

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <sqlite3.h>

#define ONELIBRARY_STRINGIFY(x) ONELIBRARY_STRINGIFY_(x)
#define ONELIBRARY_STRINGIFY_(x) #x

/// Create the tables a real OneLibrary export carries, from the reference
/// script in `testdata/ref/onelibrary`.
///
/// The script is the record of what a device holds; see its own comments.
inline void create_onelibrary_schema(sqlite3* db)
{
    const std::string path = std::string{ONELIBRARY_STRINGIFY(TESTDATA_DIR)} +
                             "/ref/onelibrary/schema.sql";

    std::ifstream file{path};
    if (!file)
        throw std::runtime_error{"Cannot read the schema at " + path};

    std::ostringstream script;
    script << file.rdbuf();

    char* error = nullptr;
    const auto rc =
        sqlite3_exec(db, script.str().c_str(), nullptr, nullptr, &error);
    const std::string message = error != nullptr ? error : "";
    sqlite3_free(error);
    if (rc != SQLITE_OK)
        throw std::runtime_error{"Cannot create the schema: " + message};
}
