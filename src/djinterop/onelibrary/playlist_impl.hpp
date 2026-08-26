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
#include <optional>
#include <string>
#include <vector>

#include "../impl/playlist_impl.hpp"
#include "onelibrary_context.hpp"

namespace djinterop::onelibrary
{
/// A playlist in a OneLibrary database.
///
/// The format keeps one tree that serves as both playlists and crates, so this
/// reads the same rows as `crate_impl`.
class playlist_impl : public djinterop::playlist_impl
{
public:
    playlist_impl(std::shared_ptr<onelibrary_context> context, int64_t id);

    [[nodiscard]] int64_t id() const noexcept { return id_; }

    void add_track_back(const djinterop::track_impl& tr) override;
    void add_track_after(
        const djinterop::track_impl& tr,
        const djinterop::track_impl& after) override;
    std::vector<playlist> children() override;
    void clear_tracks() override;
    playlist create_sub_playlist(const std::string& name) override;
    playlist create_sub_playlist_after(
        const std::string& name,
        const djinterop::playlist_impl& after) override;
    [[nodiscard]] database db() const override;
    [[nodiscard]] std::string name() const override;
    std::optional<playlist> parent() override;
    void remove_track(const djinterop::track_impl& tr) override;
    void set_name(const std::string& name) override;
    void set_parent(const djinterop::playlist_impl* parent_maybe) override;
    std::optional<playlist> sub_playlist_by_name(
        const std::string& name) override;
    [[nodiscard]] std::vector<track> tracks() const override;

    bool operator==(const djinterop::playlist_impl& other) const override;

private:
    std::shared_ptr<onelibrary_context> context_;
    int64_t id_;
};

/// Wrap a playlist row as a playlist.
playlist make_playlist(std::shared_ptr<onelibrary_context> context, int64_t id);

}  // namespace djinterop::onelibrary
