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

void playlist::add_track_back(const track& tr)
{
    pimpl_->add_track_back(*tr.pimpl_);
}

void playlist::add_track_after(const track& tr, const track& after)
{
    pimpl_->add_track_after(*tr.pimpl_, *after.pimpl_);
}

std::vector<playlist> playlist::children() const
{
    return pimpl_->children();
}

void playlist::clear_tracks()
{
    pimpl_->clear_tracks();
}

playlist playlist::create_sub_playlist(const std::string& name)
{
    return pimpl_->create_sub_playlist(name);
}

playlist playlist::create_sub_playlist_after(const std::string& name, const playlist& after)
{
    return pimpl_-> create_sub_playlist_after(name, *after.pimpl_);
}

database playlist::db() const
{
    return pimpl_->db();
}

std::string playlist::name() const
{
    return pimpl_->name();
}

std::optional<playlist> playlist::parent() const
{
    return pimpl_->parent();
}

void playlist::remove_track(const track& tr)
{
    pimpl_->remove_track(*tr.pimpl_);
}

void playlist::set_name(const std::string& name)
{
    pimpl_->set_name(name);
}

void playlist::set_parent(const std::optional<playlist>& parent) const
{
    pimpl_->set_parent(parent ? parent->pimpl_.get() : nullptr);
}

std::optional<playlist> playlist::sub_playlist_by_name(const std::string& name) const
{
    return pimpl_->sub_playlist_by_name(name);
}

std::vector<track> playlist::tracks() const
{
    return pimpl_->tracks();
}

bool playlist::operator==(const playlist& other) const
{
    return *pimpl_ == *other.pimpl_;
}

playlist::playlist(std::shared_ptr<playlist_impl> pimpl) :
    pimpl_{std::move(pimpl)}
{
}

}  // namespace djinterop
