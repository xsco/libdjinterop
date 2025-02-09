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

#include <string>
#include <vector>

#include <djinterop/playlist.hpp>

#include "impl/playlist_impl.hpp"

namespace djinterop
{
playlist::playlist(const playlist& other) noexcept = default;

playlist::~playlist() = default;

playlist& playlist::operator=(const playlist& other) noexcept = default;

void playlist::add_track_back(track tr)
{
    pimpl_->add_track_back(tr);
}

void playlist::add_track_after(track tr, track after)
{
    pimpl_->add_track_after(tr, after);
}

void playlist::clear_tracks()
{
    pimpl_->clear_tracks();
}

database playlist::db() const
{
    return pimpl_->db();
}

std::string playlist::name() const
{
    return pimpl_->name();
}

void playlist::remove_track(track tr)
{
    pimpl_->remove_track(tr);
}

void playlist::set_name(const std::string& name)
{
    pimpl_->set_name(name);
}

std::vector<track> playlist::tracks() const
{
    return pimpl_->tracks();
}

playlist::playlist(std::shared_ptr<playlist_impl> pimpl) : pimpl_{std::move(pimpl)} {}

}  // namespace djinterop
