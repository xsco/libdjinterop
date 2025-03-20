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

#include <djinterop/engine/v2/engine_library.hpp>

#include "../../impl/crate_impl.hpp"

namespace djinterop
{
class track;

namespace engine::v2
{
class crate_impl : public djinterop::crate_impl
{
public:
    crate_impl(std::shared_ptr<engine_library> library, int64_t id);

    void add_track(int64_t track_id) override;
    void add_track(track tr) override;
    std::vector<crate> children() override;
    void clear_tracks() override;
    crate create_sub_crate(const std::string& name) override;
    crate create_sub_crate_after(
        const std::string& name, const crate& after) override;
    database db() override;
    std::vector<crate> descendants() override;
    bool is_valid() override;
    std::string name() override;
    std::optional<crate> parent() override;
    void remove_track(track tr) override;
    void set_name(std::string name) override;
    void set_parent(std::optional<crate> parent) override;
    std::optional<crate> sub_crate_by_name(const std::string& name) override;
    std::vector<track> tracks() override;

private:
    std::shared_ptr<engine_library> library_;
};

}  // namespace engine::v2
}  // namespace djinterop
