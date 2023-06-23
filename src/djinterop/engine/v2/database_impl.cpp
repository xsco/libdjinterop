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
#include "database_impl.hpp"

#include <chrono>
#include <stdexcept>

#include <djinterop/djinterop.hpp>

#include "crate_impl.hpp"
#include "track_impl.hpp"

namespace djinterop::engine::v2
{
database_impl::database_impl(std::shared_ptr<engine_library> library) :
    library_{std::move(library)}
{
}

stdx::optional<crate> database_impl::crate_by_id(int64_t id)
{
    auto exists = library_->playlist().exists(id);
    if (!exists)
    {
        return stdx::nullopt;
    }

    return std::make_optional<crate>(
        std::make_shared<crate_impl>(library_, id));
}

std::vector<crate> database_impl::crates()
{
    auto ids = library_->playlist().all_ids();
    std::vector<crate> results;

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

std::vector<crate> database_impl::crates_by_name(const std::string& name)
{
    auto ids = library_->playlist().find_ids(name);
    std::vector<crate> results;

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

crate database_impl::create_root_crate(std::string name)
{
    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        PARENT_LIST_ID_NONE,
        true,
        PLAYLIST_NO_NEXT_LIST_ID,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return crate{std::make_shared<crate_impl>(library_, id)};
}

track database_impl::create_track(const track_snapshot& snapshot)
{
    return djinterop::engine::v2::create_track(library_, snapshot);
}

std::string database_impl::directory()
{
    return library_->directory();
}

void database_impl::verify()
{
    library_->verify();
}

void database_impl::remove_crate(crate cr)
{
    library_->playlist().remove(cr.id());
}

void database_impl::remove_track(track tr)
{
    library_->track().remove(tr.id());
}

std::vector<crate> database_impl::root_crates()
{
    auto ids = library_->playlist().root_ids();
    std::vector<crate> results;

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

stdx::optional<crate> database_impl::root_crate_by_name(const std::string& name)
{
    auto id_maybe = library_->playlist().find_root_id(name);
    if (!id_maybe)
    {
        return stdx::nullopt;
    }

    return std::make_optional<crate>(
        std::make_shared<crate_impl>(library_, *id_maybe));
}

stdx::optional<track> database_impl::track_by_id(int64_t id)
{
    auto track_table = library_->track();
    if (track_table.exists(id))
    {
        return stdx::make_optional<track>(
            std::make_shared<track_impl>(library_, id));
    }

    return stdx::nullopt;
}

std::vector<track> database_impl::tracks()
{
    std::vector<track> results;
    auto track_table = library_->track();
    for (auto&& id : track_table.all_ids())
    {
        results.emplace_back(std::make_shared<track_impl>(library_, id));
    }

    return results;
}

std::vector<track> database_impl::tracks_by_relative_path(
    const std::string& relative_path)
{
    std::vector<track> results;
    auto track_table = library_->track();
    auto id_maybe = track_table.find_id_by_path(relative_path);
    if (id_maybe)
    {
        results.emplace_back(std::make_shared<track_impl>(library_, *id_maybe));
    }

    return results;
}

std::string database_impl::uuid()
{
    return library_->information().get().uuid;
}

std::string database_impl::version_name()
{
    return library_->version().name;
}

}  // namespace djinterop::engine::v2
