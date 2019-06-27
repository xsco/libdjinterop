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

#include "enginelibrary/database_impl.hpp"

namespace djinterop
{
namespace enginelibrary
{
database_impl::database_impl(const std::string& directory)
    : directory_{directory},
      music_db_path_{directory_ + "/m.db"},
      perfdata_db_path_{directory_ + "/p.db"},
      music_db_{music_db_path_},
      perfdata_db_{perfdata_db_path_}
{
    // TODO (haslersn): Should we check that directory is an absolute path?
}

}  // namespace enginelibrary
}  // namespace djinterop
