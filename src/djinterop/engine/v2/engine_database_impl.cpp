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
#include "engine_database_impl.hpp"

#include <stdexcept>

#include <djinterop/djinterop.hpp>
#include <djinterop/transaction_guard.hpp>

#include "../../util.hpp"
#include "../schema/schema.hpp"
#include "engine_storage.hpp"

namespace djinterop::engine::v2
{
engine_database_impl::engine_database_impl(std::shared_ptr<engine_storage> storage) :
    storage_{std::move(storage)}
{
}

transaction_guard engine_database_impl::begin_transaction()
{
    throw std::runtime_error{"Not implemented yet"};
}

stdx::optional<crate> engine_database_impl::crate_by_id(int64_t id)
{
    throw std::runtime_error{"Not implemented yet"};
}

std::vector<crate> engine_database_impl::crates()
{
    throw std::runtime_error{"Not implemented yet"};
}

std::vector<crate> engine_database_impl::crates_by_name(const std::string& name)
{
    throw std::runtime_error{"Not implemented yet"};
}

crate engine_database_impl::create_root_crate(std::string name)
{
    throw std::runtime_error{"Not implemented yet"};
}

track engine_database_impl::create_track(const track_snapshot& snapshot)
{
    throw std::runtime_error{"Not implemented yet"};
}

std::string engine_database_impl::directory()
{
    return storage_->directory;
}

void engine_database_impl::verify()
{
    storage_->verify();
}

void engine_database_impl::remove_crate(crate cr)
{
    throw std::runtime_error{"Not implemented yet"};
}

void engine_database_impl::remove_track(track tr)
{
    throw std::runtime_error{"Not implemented yet"};
}

std::vector<crate> engine_database_impl::root_crates()
{
    throw std::runtime_error{"Not implemented yet"};
}

stdx::optional<crate> engine_database_impl::root_crate_by_name(
    const std::string& name)
{
    throw std::runtime_error{"Not implemented yet"};
}

stdx::optional<track> engine_database_impl::track_by_id(int64_t id)
{
    throw std::runtime_error{"Not implemented yet"};
}

std::vector<track> engine_database_impl::tracks()
{
    throw std::runtime_error{"Not implemented yet"};
}

std::vector<track> engine_database_impl::tracks_by_relative_path(
    const std::string& relative_path)
{
    throw std::runtime_error{"Not implemented yet"};
}

std::string engine_database_impl::uuid()
{
    throw std::runtime_error{"Not implemented yet"};
}

std::string engine_database_impl::version_name()
{
    return storage_->version.name;
}

}  // namespace djinterop::engine::v2
