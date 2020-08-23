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

#include <stdexcept>
#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/semantic_version.hpp>

namespace djinterop::enginelibrary::schema
{
class schema_creator_validator
{
public:
    virtual ~schema_creator_validator() = default;
    virtual std::string name() const = 0;

    virtual void verify(sqlite::database& db) const
    {
        verify_music_schema(db);
        verify_performance_schema(db);
    }

    virtual void create(sqlite::database& db)
    {
        create_music_schema(db);
        create_performance_schema(db);
    }

protected:
    virtual void verify_music_schema(sqlite::database& db) const = 0;
    virtual void verify_performance_schema(sqlite::database& db) const = 0;
    virtual void create_music_schema(sqlite::database& db) = 0;
    virtual void create_performance_schema(sqlite::database& db) = 0;
};

bool is_supported(const semantic_version& version);

std::unique_ptr<schema_creator_validator> make_schema_creator_validator(
    const semantic_version& version);

}  // namespace djinterop::enginelibrary::schema
