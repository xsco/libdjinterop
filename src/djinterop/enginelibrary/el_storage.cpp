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

#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/enginelibrary/schema.hpp>

namespace djinterop
{
namespace enginelibrary
{
el_storage::el_storage(std::string directory)
    : directory{directory}, db{":memory:"}
{
    // TODO (haslersn): Should we check that directory is an absolute path?
    db << "ATTACH ? as 'music'" << (directory + "/m.db");
    db << "ATTACH ? as 'perfdata'" << (directory + "/p.db");
}

bool el_storage::schema_version_supported(semantic_version schema_version)
{
    return is_supported(schema_version);
}

void el_storage::create_and_validate_schema(semantic_version schema_version)
{
    create_music_schema(db, schema_version);
    verify_music_schema(db);
    create_performance_schema(db, schema_version);
    verify_performance_schema(db);
}

bool el_storage::schema_created() const
{
    std::string sql =
        "SELECT SUM(rows) FROM ("
        "  SELECT COUNT(*) AS rows "
        "  FROM music.sqlite_master WHERE "
        "  type = 'table' "
        "  UNION ALL "
        "  SELECT COUNT(*) AS rows "
        "  FROM perfdata.sqlite_master "
        "  WHERE type = 'table'"
        ")";
    int32_t table_count;
    db << sql >> table_count;
    return table_count != 0;
}

}  // namespace enginelibrary
}  // namespace djinterop
