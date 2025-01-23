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

#include "engine_library_dir_utils.hpp"

#include <djinterop/exceptions.hpp>

#include "../util/filesystem.hpp"

namespace djinterop::engine
{
namespace
{
std::string make_legacy_m_db_path(const std::string& directory)
{
    return directory + "/m.db";
}

std::string make_legacy_p_db_path(const std::string& directory)
{
    return directory + "/p.db";
}

std::string make_database2_db_dir_path(const std::string& directory)
{
    return directory + "/Database2";
}

std::string make_database2_m_db_path(const std::string& directory)
{
    return directory + "/Database2/m.db";
}
}  // namespace

bool detect_is_database2(const std::string& directory)
{
    if (!djinterop::util::path_exists(directory))
    {
        throw database_not_found{directory};
    }

    // Assume that all versions of engine libraries have a database called
    // "m.db", containing a table called "Information", containing the schema
    // version in some of its columns.
    auto legacy_m_db_path = directory + "/m.db";
    auto database2_m_db_path = directory + "/Database2/m.db";
    auto legacy_m_db_path_exists =
        djinterop::util::path_exists(legacy_m_db_path);
    auto database2_m_db_path_exists =
        djinterop::util::path_exists(database2_m_db_path);

    if (!legacy_m_db_path_exists && !database2_m_db_path_exists)
    {
        throw database_not_found{"Neither m.db nor Database2/m.db was found"};
    }

    if (legacy_m_db_path_exists && database2_m_db_path_exists)
    {
        throw database_not_found{
            "Found m.db and Database2/m.db files, which is not supposed to "
            "happen"};
    }

    return database2_m_db_path_exists;
}

sqlite::database create_legacy_sqlite_database(const std::string& directory)
{
    if (!djinterop::util::path_exists(directory))
    {
        // Note: only creates leaf directory, not entire tree.
        djinterop::util::create_dir(directory);
    }

    sqlite::database db{":memory:"};
    db << "ATTACH ? as 'music'" << make_legacy_m_db_path(directory);
    db << "ATTACH ? as 'perfdata'" << make_legacy_p_db_path(directory);
    return db;
}

sqlite::database create_database2_sqlite_database(const std::string& directory)
{
    // Ensure the target directory exists.
    if (!djinterop::util::path_exists(directory))
    {
        djinterop::util::create_dir(directory);
    }

    auto db_dir_path = make_database2_db_dir_path(directory);
    if (!djinterop::util::path_exists(db_dir_path))
    {
        djinterop::util::create_dir(db_dir_path);
    }

    // Target database must not exist.
    auto db_path = make_database2_m_db_path(directory);
    if (djinterop::util::path_exists(db_path))
    {
        throw database_inconsistency{
            "Cannot create new Engine library, as the database file already "
            "exists"};
    }

    return sqlite::database{db_path};
}

sqlite::database create_temporary_legacy_sqlite_database()
{
    sqlite::database db{":memory:"};
    db << "ATTACH ':memory:' as 'music'";
    db << "ATTACH ':memory:' as 'perfdata'";
    return db;
}

sqlite::database create_temporary_database2_sqlite_database()
{
    return sqlite::database{":memory:"};
}

sqlite::database load_legacy_sqlite_database(const std::string& directory)
{
    if (!djinterop::util::path_exists(directory))
    {
        throw database_not_found{directory};
    }

    sqlite::database db{":memory:"};
    db << "ATTACH ? as 'music'" << (directory + "/m.db");
    db << "ATTACH ? as 'perfdata'" << (directory + "/p.db");
    return db;
}

sqlite::database load_database2_sqlite_database(const std::string& directory)
{
    auto db_path = make_database2_m_db_path(directory);
    if (!djinterop::util::path_exists(db_path))
    {
        throw database_not_found{directory};
    }

    return sqlite::database{db_path};
}

bool legacy_database_exists(const std::string& directory)
{
    return djinterop::util::path_exists(make_legacy_m_db_path(directory));
}

bool database2_database_exists(const std::string& directory)
{
    return djinterop::util::path_exists(make_database2_m_db_path(directory));
}

}  // namespace djinterop::engine
