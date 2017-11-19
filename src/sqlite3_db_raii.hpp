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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_SQLITE3_DB_RAII_HPP
#define ENGINEPRIME_SQLITE3_DB_RAII_HPP

#include <stdexcept>
#include <string>
#include <iostream>
#include "sqlite3.h"

namespace engineprime {

// RAII_friendly wrapper around opening/closing SQLite3 databases
struct sqlite3_db_raii
{
    sqlite3_db_raii(const std::string &path)
    {
        this->path = path;
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
        {
            std::string err_msg_str{sqlite3_errmsg(db)};
            sqlite3_close(db);
            throw std::runtime_error{err_msg_str};
        }
    }
    
    ~sqlite3_db_raii()
    {
        sqlite3_close(db);
    }
    
    std::string path;
    sqlite3 *db;
};

} // engineprime

#endif // ENGINEPRIME_SQLITE3_DB_RAII_HPP
