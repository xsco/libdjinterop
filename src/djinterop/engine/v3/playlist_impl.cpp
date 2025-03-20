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

namespace djinterop::engine::v3
{
playlist_impl::playlist_impl(
    std::shared_ptr<engine_library> library, int64_t id) :
    base_engine_impl{std::move(library)}, id_{id}
{
}

void playlist_impl::add_track_back(const djinterop::track_impl& tr)
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

    library_->playlist_entity().add(row, true);
}

void playlist_impl::add_track_after(
    const djinterop::track_impl& tr, const djinterop::track_impl& after)
{
    auto after_row = library_->playlist_entity().get(id_, after.id());
    if (!after_row)
    {
        throw track_deleted{after.id()};
    }

    if (after_row->list_id != id_)
    {
        throw playlist_invalid_parent{
            "Cannot add a track to playlist " + std::to_string(id_) +
            " after track " + std::to_string(after.id()) +
            " because it does not belong to this playlist"};
    }

    playlist_entity_row row{
        PLAYLIST_ENTITY_ROW_ID_NONE,
        id_,
        tr.id(),
        library_->information()
            .get()
            .uuid,  // TODO (mr-smidge): Avoid repeated DB UUID lookup.
        after_row->next_entity_id,
        PLAYLIST_ENTITY_DEFAULT_MEMBERSHIP_REFERENCE};

    library_->playlist_entity().add(row, true);
}

std::vector<playlist> playlist_impl::children()
{
    std::vector<playlist> results;

    auto ids = library_->playlist().child_ids(id_);
    for (auto&& id : ids)
    {
        results.emplace_back(std::make_shared<playlist_impl>(library_, id));
    }

    return results;
}

void playlist_impl::clear_tracks()
{
    library_->playlist_entity().clear(id_);
}

playlist playlist_impl::create_sub_playlist(const std::string& name)
{
    if (library_->playlist().find_id(id_, name))
    {
        throw playlist_already_exists{
            "Cannot create a playlist with name '" + name +
            "' under parent playlist '" + this->name() +
            "', because a playlist with that name already exists"};
    }

    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        id_,
        true,
        PLAYLIST_NO_NEXT_LIST_ID,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return playlist{std::make_shared<playlist_impl>(library_, id)};
}

playlist playlist_impl::create_sub_playlist_after(
    const std::string& name, const djinterop::playlist_impl& after_base)
{
    const auto& after = context_cast<playlist_impl>(after_base);

    if (library_->playlist().find_id(id_, name))
    {
        throw playlist_already_exists{
            "Cannot create a playlist with name '" + name +
            "' under parent playlist '" + this->name() +
            "', because a playlist with that name already exists"};
    }

    auto after_row = library_->playlist().get(after.id_);
    if (after_row->parent_list_id != id_)
    {
        throw playlist_invalid_parent{
            "Cannot create a playlist under parent playlist " + this->name() +
            " after playlist " + after_row->title +
            ", because it resides under a different parent playlist"};
    }

    // DB triggers will take care of massaging the next-list-id columns.  We
    // only need to work out what the new "next" list should be.
    playlist_row row{
        PLAYLIST_ROW_ID_NONE,
        name,
        id_,
        true,
        after_row->next_list_id,
        std::chrono::system_clock::now(),
        true};

    auto id = library_->playlist().add(row);
    return playlist{std::make_shared<playlist_impl>(library_, id)};
}

database playlist_impl::db() const
{
    return database{std::make_shared<database_impl>(library_)};
}

std::string playlist_impl::name() const
{
    auto row = library_->playlist().get(id_);
    return row->title;
}

std::optional<playlist> playlist_impl::parent()
{
    auto row = library_->playlist().get(id_);
    if (row->parent_list_id == PARENT_LIST_ID_NONE)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<playlist_impl>(library_, row->parent_list_id);
    return std::make_optional<playlist>(playlist{impl});
}

void playlist_impl::remove_track(const djinterop::track_impl& tr)
{
    library_->playlist_entity().remove(id_, tr.id());
}

void playlist_impl::set_name(const std::string& name)
{
    auto row = library_->playlist().get(id_);
    if (!row)
    {
        throw playlist_deleted{id_};
    }

    row->title = name;
    try
    {
        library_->playlist().update(*row);
    }
    catch (const playlist_row_invalid_title& e)
    {
        throw playlist_invalid_name{std::string{e.what()}, name};
    }
}

void playlist_impl::set_parent(
    const djinterop::playlist_impl* parent_base_maybe)
{
    const auto* parent_maybe =
        context_cast_maybe<playlist_impl>(parent_base_maybe);
    if (parent_maybe && parent_maybe->id_ == id_)
    {
        throw playlist_invalid_parent{"Cannot set playlist parent to itself"};
    }

    auto row = library_->playlist().get(id_);
    if (!row)
    {
        throw playlist_deleted{id_};
    }

    row->parent_list_id =
        parent_maybe ? parent_maybe->id_ : PARENT_LIST_ID_NONE;
    library_->playlist().update(*row);
}

std::optional<playlist> playlist_impl::sub_playlist_by_name(
    const std::string& name)
{
    auto id_maybe = library_->playlist().find_id(id_, name);
    if (!id_maybe)
    {
        return std::nullopt;
    }

    auto impl = std::make_shared<playlist_impl>(library_, *id_maybe);
    return std::make_optional<playlist>(playlist{impl});
}

std::vector<track> playlist_impl::tracks() const
{
    std::vector<track> results;

    auto track_ids = library_->playlist_entity().track_ids(id_);
    results.reserve(track_ids.size());
    for (auto&& track_id : track_ids)
    {
        results.emplace_back(std::make_shared<track_impl>(library_, track_id));
    }

    return results;
}

}  // namespace djinterop::engine::v3
