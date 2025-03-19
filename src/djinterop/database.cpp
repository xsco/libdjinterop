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

#include "impl/database_impl.hpp"
#include "djinterop/database.hpp"


namespace djinterop
{
database::database(const database& db) = default;

database::~database() = default;

database& database::operator=(const database& db) = default;

bool database::supports_feature(const feature& feature) const noexcept
{
    return pimpl_->supports_feature(feature);
}

std::optional<crate> database::crate_by_id(int64_t id) const
{
    return pimpl_->crate_by_id(id);
}

std::vector<crate> database::crates() const
{
    return pimpl_->crates();
}

std::vector<crate> database::crates_by_name(const std::string& name) const
{
    return pimpl_->crates_by_name(name);
}

playlist database::create_root_playlist(const std::string& name)
{
    return pimpl_->create_root_playlist(name);
}

playlist database::create_root_playlist_after(const std::string& name, const playlist& after)
{
    return pimpl_->create_root_playlist_after(name, *after.pimpl_);
}

crate database::create_root_crate(const std::string& name)
{
    return pimpl_->create_root_crate(name);
}

crate database::create_root_crate_after(const std::string& name, const crate& after)
{
    return pimpl_->create_root_crate_after(name, after);
}

track database::create_track(const track_snapshot& snapshot)
{
    return pimpl_->create_track(snapshot);
}

std::string database::directory() const
{
    return pimpl_->directory();
}

void database::verify() const
{
    pimpl_->verify();
}

std::vector<playlist> database::playlists_by_name(const std::string& name) const
{
    return pimpl_->playlists_by_name(name);
}

void database::remove_crate(crate cr) const
{
    pimpl_->remove_crate(cr);
}

void database::remove_playlist(playlist pl) const
{
    pimpl_->remove_playlist(*pl.pimpl_);
}

void database::remove_track(track tr) const
{
    pimpl_->remove_track(tr);
}

std::vector<crate> database::root_crates() const
{
    return pimpl_->root_crates();
}

std::optional<crate> database::root_crate_by_name(
    const std::string& name) const
{
    return pimpl_->root_crate_by_name(name);
}

std::vector<playlist> database::root_playlists() const
{
    return pimpl_->root_playlists();
}

std::optional<playlist> database::root_playlist_by_name(
    const std::string& name) const
{
    return pimpl_->root_playlist_by_name(name);
}

std::optional<track> database::track_by_id(int64_t id) const
{
    return pimpl_->track_by_id(id);
}

std::vector<track> database::tracks() const
{
    return pimpl_->tracks();
}

std::vector<track> database::tracks_by_relative_path(
    const std::string& relative_path) const
{
    return pimpl_->tracks_by_relative_path(relative_path);
}

std::string database::uuid() const
{
    return pimpl_->uuid();
}

std::string database::version_name() const
{
    return pimpl_->version_name();
}

database::database(std::shared_ptr<database_impl> pimpl) :
    pimpl_{std::move(pimpl)}
{
}

}  // namespace djinterop
