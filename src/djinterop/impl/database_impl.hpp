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

#include <bitset>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/database.hpp>

namespace djinterop
{
class crate;
class playlist;
class playlist_impl;
class track;
struct track_snapshot;

class database_impl
{
public:
    explicit database_impl(std::initializer_list<djinterop::feature> features)
    {
        for (auto&& feat : features)
            features_.set(static_cast<size_t>(feat));
    }
    virtual ~database_impl() = default;

    bool supports_feature(const feature& feature) const noexcept
    {
        return features_.test(static_cast<size_t>(feature));
    }

    virtual std::optional<crate> crate_by_id(int64_t id) = 0;
    virtual std::vector<crate> crates() = 0;
    virtual std::vector<crate> crates_by_name(const std::string& name) = 0;
    virtual playlist create_root_playlist(const std::string& name) = 0;
    virtual playlist create_root_playlist_after(
        const std::string& name, const playlist_impl& after) = 0;
    virtual crate create_root_crate(const std::string& name) = 0;
    virtual crate create_root_crate_after(
        const std::string& name, const crate& after) = 0;
    virtual track create_track(const track_snapshot& snapshot) = 0;
    virtual std::string directory() = 0;
    virtual void verify() = 0;
    virtual std::vector<playlist> playlists_by_name(
        const std::string& name) = 0;
    virtual std::vector<playlist> playlists() = 0;
    virtual void remove_crate(crate cr) = 0;
    virtual void remove_playlist(const playlist_impl& pl) = 0;
    virtual void remove_track(track tr) = 0;
    virtual std::vector<crate> root_crates() = 0;
    virtual std::optional<crate> root_crate_by_name(
        const std::string& name) = 0;
    virtual std::vector<playlist> root_playlists() = 0;
    virtual std::optional<playlist> root_playlist_by_name(
        const std::string& name) = 0;
    virtual std::optional<track> track_by_id(int64_t id) = 0;
    virtual std::vector<track> tracks() = 0;
    virtual std::vector<track> tracks_by_relative_path(
        const std::string& relative_path) = 0;
    virtual std::string uuid() = 0;
    virtual std::string version_name() = 0;

private:
    std::bitset<64> features_;
};

}  // namespace djinterop
