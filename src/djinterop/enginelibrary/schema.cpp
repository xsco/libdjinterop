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

#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/schema.hpp>
#include <djinterop/enginelibrary/schema_1_6_0.hpp>
#include <djinterop/enginelibrary/schema_1_7_1.hpp>

namespace djinterop
{
namespace enginelibrary
{
template <typename F1, typename F2>
static void dispatch(
    const semantic_version &version, F1 func_1_6_0, F2 func_1_7_1)
{
    if (version == version_1_6_0)
        func_1_6_0();
    else if (version == version_1_7_1)
        func_1_7_1();
    else
        throw unsupported_database_version{version};
}

static semantic_version get_version(sqlite::database &db)
{
    semantic_version version;
    db << "SELECT schemaVersionMajor, schemaVersionMinor, schemaVersionPatch "
          "FROM Information" >>
        std::tie(version.maj, version.min, version.pat);
    return version;
}

bool is_supported(const semantic_version &version)
{
    // TODO - add support for DB schema version 1.9.1 (new "List" tables, old
    // ones as VIEWs)
    return (version == version_1_6_0 || version == version_1_7_1);
}

semantic_version verify_music_schema(sqlite::database &db)
{
    auto version = get_version(db);
    dispatch(
        version, [&db] { verify_music_schema_1_6_0(db); },
        [&db] { verify_music_schema_1_7_1(db); });

    return version;
}

semantic_version verify_performance_schema(sqlite::database &db)
{
    auto version = get_version(db);
    dispatch(
        version, [&db] { verify_performance_schema_1_6_0(db); },
        [&db] { verify_performance_schema_1_7_1(db); });

    return version;
}

void create_music_schema(sqlite::database &db, const semantic_version &version)
{
    dispatch(
        version, [&db] { create_music_schema_1_6_0(db); },
        [&db] { create_music_schema_1_7_1(db); });
}

void create_performance_schema(
    sqlite::database &db, const semantic_version &version)
{
    dispatch(
        version, [&db] { create_performance_schema_1_6_0(db); },
        [&db] { create_performance_schema_1_7_1(db); });
}

}  // namespace enginelibrary
}  // namespace djinterop
