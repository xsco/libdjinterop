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

#include "engine_transaction_guard_impl.hpp"
#include "engine_storage.hpp"

namespace djinterop::engine::v1
{
engine_transaction_guard_impl::engine_transaction_guard_impl(
    std::shared_ptr<engine_storage> storage)
    : storage_{std::move(storage)}, savepoint_{++storage_->last_savepoint}
{
    // TODO (haslersn): Should el_storage::last_savepoint be atomic such that
    // this is thread-safe?
    storage_->db << ("SAVEPOINT s" + std::to_string(savepoint_));
}

engine_transaction_guard_impl::~engine_transaction_guard_impl()
{
    if (savepoint_ != 0)
    {
        try
        {
            storage_->db << ("ROLLBACK TO s" + std::to_string(savepoint_));
        }
        catch (...)
        {
            // The exception is intentionally swallowed. An exception could for
            // example arise if SQLite performed an automatic rollback, causing
            // the explicit rollback to return an error. Such an error does no
            // harm, so we swallow it.
            //
            // TODO (haslersn): We could still issue a warning
        }
    }
}

void engine_transaction_guard_impl::commit()
{
    auto savepoint = savepoint_;
    savepoint_ = 0;
    storage_->db << ("RELEASE s" + std::to_string(savepoint));
}

}  // namespace djinterop::engine::v1
