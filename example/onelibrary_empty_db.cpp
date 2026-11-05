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

#include <iostream>
#include <string>

#include <djinterop/onelibrary/onelibrary.hpp>

namespace onelib = djinterop::onelibrary;

/// Example program that creates an empty OneLibrary database and verifies it.
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "usage: onelibrary_empty_db <passphrase>\n";
        return 2;
    }

    const std::string passphrase = argv[1];
    const std::string dir = "OneLibrary";

    std::cout << "OneLibrary Empty Database Example Program\n" << std::endl;

    auto db = onelib::create_database(dir, passphrase);
    db.verify();

    std::cout << "Created empty OneLibrary database:" << std::endl;
    std::cout << "  Path:   " << db.directory() << std::endl;
    std::cout << "  Verified OK" << std::endl;

    return 0;
}
