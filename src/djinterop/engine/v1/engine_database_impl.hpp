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

#include <sqlite_modern_cpp.h>

#include "../../impl/database_impl.hpp"
#include "engine_storage.hpp"

namespace djinterop::engine::v1
{
class engine_database_impl : public database_impl
{
public:
    engine_database_impl(std::shared_ptr<engine_storage> storage);

    std::optional<djinterop::crate> crate_by_id(int64_t id) override;
    std::vector<djinterop::crate> crates() override;
    std::vector<djinterop::crate> crates_by_name(
        const std::string& name) override;
    crate create_root_crate(const std::string& name) override;
    crate create_root_crate_after(
        const std::string& name, const crate& after) override;
    track create_track(const track_snapshot& snapshot) override;
    std::string directory() override;
    void verify() override;
    void remove_crate(djinterop::crate cr) override;
    void remove_track(djinterop::track tr) override;
    std::vector<djinterop::crate> root_crates() override;
    std::optional<djinterop::crate> root_crate_by_name(
        const std::string& name) override;
    std::optional<djinterop::track> track_by_id(int64_t id) override;
    std::vector<djinterop::track> tracks() override;
    std::vector<djinterop::track> tracks_by_relative_path(
        const std::string& relative_path) override;
    std::string uuid() override;
    std::string version_name() override;

private:
    std::shared_ptr<engine_storage> storage_;
};

}  // namespace djinterop::engine::v1
