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

#include "../../util/sqlite_transaction.hpp"
#include "engine_database_impl.hpp"
#include "engine_playlist_impl.hpp"
#include "engine_storage.hpp"
#include "engine_track_impl.hpp"

namespace djinterop::engine::v1
{
namespace
{
void ensure_valid_playlist_name(const std::string& name)
{
    if (name == "")
    {
        throw djinterop::playlist_invalid_name{
            "Playlist names must be non-empty", name};
    }
    else if (name.find_first_of(';') != std::string::npos)
    {
        throw djinterop::playlist_invalid_name{
            "Playlist names must not contain semicolons", name};
    }
}

}  // namespace

engine_playlist_impl::engine_playlist_impl(
    std::shared_ptr<engine_storage> storage, int64_t id) :
    storage_{std::move(storage)}, id_{id}
{
}

void engine_playlist_impl::add_track_back(const djinterop::track_impl& tr)
{
    std::string uuid;
    storage_->db << "SELECT uuid FROM Information" >> uuid;
    int64_t next_track_number = 0;
    storage_->db << "SELECT MAX(trackNumber) + 1 FROM PlaylistTrackList WHERE "
                    "playlistId = ?"
                 << id() >>
        next_track_number;
    storage_->db
        << "INSERT INTO PlaylistTrackList (playlistId, trackId, "
           "trackIdInOriginDatabase, databaseUuid, trackNumber) VALUES "
           "(?, ?, ?, ?, ?)"
        << id() << tr.id() << tr.id() << uuid << next_track_number;
}

void engine_playlist_impl::add_track_after(
    const djinterop::track_impl& tr, const djinterop::track_impl& after)
{
    std::string uuid;
    storage_->db << "SELECT uuid FROM Information" >> uuid;

    djinterop::util::sqlite_transaction trans{storage_->db};

    std::optional<int64_t> after_track_number{};
    storage_->db
            << "SELECT trackNumber FROM PlaylistTrackList WHERE playlistId = ? "
               "AND trackId = ? ORDER BY trackNumber LIMIT 1"
            << id() << after.id() >>
        [&after_track_number](int64_t track_number)
    { after_track_number = track_number; };

    if (!after_track_number)
    {
        throw track_not_in_playlist{
            "The provided track to add after is not in the playlist"};
    }

    storage_->db << "UPDATE PlaylistTrackList SET trackNumber = trackNumber + "
                    "1 WHERE playlistId = ? AND trackNumber > ?"
                 << id() << after_track_number;

    storage_->db
        << "INSERT INTO PlaylistTrackList (playlistId, trackId, "
           "trackIdInOriginDatabase, databaseUuid, trackNumber) VALUES "
           "(?, ?, ?, ?, ?)"
        << id() << tr.id() << tr.id() << uuid
        << (after_track_number.value() + 1);

    trans.commit();
}

std::vector<playlist> engine_playlist_impl::children()
{
    throw unsupported_operation{
        "Engine V1 databases do not support nested playlists"};
}

void engine_playlist_impl::clear_tracks()
{
    storage_->db << "DELETE FROM PlaylistTrackList WHERE playlistId = ?"
                 << id();
}

playlist engine_playlist_impl::create_sub_playlist(const std::string& name)
{
    throw unsupported_operation{
        "Engine V1 databases do not support nested playlists"};
}

playlist engine_playlist_impl::create_sub_playlist_after(
    const std::string& name, const djinterop::playlist_impl& after_base)
{
    throw unsupported_operation{
        "Engine V1 databases do not support nested playlists"};
}

database engine_playlist_impl::db() const
{
    return database{std::make_shared<engine_database_impl>(storage_)};
}

std::string engine_playlist_impl::name() const
{
    std::optional<std::string> name;
    storage_->db << "SELECT title FROM Playlist WHERE id = ?" << id() >>
        [&](std::string title)
    {
        if (!name)
        {
            name = std::move(title);
        }
        else
        {
            throw playlist_database_inconsistency{
                "More than one playlist with the same ID " +
                std::to_string(id())};
        }
    };
    if (!name)
    {
        throw playlist_deleted{id()};
    }
    return *name;
}

std::optional<playlist> engine_playlist_impl::parent()
{
    return {};
}

void engine_playlist_impl::remove_track(const djinterop::track_impl& tr)
{
    storage_->db
        << "DELETE FROM PlaylistTrackList WHERE playlistId = ? AND trackId = ?"
        << id() << tr.id();
}

void engine_playlist_impl::set_name(const std::string& name)
{
    ensure_valid_playlist_name(name);
    storage_->db << "UPDATE Playlist SET title = ? WHERE id = ?" << name
                 << id();
}

void engine_playlist_impl::set_parent(
    const djinterop::playlist_impl* parent_base_maybe)
{
    throw unsupported_operation{
        "Engine V1 databases do not support nested playlists"};
}

std::optional<playlist> engine_playlist_impl::sub_playlist_by_name(
    const std::string& name)
{
    throw unsupported_operation{
        "Engine V1 databases do not support nested playlists"};
}

std::vector<track> engine_playlist_impl::tracks() const
{
    std::vector<track> results;
    storage_->db << "SELECT trackId FROM PlaylistTrackList WHERE playlistId = "
                    "? ORDER BY trackNumber"
                 << id() >>
        [&](int64_t track_id)
    {
        results.emplace_back(
            std::make_shared<engine_track_impl>(storage_, track_id));
    };
    return results;
}

}  // namespace djinterop::engine::v1
