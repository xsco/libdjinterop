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

#include "../../impl/playlist_impl.hpp"

namespace djinterop
{
class track;

namespace engine::v1
{
class engine_storage;

class engine_playlist_impl : public djinterop::playlist_impl
{
public:
    engine_playlist_impl(std::shared_ptr<engine_storage> storage, int64_t id);

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
        const djinterop::playlist_impl& after_base) override;
    database db() const override;
    std::string name() const override;
    std::optional<playlist> parent() override;
    void remove_track(const djinterop::track_impl& tr) override;
    std::optional<playlist> sub_playlist_by_name(
        const std::string& name) override;
    void set_name(const std::string& name) override;
    void set_parent(const djinterop::playlist_impl* parent_base_maybe) override;
    std::vector<track> tracks() const override;

    bool operator==(const djinterop::playlist_impl& other_base) const override
    {
        const auto* other =
            dynamic_cast<const engine_playlist_impl*>(&other_base);
        return other != nullptr && id_ == other->id_;
    }

private:
    std::shared_ptr<engine_storage> storage_;

    int64_t id_;
};

}  // namespace engine::v1
}  // namespace djinterop
