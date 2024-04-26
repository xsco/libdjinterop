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

#include <djinterop/engine/v2/change_log_table.hpp>

#include <cassert>
#include <utility>

#include <djinterop/exceptions.hpp>

#include "engine_library_context.hpp"

namespace djinterop::engine::v2
{
change_log_table::change_log_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
    if (context_->version.schema_version >= semantic_version{2, 20, 3})
    {
        throw djinterop::unsupported_operation{
            "The ChangeLog table is removed in firmware 3.0 and above"};
    }
}

int64_t change_log_table::add(int track_id)
{
    context_->db << "INSERT INTO ChangeLog (trackId) VALUES (?)" << track_id;
    return context_->db.last_insert_rowid();
}

std::vector<change_log_row> change_log_table::all() const
{
    std::vector<change_log_row> results;
    context_->db << "SELECT id, trackId FROM ChangeLog" >>
        [&](int64_t id, int64_t track_id) {
            results.push_back(change_log_row{id, track_id});
        };

    return results;
}

std::vector<change_log_row> change_log_table::after(int64_t id) const
{
    std::vector<change_log_row> results;
    context_->db << "SELECT id, trackId FROM ChangeLog WHERE id > ?" << id >>
        [&](int64_t id, int64_t track_id) {
            results.push_back(change_log_row{id, track_id});
        };

    return results;
}

std::optional<change_log_row> change_log_table::last() const
{
    std::optional<change_log_row> result;
    context_->db
            << "SELECT id, trackId FROM ChangeLog ORDER BY id DESC LIMIT 1" >>
        [&](int64_t id, int64_t track_id)
    {
        assert(!result);
        result = change_log_row{id, track_id};
    };

    return result;
}
}  // namespace djinterop::engine::v2
