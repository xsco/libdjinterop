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

#include <array>
#include <memory>

#include <djinterop/impl/transaction_guard_impl.hpp>

namespace djinterop
{
namespace enginelibrary
{
class el_storage;

class el_transaction_guard_impl : public transaction_guard_impl
{
public:
    el_transaction_guard_impl(std::shared_ptr<el_storage> storage_);
    ~el_transaction_guard_impl();
    void commit() override;

private:
    std::shared_ptr<el_storage> storage_;
    int64_t savepoint_;
};

}  // namespace enginelibrary
}  // namespace djinterop