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

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

/// Example program to demonstrate usage of the Engine v2 low-level API.
int main()
{
    std::cout << "Engine Library v2 Low-level API Example Program" << std::endl;
    std::cout << std::endl;

    auto dir = "Engine Library";
    auto exists = ev2::engine_library::exists(dir);
    auto library = exists
                       ? ev2::engine_library{dir}
                       : ev2::engine_library::create(dir, e::latest_v2_schema);

    std::cout << (exists ? "Loaded" : "Created")
              << " engine library:" << std::endl;
    std::cout << "Version: " << library.schema() << std::endl;
    std::cout << "Path:    " << dir << std::endl;
    std::cout << "UUID:    " << library.information().get().uuid << std::endl;

    return 0;
}
