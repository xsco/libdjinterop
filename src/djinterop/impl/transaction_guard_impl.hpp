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

namespace djinterop
{
class transaction_guard_impl
{
public:
    transaction_guard_impl() noexcept;
    virtual ~transaction_guard_impl() noexcept;

    /// For the implementation, we guarantee that `commit()` is called at most
    /// once per object. (See `djinterop/transaction_guard.cpp` to see why.)
    virtual void commit() = 0;
};

}  // namespace djinterop
