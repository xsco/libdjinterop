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

#include <boost/utility/string_view.hpp>

#include <djinterop/crate.hpp>
#include <djinterop/impl/database_impl.hpp>
#include <djinterop/track.hpp>

namespace djinterop
{
class database;
class track;

class crate_impl
{
public:
    crate_impl(int64_t id) noexcept;
    virtual ~crate_impl() noexcept;

    int64_t id() noexcept;

    virtual void add_track(track tr) = 0;
    virtual std::vector<crate> children() = 0;
    virtual void clear_tracks() = 0;
    virtual database db() = 0;
    virtual std::vector<crate> descendants() = 0;
    virtual bool is_valid() = 0;
    virtual std::string name() = 0;
    virtual boost::optional<crate> parent() = 0;
    virtual void remove_track(track tr) = 0;
    virtual void set_name(boost::string_view name) = 0;
    virtual void set_parent(boost::optional<crate> parent) = 0;
    virtual std::vector<track> tracks() = 0;

private:
    int64_t id_;
};

}  // namespace djinterop
