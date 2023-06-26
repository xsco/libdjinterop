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

#include "crate_impl.hpp"
#include "track_impl.hpp"

#include <stdexcept>

#include <djinterop/exceptions.hpp>

#include "database_impl.hpp"

namespace djinterop::engine::v2
{
crate_impl::crate_impl(std::shared_ptr<engine_library> library, int64_t id) :
    djinterop::crate_impl{id}, library_{std::move(library)},
    playlist_entity_{library_->playlist_entity()}, playlist_{
                                                       library_->playlist()}
{
}

void crate_impl::add_track(int64_t track_id)
{
    playlist_entity_row row{
        PLAYLIST_ENTITY_ROW_ID_NONE,
        id(),
        track_id,
        library_->information()
            .get()
            .uuid,  // TODO (mr-smidge): Avoid repeated DB UUID lookup.
        PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID,
        PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE};

    // Crates have no fixed ordering, so tracks are added arbitrarily to the
    // end of the playlist.
    playlist_entity_.add_back(row);
}

void crate_impl::add_track(track tr)
{
    add_track(tr.id());
}

std::vector<crate> crate_impl::children()
{
    std::vector<crate> results;

    auto ids = playlist_.child_ids(id());
    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

void crate_impl::clear_tracks()
{
    playlist_entity_.clear(id());
}

crate crate_impl::create_sub_crate(std::string name)
{
    if (library_->playlist().find_id(id(), name))
    {
        throw crate_already_exists{
            "Cannot create a crate with name '" + name +
            "' under parent crate '" + this->name() +
            "', because a crate with that name already exists"};
    }

    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        id(),
        true,
        PLAYLIST_NO_NEXT_LIST_ID,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return crate{std::make_shared<crate_impl>(library_, id)};
}

database crate_impl::db()
{
    return database{std::make_shared<database_impl>(library_)};
}

std::vector<crate> crate_impl::descendants()
{
    std::vector<crate> results;

    auto ids = playlist_.descendant_ids(id());
    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

bool crate_impl::is_valid()
{
    return playlist_.exists(id());
}

std::string crate_impl::name()
{
    auto row = playlist_.get(id());
    return row->title;
}

stdx::optional<crate> crate_impl::parent()
{
    auto row = playlist_.get(id());
    if (row->parent_list_id == PARENT_LIST_ID_NONE)
    {
        return stdx::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, row->parent_list_id);
    return stdx::make_optional<crate>(crate{impl});
}

void crate_impl::remove_track(track tr)
{
    playlist_entity_.remove(id(), tr.id());
}

void crate_impl::set_name(std::string name)
{
    auto row = playlist_.get(id());
    if (!row)
    {
        throw crate_deleted{id()};
    }

    row->title = name;
    playlist_.update(*row);
}

void crate_impl::set_parent(stdx::optional<crate> parent)
{
    if (parent && parent->id() == id())
    {
        throw crate_invalid_parent{"Cannot set crate parent to itself"};
    }

    auto row = playlist_.get(id());
    if (!row)
    {
        throw crate_deleted{id()};
    }

    row->parent_list_id = parent ? parent->id() : PARENT_LIST_ID_NONE;
    playlist_.update(*row);
}

stdx::optional<crate> crate_impl::sub_crate_by_name(const std::string& name)
{
    auto id_maybe = library_->playlist().find_id(id(), name);
    if (!id_maybe)
    {
        return stdx::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, *id_maybe);
    return stdx::make_optional<crate>(crate{impl});
}

std::vector<track> crate_impl::tracks()
{
    std::vector<track> results;

    auto track_ids = playlist_entity_.track_ids(id());
    for (auto&& id : track_ids)
    {
        results.emplace_back(std::make_shared<track_impl>(library_, id));
    }

    return results;
}

}  // namespace djinterop::engine::v2
