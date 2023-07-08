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

#include "schema.hpp"

namespace djinterop::engine::schema
{
class schema_2_18_0 : public schema_v2
{
public:
    static constexpr const semantic_version schema_version{2, 18, 0};

    void verify(sqlite::database& db) const override;
    void create(sqlite::database& db) override;

protected:
    virtual void verify_master_list(sqlite::database& db) const;

    virtual void verify_album_art(sqlite::database& db) const;
    virtual void verify_change_log(sqlite::database& db) const;
    virtual void verify_information(sqlite::database& db) const;
    virtual void verify_pack(sqlite::database& db) const;
    virtual void verify_playlist(sqlite::database& db) const;
    virtual void verify_playlist_entity(sqlite::database& db) const;
    virtual void verify_preparelist_entity(sqlite::database& db) const;
    virtual void verify_track(sqlite::database& db) const;
};

}  // namespace djinterop::engine::schema
