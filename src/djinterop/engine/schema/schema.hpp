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

#include <djinterop/engine/engine_version.hpp>
#include <djinterop/semantic_version.hpp>

namespace djinterop::engine::schema
{
class schema_creator_validator
{
public:
    virtual ~schema_creator_validator() = default;

    virtual void verify(sqlite::database& db) const = 0;
    virtual void create(sqlite::database& db) = 0;
};

class schema_v1 : public schema_creator_validator
{
public:
    ~schema_v1() override = default;

    void verify(sqlite::database& db) const override
    {
        verify_music_schema(db);
        verify_performance_schema(db);
    }

    void create(sqlite::database& db) override
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

class schema_v2 : public schema_creator_validator
{
public:
    ~schema_v2() override = default;
};

std::unique_ptr<schema_creator_validator> make_schema_creator_validator(
    const engine_version& version);

}  // namespace djinterop::engine::schema
