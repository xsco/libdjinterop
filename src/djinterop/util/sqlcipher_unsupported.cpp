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

#include "sqlcipher.hpp"

#include <djinterop/exceptions.hpp>

namespace djinterop::util
{
sqlite::database open_encrypted_database(
    const std::string& path, const std::string&)
{
    throw unsupported_database{
        "This build of libdjinterop cannot read the encrypted database `" +
        path + "`, as it was built without EXPERIMENTAL_ENABLE_SQLCIPHER"};
}

}  // namespace djinterop::util
