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

#include <engineprime/engineprime.hpp>

#include <stdexcept>
#include <string>
#include "sqlite3.h"

using namespace engineprime;
using std::string;

// RAII_friendly wrapper around opening/closing SQLite3 databases
struct sqlite3_db_raii
{
    sqlite3_db_raii(const std::string &path)
    {
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
        {
            string err_msg_str{sqlite3_errmsg(db)};
            sqlite3_close(db);
            throw std::runtime_error{err_msg_str};
        }
    }
    
    ~sqlite3_db_raii()
    {
        sqlite3_close(db);
    }
    
    sqlite3 *db;
};

struct database::impl
{
    impl(const std::string &dir_path) :
        dir_path_{dir_path},
        db_m_path_{dir_path_ + "/m.db"},
        db_p_path_{dir_path_ + "/p.db"},
        db_m_{db_m_path_},
        db_p_{db_p_path_}
    {
        // Read from 'Information' table
        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db_m_.db,
            "SELECT uuid, schemaVersionMajor, schemaVersionMinor, "
            "schemaVersionPatch "
            "FROM Information",
            -1,
            &stmt,
            0) != SQLITE_OK)
        {
            string err_msg_str{sqlite3_errmsg(db_m_.db)};
            throw std::runtime_error{err_msg_str};
        }
        
        // Expect only a single row in Information
        if (sqlite3_step(stmt) != SQLITE_ROW)
        {
            string err_msg_str{sqlite3_errmsg(db_m_.db)};
            throw std::runtime_error{err_msg_str};
        }
        
        uuid_ = string{reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0))};
        schema_version_major_ = sqlite3_column_int(stmt, 1);
        schema_version_minor_ = sqlite3_column_int(stmt, 2);
        schema_version_patch_ = sqlite3_column_int(stmt, 3);
        
        sqlite3_finalize(stmt);
    }
    
    string dir_path_;
    string db_m_path_;
    string db_p_path_;
    sqlite3_db_raii db_m_;
    sqlite3_db_raii db_p_;
    string uuid_;
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

