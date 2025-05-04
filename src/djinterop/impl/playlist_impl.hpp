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
class track_impl;

class playlist_impl
{
public:
    virtual ~playlist_impl() noexcept = default;

    virtual void add_track_back(const track_impl& tr) = 0;
    virtual void add_track_after(const track_impl& tr, const track_impl& after) = 0;
    virtual std::vector<playlist> children() = 0;
    virtual void clear_tracks() = 0;
    virtual playlist create_sub_playlist(const std::string& name) = 0;
    virtual playlist create_sub_playlist_after(
        const std::string& name, const playlist_impl& after) = 0;
    virtual database db() const = 0;
    virtual std::string name() const = 0;
    virtual std::optional<playlist> parent() = 0;
    virtual void remove_track(const track_impl& tr) = 0;
    virtual void set_name(const std::string& name) = 0;
    virtual void set_parent(const playlist_impl* parent_maybe) = 0;
    virtual std::optional<playlist> sub_playlist_by_name(
        const std::string& name) = 0;
    virtual std::vector<track> tracks() const = 0;

    virtual bool operator==(const playlist_impl&) const = 0;
};

}  // namespace djinterop
