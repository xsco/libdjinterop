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

#include "el_storage.hpp"

#include <djinterop/database.hpp>
#include <djinterop/exceptions.hpp>

#include "../util.hpp"
#include "schema/schema.hpp"

namespace djinterop::enginelibrary
{
namespace
{
sqlite::database make_attached_db(const std::string& directory, bool must_exist)
{
    if (!dir_exists(directory))
    {
        if (must_exist)
        {
            throw database_not_found{directory};
        }
        else
        {
            // Note: only creates leaf directory, not entire tree.
            create_dir(directory);
        }
    }

    sqlite::database db{":memory:"};
    db << "ATTACH ? as 'music'" << (directory + "/m.db");
    db << "ATTACH ? as 'perfdata'" << (directory + "/p.db");
    return db;
}

semantic_version get_version(sqlite::database& db)
{
    // Check that the `Information` table has been created.
    std::string sql =
        "SELECT SUM(rows) FROM ("
        "  SELECT COUNT(*) AS rows "
        "  FROM music.sqlite_master "
        "  WHERE name = 'Information' "
        "  UNION ALL "
        "  SELECT COUNT(*) AS rows "
        "  FROM perfdata.sqlite_master "
        "  WHERE name = 'Information' "
        ")";
    int32_t table_count;
    db << sql >> table_count;
    if (table_count != 2)
    {
        throw database_inconsistency{
            "Did not find an `Information` table for both the music and "
            "performance databases"};
    }

    semantic_version music_version;
    semantic_version perfdata_version;
    db << "SELECT schemaVersionMajor, schemaVersionMinor, "
          "schemaVersionPatch FROM music.Information" >>
        std::tie(music_version.maj, music_version.min, music_version.pat);
    db << "SELECT schemaVersionMajor, schemaVersionMinor, "
          "schemaVersionPatch FROM music.Information" >>
        std::tie(
            perfdata_version.maj, perfdata_version.min, perfdata_version.pat);
    if (music_version != perfdata_version)
    {
        throw database_inconsistency{
            "The stated schema versions do not match between the music and "
            "performance data databases!"};
    }

    return music_version;
}

}  // anonymous namespace

el_storage::el_storage(const std::string& directory) :
    directory{directory}, db{make_attached_db(directory, true)},
    version{get_version(db)},
    schema_creator_validator{schema::make_schema_creator_validator(version)}
{
}

el_storage::el_storage(const std::string& directory, semantic_version version) :
    directory{directory}, db{make_attached_db(directory, false)},
    version{version}, schema_creator_validator{
                          schema::make_schema_creator_validator(version)}
{
    // Create the desired schema on the new database.
    schema_creator_validator->create(db);
}

}  // namespace djinterop::enginelibrary
