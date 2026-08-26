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

#include "playlist_impl.hpp"

#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>

#include "database_impl.hpp"
#include "playlist_table.hpp"
#include "track_impl.hpp"

namespace djinterop::onelibrary
{
playlist_impl::playlist_impl(
    std::shared_ptr<onelibrary_context> context, int64_t id) :
    context_{std::move(context)}, id_{id}
{
}

playlist make_playlist(std::shared_ptr<onelibrary_context> context, int64_t id)
{
    return playlist{std::make_shared<playlist_impl>(std::move(context), id)};
}

std::string playlist_impl::name() const
{
    const auto row = playlist_table{context_}.get(id_);
    if (!row)
        throw playlist_deleted{id_};

    return row->name;
}

std::optional<playlist> playlist_impl::parent()
{
    const auto row = playlist_table{context_}.get(id_);
    if (!row)
        throw playlist_deleted{id_};

    if (!row->parent_id)
        return std::nullopt;

    return make_playlist(context_, *row->parent_id);
}

std::vector<playlist> playlist_impl::children()
{
    std::vector<playlist> results;
    for (const auto& child : playlist_table{context_}.child_ids(id_))
        results.push_back(make_playlist(context_, child));

    return results;
}

std::optional<playlist> playlist_impl::sub_playlist_by_name(
    const std::string& name)
{
    const auto child = playlist_table{context_}.find_child(id_, name);
    if (!child)
        return std::nullopt;

    return make_playlist(context_, *child);
}

std::vector<track> playlist_impl::tracks() const
{
    std::vector<track> results;
    for (const auto& track_id : playlist_table{context_}.track_ids(id_))
        results.push_back(
            track{std::make_shared<track_impl>(context_, track_id)});

    return results;
}

database playlist_impl::db() const
{
    return database{std::make_shared<database_impl>(context_)};
}

bool playlist_impl::operator==(const djinterop::playlist_impl& other) const
{
    const auto* other_impl = dynamic_cast<const playlist_impl*>(&other);
    return other_impl != nullptr && other_impl->context_ == context_ &&
           other_impl->id_ == id_;
}

void playlist_impl::add_track_back(const djinterop::track_impl&)
{
    read_only();
}

void playlist_impl::add_track_after(
    const djinterop::track_impl&, const djinterop::track_impl&)
{
    read_only();
}

void playlist_impl::clear_tracks()
{
    read_only();
}

playlist playlist_impl::create_sub_playlist(const std::string&)
{
    read_only();
}

playlist playlist_impl::create_sub_playlist_after(
    const std::string&, const djinterop::playlist_impl&)
{
    read_only();
}

void playlist_impl::remove_track(const djinterop::track_impl&)
{
    read_only();
}

void playlist_impl::set_name(const std::string&)
{
    read_only();
}

void playlist_impl::set_parent(const djinterop::playlist_impl*)
{
    read_only();
}

}  // namespace djinterop::onelibrary
