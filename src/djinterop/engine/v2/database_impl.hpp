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

#include <memory>

#include <djinterop/engine/v2/engine_library.hpp>

#include "../../impl/database_impl.hpp"
#include "../base_engine_impl.hpp"

namespace djinterop::engine::v2
{
class database_impl : public djinterop::database_impl,
                      base_engine_impl<database_impl, engine_library>
{
public:
    explicit database_impl(std::shared_ptr<engine_library> library);

    std::optional<djinterop::crate> crate_by_id(int64_t id) override;
    std::vector<djinterop::crate> crates() override;
    std::vector<djinterop::crate> crates_by_name(
        const std::string& name) override;
    crate create_root_crate(const std::string& name) override;
    crate create_root_crate_after(
        const std::string& name, const crate& after) override;
    playlist create_root_playlist(const std::string& name) override;
    playlist create_root_playlist_after(
        const std::string& name,
        const djinterop::playlist_impl& after_base) override;
    track create_track(const track_snapshot& snapshot) override;
    std::string directory() override;
    void verify() override;
    std::vector<playlist> playlists_by_name(const std::string& name) override;
    std::vector<playlist> playlists() override;
    void remove_crate(djinterop::crate cr) override;
    void remove_playlist(const djinterop::playlist_impl& pl_base) override;
    void remove_track(djinterop::track tr) override;
    std::vector<djinterop::crate> root_crates() override;
    std::optional<djinterop::crate> root_crate_by_name(
        const std::string& name) override;
    std::vector<playlist> root_playlists() override;
    std::optional<djinterop::playlist> root_playlist_by_name(
        const std::string& name) override;
    std::optional<djinterop::track> track_by_id(int64_t id) override;
    std::vector<djinterop::track> tracks() override;
    std::vector<djinterop::track> tracks_by_relative_path(
        const std::string& relative_path) override;
    std::string uuid() override;
    std::string version_name() override;
};

}  // namespace djinterop::engine::v2
