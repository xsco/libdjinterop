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

namespace djinterop::engine::v3
{
crate_impl::crate_impl(std::shared_ptr<engine_library> library, int64_t id) :
    djinterop::crate_impl{id}, library_{std::move(library)}
{
}

void crate_impl::add_track(int64_t track_id)
{
    // Crates have no fixed ordering, so tracks are added arbitrarily to the
    // end of the playlist.
    int64_t next_entity_id = PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID;

    playlist_entity_row row{
        PLAYLIST_ENTITY_ROW_ID_NONE,
        id(),
        track_id,
        library_->information()
            .get()
            .uuid,  // TODO (mr-smidge): Avoid repeated DB UUID lookup.
        next_entity_id,
        PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE};

    library_->playlist_entity().add(row);
}

void crate_impl::add_track(track tr)
{
    add_track(tr.id());
}

std::vector<crate> crate_impl::children()
{
    std::vector<crate> results;

    auto ids = library_->playlist().child_ids(id());
    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

void crate_impl::clear_tracks()
{
    library_->playlist_entity().clear(id());
}

crate crate_impl::create_sub_crate(const std::string& name)
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

crate crate_impl::create_sub_crate_after(
    const std::string& name, const crate& after)
{
    if (library_->playlist().find_id(id(), name))
    {
        throw crate_already_exists{
            "Cannot create a crate with name '" + name +
            "' under parent crate '" + this->name() +
            "', because a crate with that name already exists"};
    }

    auto after_row = library_->playlist().get(after.id());
    if (after_row->parent_list_id != id())
    {
        throw crate_invalid_parent{
            "Cannot create a crate under parent crate " + this->name() +
            " after crate " + after_row->title +
            ", because it resides under a different parent crate"};
    }

    // DB triggers will take care of massaging the next-list-id columns.  We
    // only need to work out what the new "next" list should be.
    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        id(),
        true,
        after_row->next_list_id,
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

    auto ids = library_->playlist().descendant_ids(id());
    results.reserve(ids.size());
    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<crate_impl>(library_, id));
    }

    return results;
}

bool crate_impl::is_valid()
{
    return library_->playlist().exists(id());
}

std::string crate_impl::name()
{
    auto row = library_->playlist().get(id());
    return row->title;
}

std::optional<crate> crate_impl::parent()
{
    auto row = library_->playlist().get(id());
    if (row->parent_list_id == PARENT_LIST_ID_NONE)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, row->parent_list_id);
    return std::make_optional<crate>(crate{impl});
}

void crate_impl::remove_track(track tr)
{
    library_->playlist_entity().remove(id(), tr.id());
}

void crate_impl::set_name(std::string name)
{
    auto row = library_->playlist().get(id());
    if (!row)
    {
        throw crate_deleted{id()};
    }

    row->title = name;
    try
    {
        library_->playlist().update(*row);
    }
    catch (const playlist_row_invalid_title& e)
    {
        throw crate_invalid_name{std::string{e.what()}, name};
    }
}

void crate_impl::set_parent(std::optional<crate> parent)
{
    if (parent && parent->id() == id())
    {
        throw crate_invalid_parent{"Cannot set crate parent to itself"};
    }

    auto row = library_->playlist().get(id());
    if (!row)
    {
        throw crate_deleted{id()};
    }

    row->parent_list_id = parent ? parent->id() : PARENT_LIST_ID_NONE;
    library_->playlist().update(*row);
}

std::optional<crate> crate_impl::sub_crate_by_name(const std::string& name)
{
    auto id_maybe = library_->playlist().find_id(id(), name);
    if (!id_maybe)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<crate_impl>(library_, *id_maybe);
    return std::make_optional<crate>(crate{impl});
}

std::vector<track> crate_impl::tracks()
{
    std::vector<track> results;

    auto track_ids = library_->playlist_entity().track_ids(id());
    results.reserve(track_ids.size());
    for (auto&& id : track_ids)
    {
        results.emplace_back(std::make_shared<track_impl>(library_, id));
    }

    return results;
}

}  // namespace djinterop::engine::v3
