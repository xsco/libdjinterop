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

#include <djinterop/database.hpp>
#include <djinterop/impl/track_impl.hpp>

namespace djinterop
{
track_impl::track_impl(int64_t id) noexcept : id_{id} {}

track_impl::~track_impl() noexcept = default;

int64_t track_impl::id() noexcept
{
    return id_;
}

}  // namespace djinterop
