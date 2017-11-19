/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engineprime/database.hpp>

#include <string>
#include "sqlite3_db_raii.hpp"

namespace engineprime {

using std::string;

struct database::impl
{
    impl(const std::string &dir_path) :
        dir_path_{dir_path},
        db_m_path_{dir_path_ + "/m.db"},
        db_p_path_{dir_path_ + "/p.db"}
    {
        sqlite3_db_raii db_m{db_m_path_};
        
        // Read from 'Information' table
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db_m.db,
            "SELECT uuid, schemaVersionMajor, schemaVersionMinor, "
            "schemaVersionPatch "
            "FROM Information",
            -1,
            &stmt,
            0) != SQLITE_OK)
        {
            std::string err_msg_str{sqlite3_errmsg(db_m.db)};
            throw std::runtime_error{err_msg_str};
        }
        
        // Expect only a single row in Information
        if (sqlite3_step(stmt) != SQLITE_ROW)
        {
            std::string err_msg_str{sqlite3_errmsg(db_m.db)};
            throw std::runtime_error{err_msg_str};
        }
        
        uuid_ = string{reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0))};
        schema_version_major_ = sqlite3_column_int(stmt, 1);
        schema_version_minor_ = sqlite3_column_int(stmt, 2);
        schema_version_patch_ = sqlite3_column_int(stmt, 3);
        
        sqlite3_finalize(stmt);
    }
    
    std::string dir_path_;
    std::string db_m_path_;
    std::string db_p_path_;
    std::string uuid_;
    int schema_version_major_;
    int schema_version_minor_;
    int schema_version_patch_;
};


database::database(const std::string &dir_path) : pimpl_{new impl{dir_path}}
{}

database::~database() = default;

const std::string &database::directory_path() const
{
    return pimpl_->dir_path_;
}
const std::string &database::music_db_path() const
{
    return pimpl_->db_m_path_;
}
const std::string &database::performance_db_path() const
{
    return pimpl_->db_p_path_;
}

const std::string &database::uuid() const
{
    return pimpl_->uuid_;
}
int database::schema_version_major() const
{
    return pimpl_->schema_version_major_;
}
int database::schema_version_minor() const
{
    return pimpl_->schema_version_minor_;
}
int database::schema_version_patch() const
{
    return pimpl_->schema_version_patch_;
}

} // namespace engineprime
