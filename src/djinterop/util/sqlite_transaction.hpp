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

#include <memory>
#include <string>
#include <utility>

#include <sqlite_modern_cpp.h>

namespace djinterop::util
{
class sqlite_transaction
{
public:
    explicit sqlite_transaction(sqlite::database db) : db_{std::move(db)}
    {
        db_ << "BEGIN TRANSACTION";
    }

    ~sqlite_transaction()
    {
        if (!committed_)
        {
            try
            {
                db_ << "ROLLBACK TRANSACTION";
            }
            catch ([[maybe_unused]] const sqlite::sqlite_exception& e)
            {
                // The exception is intentionally swallowed. An exception could
                // for example arise if SQLite performed an automatic rollback,
                // causing the explicit rollback to return an error. Such an
                // error does no harm, so we swallow it.  This is also the
                // recommendation of the SQLite documentation:
                //
                // "It is recommended that applications respond to [errors] by
                // explicitly issuing a ROLLBACK command. If the transaction has
                // already been rolled back automatically by the error response,
                // then the ROLLBACK command will fail with an error, but no
                // harm is caused by this."
            }
        }
    }

    void commit()
    {
        db_ << "COMMIT TRANSACTION";
        committed_ = true;
    }

private:
    sqlite::database db_;
    bool committed_ = false;
};

}  // namespace djinterop::util
