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

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_DATABASE_HPP
#define DJINTEROP_ENGINELIBRARY_DATABASE_HPP

#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>

#include "schema_version.hpp"

namespace djinterop {
namespace enginelibrary {

class database_not_found : public std::runtime_error
{
public:
    explicit database_not_found(const std::string &what_arg) noexcept :
        runtime_error{what_arg}
    {}
    virtual ~database_not_found() = default;
};

class database
{
public:

    /**
     * \brief Construct an Engine Prime database, specifying the path to the
     *        Engine library directory
     */
    explicit database(const std::string &dir_path);

	/**
	 * \brief Copying is disallowed
	 */
	database(const database &db) = delete;

    /**
     * \brief Move constructor
     */
	database(database &&db) noexcept;

    /**
     * \brief Destructor
     */
    ~database();

	/**
	 * \brief Copy assignment is disallowed
	 */
	database &operator =(const database &db) = delete;

	/**
	 * \brief Move assignment
	 */
	database &operator =(database &&db) noexcept;

    /**
     * \brief Returns a `bool` indicating whether the database files exist on
     *        disk or not
     */
	bool exists() const;

    /**
     * \brief Returns a `bool` indicating whether the database version is
     *        supported by this version of libdjinterop or not
     */
    bool is_supported() const;

    /**
     * \brief Verify the schema of an Engine Prime database, throwing an
     *        exception if there is any kind of inconsistency
     */
    void verify() const;

    /**
     * \brief Get the directory path on which this database is based
     */
    std::string directory_path() const;

    /**
     * \brief Get the path to the music database, i.e. m.db
     */
    std::string music_db_path() const;

    /**
     * \brief Get the path to the performance data database, i.e. p.db
     */
    std::string performance_db_path() const;

    /**
     * \brief Get the UUID of this database
     */
    std::string uuid() const;

    /**
     * \brief Get the schema version of this database
     */
	schema_version version() const;

private:
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

/**
 * \brief Create a new, empty database in a given directory and at a specified
 *        schema version
 */
database create_database(
        const std::string &dir_path, const schema_version &version);

} // enginelibrary
} // djinterop

#endif // DJINTEROP_ENGINELIBRARY_DATABASE_HPP
