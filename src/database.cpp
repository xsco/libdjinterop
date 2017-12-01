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
#include <sys/stat.h>
#include "sqlite3_db_raii.hpp"

namespace engineprime {

// Utility method for getting std::string out of a stmt
static std::string sqlite3_column_str(sqlite3_stmt *stmt, int index)
{
	auto uchar_ptr = sqlite3_column_text(stmt, index);
	if (uchar_ptr == nullptr)
		return "";
	return reinterpret_cast<const char *>(uchar_ptr);
}

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
        
        uuid_ = sqlite3_column_str(stmt, 0);
        version_.maj = sqlite3_column_int(stmt, 1);
        version_.min = sqlite3_column_int(stmt, 2);
        version_.pat = sqlite3_column_int(stmt, 3);
        
        sqlite3_finalize(stmt);
    }

	bool exists()
	{
		struct stat buffer;
		auto m_exists = stat(db_m_path_.c_str(), &buffer) == 0;
		auto p_exists = stat(db_p_path_.c_str(), &buffer) == 0;
		return m_exists && p_exists;
	}
    
    std::string dir_path_;
    std::string db_m_path_;
    std::string db_p_path_;
    std::string uuid_;
	schema_version version_;
};


database::database(const std::string &dir_path) :
	pimpl_{new impl{dir_path}}
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
const schema_version &database::version() const
{
    return pimpl_->version_;
}

database create_database(const std::string &dir_path,
		const schema_version &version)
{
	if (version != version_firmware_1_0_0 &&
		version != version_firmware_1_0_3)
	{
		throw unsupported_database_version{
			"Unsupported database version", version};
	}

	// TODO - create schema for m.db and p.db

	database db{dir_path};
	return db;
}

} // namespace engineprime

