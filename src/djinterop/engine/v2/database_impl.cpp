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
#include "playlist_impl.hpp"
#include "track_impl.hpp"

namespace djinterop::engine::v2
{
database_impl::database_impl(std::shared_ptr<engine_library> library) :
    djinterop::database_impl{
        {feature::supports_nested_crates, feature::supports_nested_playlists}},
    base_engine_impl{std::move(library)}
{
}

std::optional<crate> database_impl::crate_by_id(int64_t id)
{
    auto exists = library_->playlist().exists(id);
    if (!exists)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, id);
    return std::make_optional<crate>(crate{impl});
}

std::vector<crate> database_impl::crates()
{
    auto ids = library_->playlist().all_ids();
    std::vector<crate> results;
    results.reserve(ids.size());

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
    results.reserve(ids.size());

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

playlist database_impl::create_root_playlist(const std::string& name)
{
    if (library_->playlist().find_root_id(name))
    {
        throw playlist_already_exists{
            "Cannot create a playlist with name '" + name +
            "', because a playlist with that name already exists"};
    }

    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        PARENT_LIST_ID_NONE,
        true,
        PLAYLIST_NO_NEXT_LIST_ID,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return playlist{std::make_shared<playlist_impl>(library_, id)};
}

playlist database_impl::create_root_playlist_after(
    const std::string& name, const djinterop::playlist_impl& after_base)
{
    if (library_->playlist().find_root_id(name))
    {
        throw playlist_already_exists{
            "Cannot create a playlist with name '" + name +
            "' as a root playlist, because a playlist with that name already "
            "exists"};
    }

    const auto& after = context_cast<playlist_impl>(after_base);
    auto after_row = library_->playlist().get(after.id());
    if (after_row->parent_list_id != PARENT_LIST_ID_NONE)
    {
        throw playlist_invalid_parent{
            "Cannot create a root playlist after playlist " + after_row->title +
            ", because it is not a root playlist"};
    }

    // DB triggers will take care of massaging the next-list-id columns.  We
    // only need to work out what the new "next" list should be.
    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        PARENT_LIST_ID_NONE,
        true,
        after_row->next_list_id,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return playlist{std::make_shared<playlist_impl>(library_, id)};
}

crate database_impl::create_root_crate(const std::string& name)
{
    if (library_->playlist().find_root_id(name))
    {
        throw crate_already_exists{
            "Cannot create a crate with name '" + name +
            "' as a root crate, because a crate with that name already exists"};
    }

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

crate database_impl::create_root_crate_after(
    const std::string& name, const crate& after)
{
    if (library_->playlist().find_root_id(name))
    {
        throw crate_already_exists{
            "Cannot create a crate with name '" + name +
            "' as a root crate, because a crate with that name already exists"};
    }

    auto after_row = library_->playlist().get(after.id());
    if (after_row->parent_list_id != PARENT_LIST_ID_NONE)
    {
        throw crate_invalid_parent{
            "Cannot create a root crate after crate " + after_row->title +
            ", because it is not a root crate"};
    }

    // DB triggers will take care of massaging the next-list-id columns.  We
    // only need to work out what the new "next" list should be.
    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        PARENT_LIST_ID_NONE,
        true,
        after_row->next_list_id,
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

std::vector<playlist> database_impl::playlists()
{
    auto ids = library_->playlist().all_ids();
    std::vector<playlist> results;
    results.reserve(ids.size());

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<playlist_impl>(library_, id));
    }

    return results;
}

std::vector<playlist> database_impl::playlists_by_name(const std::string& name)
{
    auto ids = library_->playlist().find_ids(name);
    std::vector<playlist> results;
    results.reserve(ids.size());

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<playlist_impl>(library_, id));
    }

    return results;
}

void database_impl::remove_crate(crate cr)
{
    library_->playlist().remove(cr.id());
}

void database_impl::remove_playlist(const djinterop::playlist_impl& pl_base)
{
    const auto& pl = context_cast<playlist_impl>(pl_base);
    library_->playlist().remove(pl.id());
}

void database_impl::remove_track(track tr)
{
    library_->track().remove(tr.id());
}

std::vector<crate> database_impl::root_crates()
{
    auto ids = library_->playlist().root_ids();
    std::vector<crate> results;
    results.reserve(ids.size());

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

std::optional<crate> database_impl::root_crate_by_name(const std::string& name)
{
    auto id_maybe = library_->playlist().find_root_id(name);
    if (!id_maybe)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, *id_maybe);
    return std::make_optional<crate>(crate{impl});
}

std::vector<playlist> database_impl::root_playlists()
{
    auto ids = library_->playlist().root_ids();
    std::vector<playlist> results;
    results.reserve(ids.size());

    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<playlist_impl>(library_, id));
    }

    return results;
}

std::optional<playlist> database_impl::root_playlist_by_name(
    const std::string& name)
{
    auto id_maybe = library_->playlist().find_root_id(name);
    if (!id_maybe)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<playlist_impl>(library_, *id_maybe);
    return std::make_optional<playlist>(playlist{impl});
}

std::optional<track> database_impl::track_by_id(int64_t id)
{
    if (library_->track().exists(id))
    {
        auto impl = std::make_shared<track_impl>(library_, id);
        return std::make_optional<track>(track{impl});
    }

    return std::nullopt;
}

std::vector<track> database_impl::tracks()
{
    std::vector<track> results;
    for (auto&& id : library_->track().all_ids())
    {
        results.emplace_back(std::make_shared<track_impl>(library_, id));
    }

    return results;
}

std::vector<track> database_impl::tracks_by_relative_path(
    const std::string& relative_path)
{
    std::vector<track> results;
    auto id_maybe = library_->track().find_id_by_path(relative_path);
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
    return to_string(library_->schema());
}

}  // namespace djinterop::engine::v2
