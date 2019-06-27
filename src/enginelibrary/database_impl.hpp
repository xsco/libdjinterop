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

#if __cplusplus < 201402L && _MSVC_LANG < 201402L
#error This library needs at least a C++14 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_DATABASE_IMPL_HPP
#define DJINTEROP_ENGINELIBRARY_DATABASE_IMPL_HPP

#include "schema.hpp"
#include "sqlite_modern_cpp.h"

namespace djinterop
{
namespace enginelibrary
{
class database_impl
{
public:
    database_impl(const std::string& directory);

    std::string directory_;
    std::string music_db_path_;
    std::string perfdata_db_path_;
    sqlite::database music_db_;
    sqlite::database perfdata_db_;
};

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_DATABASE_IMPL_HPP
