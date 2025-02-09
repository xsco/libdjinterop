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

#pragma once

#include <string>
#include <vector>

#include <djinterop/database.hpp>
#include <djinterop/playlist.hpp>
#include <djinterop/track.hpp>

namespace djinterop
{
class playlist_impl
{
public:
    virtual ~playlist_impl() noexcept = default;

    virtual void add_track_back(track tr) const = 0;
    virtual void add_track_after(track tr, track after) const = 0;
    virtual void clear_tracks() const = 0;
    virtual database db() const = 0;
    virtual std::string name() const = 0;
    virtual void remove_track(track tr) const = 0;
    virtual void set_name(const std::string& name) const = 0;
    virtual std::vector<track> tracks() const = 0;
};

}  // namespace djinterop
