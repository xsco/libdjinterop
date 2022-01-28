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

#include <sqlite_modern_cpp.h>

#include <djinterop/djinterop.hpp>

#include <djinterop/transaction_guard.hpp>

#include "engine/schema/schema.hpp"
#include "engine/v1/engine_database_impl.hpp"
#include "impl/database_impl.hpp"
#include "util.hpp"

namespace djinterop
{
database::database(const database& db) = default;

database::~database() = default;

database& database::operator=(const database& db) = default;

transaction_guard database::begin_transaction() const
{
    return pimpl_->begin_transaction();
}

stdx::optional<crate> database::crate_by_id(int64_t id) const
{
    return pimpl_->crate_by_id(id);
}

std::vector<crate> database::crates() const
{
    return pimpl_->crates();
}

std::vector<crate> database::crates_by_name(const std::string& name) const
{
    return pimpl_->crates_by_name(name);
}

crate database::create_root_crate(std::string name) const
{
    return pimpl_->create_root_crate(name);
}

track database::create_track(const track_snapshot& snapshot)
{
    return pimpl_->create_track(snapshot);
}

std::string database::directory() const
{
    return pimpl_->directory();
}

void database::verify() const
{
    pimpl_->verify();
}

void database::remove_crate(crate cr) const
{
    pimpl_->remove_crate(cr);
}

void database::remove_track(track tr) const
{
    pimpl_->remove_track(tr);
}

std::vector<crate> database::root_crates() const
{
    return pimpl_->root_crates();
}

stdx::optional<crate> database::root_crate_by_name(
    const std::string& name) const
{
    return pimpl_->root_crate_by_name(name);
}

stdx::optional<track> database::track_by_id(int64_t id) const
{
    return pimpl_->track_by_id(id);
}

std::vector<track> database::tracks() const
{
    return pimpl_->tracks();
}

std::vector<track> database::tracks_by_relative_path(
    const std::string& relative_path) const
{
    return pimpl_->tracks_by_relative_path(relative_path);
}

std::string database::uuid() const
{
    return pimpl_->uuid();
}

std::string database::version_name() const
{
    return pimpl_->version_name();
}

database::database(std::shared_ptr<database_impl> pimpl) :
    pimpl_{std::move(pimpl)}
{
}

}  // namespace djinterop
