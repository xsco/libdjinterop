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

#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/impl/database_impl.hpp>

namespace djinterop
{
namespace enginelibrary
{
class el_database_impl : public database_impl
{
public:
    el_database_impl(std::shared_ptr<el_storage> storage);

    transaction_guard begin_transaction() override;
    stdx::optional<djinterop::crate> crate_by_id(int64_t id) override;
    std::vector<djinterop::crate> crates() override;
    std::vector<djinterop::crate> crates_by_name(
        const std::string& name) override;
    djinterop::crate create_root_crate(std::string name) override;
    track create_track(const track_snapshot& snapshot) override;
    std::string directory() override;
    bool is_supported() override;
    void verify() override;
    void remove_crate(djinterop::crate cr) override;
    void remove_track(djinterop::track tr) override;
    std::vector<djinterop::crate> root_crates() override;
    stdx::optional<djinterop::crate> root_crate_by_name(
        const std::string& name) override;
    stdx::optional<djinterop::track> track_by_id(int64_t id) override;
    std::vector<djinterop::track> tracks() override;
    std::vector<djinterop::track> tracks_by_relative_path(
        const std::string& relative_path) override;
    std::string uuid() override;
    semantic_version version() override;
    std::string version_name() override;

private:
    std::shared_ptr<el_storage> storage_;
};

}  // namespace enginelibrary
}  // namespace djinterop
