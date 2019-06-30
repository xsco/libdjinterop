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

#include <djinterop/enginelibrary/database.hpp>

#include <sys/stat.h>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#endif
#include <tuple>
#include "sqlite_modern_cpp.h"

#include "schema.hpp"

namespace djinterop
{
namespace enginelibrary
{
struct database::impl
{
    impl(const std::string &dir_path)
        : dir_path_{dir_path},
          db_m_path_{dir_path_ + "/m.db"},
          db_p_path_{dir_path_ + "/p.db"}
    {
        if (exists())
        {
            sqlite::database m_db{db_m_path_};
            m_db << "SELECT uuid, schemaVersionMajor, schemaVersionMinor, "
                    "schemaVersionPatch "
                    "FROM Information" >>
                std::tie(uuid_, version_.maj, version_.min, version_.pat);
        }
    }

    bool exists() const
    {
        struct stat buffer;
        auto m_exists = stat(db_m_path_.c_str(), &buffer) == 0;
        auto p_exists = stat(db_p_path_.c_str(), &buffer) == 0;
        return m_exists && p_exists;
    }

    bool is_supported() const
    {
        return djinterop::enginelibrary::is_supported(version_);
    }

    void verify()
    {
        // Check for existence of files on disk
        struct stat buffer;
        auto m_exists = stat(db_m_path_.c_str(), &buffer) == 0;
        if (!m_exists)
        {
            throw database_not_found{db_m_path_};
        }

        auto p_exists = stat(db_p_path_.c_str(), &buffer) == 0;
        if (!p_exists)
        {
            throw database_not_found{db_p_path_};
        }

        // Verify music schema
        sqlite::database m_db{db_m_path_};
        verify_music_schema(m_db);

        // Verify performance schema
        sqlite::database p_db{db_p_path_};
        verify_performance_schema(p_db);
    }

    std::string dir_path_;
    std::string db_m_path_;
    std::string db_p_path_;
    std::string uuid_;
    schema_version version_;
};

database::database(const std::string &dir_path) : pimpl_{new impl{dir_path}} {}

database::database(database &&db) noexcept : pimpl_{std::move(db.pimpl_)} {}

database::~database() = default;

database &database::operator=(database &&db) noexcept = default;

bool database::exists() const
{
    return pimpl_->exists();
}
bool database::is_supported() const
{
    return pimpl_->is_supported();
}

void database::verify() const
{
    pimpl_->verify();
}

std::string database::directory_path() const
{
    return pimpl_->dir_path_;
}
std::string database::music_db_path() const
{
    return pimpl_->db_m_path_;
}
std::string database::performance_db_path() const
{
    return pimpl_->db_p_path_;
}
std::string database::uuid() const
{
    return pimpl_->uuid_;
}
schema_version database::version() const
{
    return pimpl_->version_;
}

database create_database(
    const std::string &dir_path, const schema_version &version)
{
    if (!is_supported(version))
    {
        throw unsupported_database_version{"Unsupported database version",
                                           version};
    }

    // Ensure the target directory exists
    struct stat buf;
    if (stat(dir_path.c_str(), &buf) != 0)
    {
        // Create the dir
#if defined(_WIN32)
        if (_mkdir(dir_path.c_str()) != 0)
#else
        if (mkdir(dir_path.c_str(), 0755) != 0)
#endif
        {
            throw std::runtime_error{
                "Failed to create directory to hold new database"};
        }
    }

    // Create schema for m.db
    auto m_db_path = dir_path + "/m.db";
    if (stat(m_db_path.c_str(), &buf) == 0)
        throw std::runtime_error{
            "Can't create new database because m.db file already exists"};
    sqlite::database m_db{m_db_path};
    create_music_schema(m_db, version);
    verify_music_schema(m_db);

    // Create schema for p.db
    auto p_db_path = dir_path + "/p.db";
    if (stat(p_db_path.c_str(), &buf) == 0)
        throw std::runtime_error{
            "Can't create new database because p.db file already exists"};
    sqlite::database p_db{p_db_path};
    create_performance_schema(p_db, version);
    verify_performance_schema(p_db);

    database db{dir_path};
    return db;
}

}  // namespace enginelibrary
}  // namespace djinterop
