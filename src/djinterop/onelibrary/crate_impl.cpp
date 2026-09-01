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

#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>

#include "database_impl.hpp"
#include "playlist_table.hpp"
#include "track_impl.hpp"

namespace djinterop::onelibrary
{
crate_impl::crate_impl(
    std::shared_ptr<onelibrary_context> context, int64_t id) :
    djinterop::crate_impl{id}, context_{std::move(context)}
{
}

crate make_crate(std::shared_ptr<onelibrary_context> context, int64_t id)
{
    return crate{std::make_shared<crate_impl>(std::move(context), id)};
}

bool crate_impl::is_valid()
{
    return playlist_table{context_}.exists(id());
}

std::string crate_impl::name()
{
    const auto row = playlist_table{context_}.get(id());
    if (!row)
        throw crate_deleted{id()};

    return row->name;
}

std::optional<crate> crate_impl::parent()
{
    const auto row = playlist_table{context_}.get(id());
    if (!row)
        throw crate_deleted{id()};

    if (!row->parent_id)
        return std::nullopt;

    return make_crate(context_, *row->parent_id);
}

std::vector<crate> crate_impl::children()
{
    std::vector<crate> results;
    for (const auto& child : playlist_table{context_}.child_ids(id()))
        results.push_back(make_crate(context_, child));

    return results;
}

std::vector<crate> crate_impl::descendants()
{
    std::vector<crate> results;
    for (const auto& descendant : playlist_table{context_}.descendant_ids(id()))
        results.push_back(make_crate(context_, descendant));

    return results;
}

std::optional<crate> crate_impl::sub_crate_by_name(const std::string& name)
{
    const auto child = playlist_table{context_}.find_child(id(), name);
    if (!child)
        return std::nullopt;

    return make_crate(context_, *child);
}

std::vector<track> crate_impl::tracks()
{
    std::vector<track> results;
    for (const auto& track_id : playlist_table{context_}.track_ids(id()))
        results.push_back(
            track{std::make_shared<track_impl>(context_, track_id)});

    return results;
}

database crate_impl::db()
{
    return database{std::make_shared<database_impl>(context_)};
}

void crate_impl::add_track(int64_t)
{
    read_only();
}

void crate_impl::add_track(track)
{
    read_only();
}

void crate_impl::clear_tracks()
{
    read_only();
}

crate crate_impl::create_sub_crate(const std::string&)
{
    read_only();
}

crate crate_impl::create_sub_crate_after(const std::string&, const crate&)
{
    read_only();
}

void crate_impl::remove_track(track)
{
    read_only();
}

void crate_impl::set_name(std::string)
{
    read_only();
}

void crate_impl::set_parent(std::optional<crate>)
{
    read_only();
}

}  // namespace djinterop::onelibrary
