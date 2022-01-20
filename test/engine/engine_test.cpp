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

#define BOOST_TEST_MODULE engine_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <djinterop/engine/engine.hpp>

#include "temporary_directory.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;

BOOST_TEST_DECORATOR(
    * utf::description("load_database() with a non-existent path"))
BOOST_AUTO_TEST_CASE(load_database__fake_path__throw)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;

    {
        // Arrange/Act/Assert
        BOOST_CHECK_THROW(
            auto db = e::load_database(tmp_loc.temp_dir + "/does_not_exist"),
            djinterop::database_not_found);
    }
}

BOOST_TEST_DECORATOR(
    * utf::description("create_database() with all supported schema versions"))
BOOST_DATA_TEST_CASE(
    create_database__valid_version__creates_verified, e::all_versions, version)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;

    {
        // Arrange/Act
        auto db = e::create_database(tmp_loc.temp_dir, version);

        // Assert
        BOOST_CHECK_NO_THROW(db.verify());
        BOOST_CHECK_EQUAL(db.directory(), tmp_loc.temp_dir);
        BOOST_CHECK_EQUAL(e::music_db_path(db), (tmp_loc.temp_dir + "/m.db"));
        BOOST_CHECK_EQUAL(
            e::perfdata_db_path(db), (tmp_loc.temp_dir + "/p.db"));
        BOOST_CHECK_EQUAL(db.version_name(), version.name);
    }
}
