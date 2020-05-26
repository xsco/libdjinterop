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

#include <djinterop/impl/transaction_guard_impl.hpp>
#include <djinterop/transaction_guard.hpp>

#include <stdexcept>

namespace djinterop
{
transaction_guard::transaction_guard() noexcept = default;

transaction_guard::transaction_guard(transaction_guard&& other) noexcept =
    default;

transaction_guard& transaction_guard::operator=(
    transaction_guard&& other) noexcept = default;

transaction_guard::~transaction_guard() = default;

transaction_guard::operator bool() noexcept
{
    return static_cast<bool>(pimpl_);
}

void transaction_guard::commit()
{
    if (pimpl_ == nullptr)
    {
        // TODO (haslersn): Consider a custom exception type
        throw std::logic_error{"Called commit on an empty transaction_guard"};
    }
    // Move to a temporary such that there can only be one attempt to commit
    auto temp = std::move(pimpl_);
    temp->commit();
}

void transaction_guard::rollback()
{
    if (pimpl_ == nullptr)
    {
        // TODO (haslersn): Consider a custom exception type
        throw std::logic_error{"Called rollback on an empty transaction_guard"};
    }
    // The destructor of `transaction_guard_impl` performs the rollback
    pimpl_ = nullptr;
}

transaction_guard::transaction_guard(
    std::unique_ptr<transaction_guard_impl> pimpl) noexcept
    : pimpl_{std::move(pimpl)}
{
}

}  // namespace djinterop
