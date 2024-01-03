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

#include <djinterop/semantic_version.hpp>

#include "schema_1_6_0.hpp"

namespace djinterop::engine::schema
{
class schema_1_7_1 : public schema_1_6_0
{
public:
    static constexpr const semantic_version schema_version{1, 7, 1};

protected:
    void verify_information(
        sqlite::database& db, const std::string& db_name) const override;
    void verify_track(sqlite::database& db) const override;
    void verify_performance_data(sqlite::database& db) const override;

    void verify_music_schema(sqlite::database& db) const override;
    void verify_performance_schema(sqlite::database& db) const override;

    void create_music_schema(sqlite::database& db) override;
    void create_performance_schema(sqlite::database& db) override;
};

}  // namespace djinterop::engine::schema
