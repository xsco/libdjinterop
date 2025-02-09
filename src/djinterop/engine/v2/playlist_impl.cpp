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

#include <djinterop/exceptions.hpp>

#include "database_impl.hpp"
#include "track_impl.hpp"


namespace djinterop::engine::v2
{
playlist_impl::playlist_impl(std::shared_ptr<engine_library> library, int64_t id) :
    library_{std::move(library)},
    playlist_entity_{library_->playlist_entity()},
    playlist_{library_->playlist()},
    id_{id}
{
}

void playlist_impl::add_track_back(track tr)
{
    playlist_entity_row row{
        PLAYLIST_ENTITY_ROW_ID_NONE,
        id_,
        tr.id(),
        library_->information()
            .get()
            .uuid,  // TODO (mr-smidge): Avoid repeated DB UUID lookup.
        PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID,
        PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE};

    playlist_entity_.add_back(row);
}

void playlist_impl::add_track_after(track tr, track after)
{
    //FIXME not yet implemented
    playlist_entity_row row{
        PLAYLIST_ENTITY_ROW_ID_NONE,
        id_,
        tr.id(),
        library_->information()
            .get()
            .uuid,  // TODO (mr-smidge): Avoid repeated DB UUID lookup.
        PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID, //FIXME different value?
        PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE};

    playlist_entity_.add_back(row);
}

void playlist_impl::clear_tracks()
{
    playlist_entity_.clear(id());
}

database playlist_impl::db()
{
    return database{std::make_shared<database_impl>(library_)};
}

std::string playlist_impl::name()
{
    auto row = playlist_.get(id());
    return row->title;
}

void playlist_impl::remove_track(track tr)
{
    playlist_entity_.remove(id(), tr.id());
}

void playlist_impl::set_name(std::string name)
{
    auto row = playlist_.get(id_);
    if (!row)
    {
        throw crate_deleted{id_};
    }

    row->title = name;
    playlist_.update(*row);
}

std::vector<track> playlist_impl::tracks()
{
    std::vector<track> results;

    auto track_ids = playlist_entity_.track_ids(id_);
    for (auto&& track_id : track_ids)
    {
        results.emplace_back(std::make_shared<track_impl>(library_, track_id));
    }

    return results;
}

}  // namespace djinterop::engine::v2
