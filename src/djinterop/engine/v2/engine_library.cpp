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

#include <djinterop/engine/v2/engine_library.hpp>

#include <memory>

#include <djinterop/database.hpp>

#include "database_impl.hpp"

namespace djinterop::engine::v2
{
database engine_library::database() const
{
    auto library = std::make_shared<engine_library>(*this);
    auto pimpl = std::make_shared<database_impl>(library);
    return djinterop::database{pimpl};
}

}  // namespace djinterop::engine::v2
