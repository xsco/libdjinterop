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
#include <memory>
#include <string>
#include <vector>

#include <djinterop/database.hpp>
#include <djinterop/engine/v2/engine_library.hpp>
#include <djinterop/engine/v2/playlist_entity_table.hpp>
#include <djinterop/engine/v2/playlist_table.hpp>
#include <djinterop/track.hpp>

#include "../../impl/playlist_impl.hpp"

namespace djinterop::engine::v2
{
class playlist_impl : public djinterop::playlist_impl
{
public:
    playlist_impl(std::shared_ptr<engine_library> library, int64_t id);

    void add_track_back(track tr) const override;
    void add_track_after(track tr, track after) const override;
    void clear_tracks() const override;
    database db() const override;
    std::string name() const override;
    void remove_track(track tr) const override;
    void set_name(const std::string& name) const override;
    std::vector<track> tracks() const override;

private:
    std::shared_ptr<engine_library> library_;
    playlist_entity_table playlist_entity_;
    playlist_table playlist_;

    int64_t id_;
};

}  // namespace djinterop::engine::v2
