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

#include <djinterop/djinterop.hpp>
#include <djinterop/impl/crate_impl.hpp>
#include <djinterop/impl/util.hpp>

namespace djinterop
{
crate::crate(const crate& other) noexcept = default;

crate::~crate() = default;

crate& crate::operator=(const crate& other) noexcept = default;

void crate::add_track(int64_t track_id) const
{
    pimpl_->add_track(track_id);
}

void crate::add_track(track tr) const
{
    pimpl_->add_track(tr);
}

std::vector<crate> crate::children() const
{
    return pimpl_->children();
}

void crate::clear_tracks() const
{
    pimpl_->clear_tracks();
}

crate crate::create_sub_crate(std::string name)
{
    return pimpl_->create_sub_crate(name);
}

database crate::db() const
{
    return pimpl_->db();
}

std::vector<crate> crate::descendants() const
{
    return pimpl_->descendants();
}

int64_t crate::id() const
{
    return pimpl_->id();
}

bool crate::is_valid() const
{
    return pimpl_->is_valid();
}

std::string crate::name() const
{
    return pimpl_->name();
}

std::experimental::optional<crate> crate::parent() const
{
    return from_boost_optional(pimpl_->parent());
}

void crate::remove_track(track tr) const
{
    pimpl_->remove_track(tr);
}

void crate::set_name(std::string name) const
{
    pimpl_->set_name(name);
}

void crate::set_parent(std::experimental::optional<crate> parent) const
{
    pimpl_->set_parent(to_boost_optional(parent));
}

std::experimental::optional<crate> crate::sub_crate_by_name(
        const std::string& name) const
{
    return from_boost_optional(pimpl_->sub_crate_by_name(name));
}

std::vector<track> crate::tracks() const
{
    return pimpl_->tracks();
}

crate::crate(std::shared_ptr<crate_impl> pimpl) : pimpl_{std::move(pimpl)} {}

}  // namespace djinterop
