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

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace djinterop
{
class crate;
class track;
struct track_snapshot;

class database_impl
{
public:
    virtual ~database_impl();

    virtual std::optional<crate> crate_by_id(int64_t id) = 0;
    virtual std::vector<crate> crates() = 0;
    virtual std::vector<crate> crates_by_name(const std::string& name) = 0;
    virtual crate create_root_crate(const std::string& name) = 0;
    virtual crate create_root_crate_after(
        const std::string& name, const crate& after) = 0;
    virtual track create_track(const track_snapshot& snapshot) = 0;
    virtual std::string directory() = 0;
    virtual void verify() = 0;
    virtual void remove_crate(crate cr) = 0;
    virtual void remove_track(track tr) = 0;
    virtual std::vector<crate> root_crates() = 0;
    virtual std::optional<crate> root_crate_by_name(
        const std::string& name) = 0;
    virtual std::optional<track> track_by_id(int64_t id) = 0;
    virtual std::vector<track> tracks() = 0;
    virtual std::vector<track> tracks_by_relative_path(
        const std::string& relative_path) = 0;
    virtual std::string uuid() = 0;
    virtual std::string version_name() = 0;
};

}  // namespace djinterop
