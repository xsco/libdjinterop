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
#ifndef DJINTEROP_TRANSACTION_GUARD_HPP
#define DJINTEROP_TRANSACTION_GUARD_HPP

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <memory>

namespace djinterop
{
class transaction_guard_impl;

class transaction_guard
{
public:
    transaction_guard() noexcept;

    transaction_guard(transaction_guard&& other) noexcept;

    transaction_guard& operator=(transaction_guard&& other) noexcept;

    ~transaction_guard();

    explicit operator bool() noexcept;

    void commit();

    void rollback();

    // TODO (haslersn): non public?
    transaction_guard(std::unique_ptr<transaction_guard_impl> pimpl) noexcept;

private:
    std::unique_ptr<transaction_guard_impl> pimpl_;
};

}  // namespace djinterop

#endif
