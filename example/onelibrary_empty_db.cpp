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

#include <djinterop/onelibrary/onelibrary.hpp>

namespace onelib = djinterop::onelibrary;

/// Example program that creates an empty OneLibrary database and verifies it.
int main()
{
    auto dir = "OneLibrary";

    std::cout << "OneLibrary Empty Database Example Program\n" << std::endl;

    auto library = onelib::onelibrary::create(dir);
    library.verify();

    std::cout << "Created empty OneLibrary database:" << std::endl;
    std::cout << "  Path:   " << library.directory() << std::endl;
    std::cout << "  Verified OK" << std::endl;

    return 0;
}
