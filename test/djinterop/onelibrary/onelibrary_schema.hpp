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
#include <vector>

#define ONELIBRARY_STRINGIFY(x) ONELIBRARY_STRINGIFY_(x)
#define ONELIBRARY_STRINGIFY_(x) #x

/// The schema a real OneLibrary export carries, read from the reference
/// script in `testdata/ref/onelibrary`.
///
/// The script is the record of what a device holds; see its own comments.
inline const std::vector<std::string>& onelibrary_schema_statements()
{
    static const std::vector<std::string> statements = []
    {
        const std::string path =
            std::string{ONELIBRARY_STRINGIFY(TESTDATA_DIR)} +
            "/ref/onelibrary/schema.sql";

        std::ifstream file{path};
        if (!file)
            throw std::runtime_error{"Cannot read the schema at " + path};

        std::ostringstream contents;
        contents << file.rdbuf();

        // The script is a sequence of statements separated by semicolons, and
        // nothing in it holds one in a string literal.
        std::vector<std::string> result;
        std::string statement;
        for (const auto character : contents.str())
        {
            if (character != ';')
            {
                statement += character;
                continue;
            }

            // Comment lines belong to the script, not to the statement.
            std::string stripped;
            std::istringstream lines{statement};
            for (std::string line; std::getline(lines, line);)
                if (line.rfind("--", 0) != 0)
                    stripped += line + " ";

            const auto begin = stripped.find_first_not_of(" \t\r\n");
            const auto end = stripped.find_last_not_of(" \t\r\n");
            if (begin != std::string::npos)
                result.push_back(
                    stripped.substr(begin, end - begin + 1));

            statement.clear();
        }

        return result;
    }();

    return statements;
}
