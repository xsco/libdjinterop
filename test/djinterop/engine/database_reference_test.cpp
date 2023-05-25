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
#include <djinterop/database.hpp>

#define BOOST_TEST_MODULE database_reference_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>

#include <djinterop/crate.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "../temporary_directory.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace utf = boost::unit_test;
namespace e = djinterop::engine;

namespace
{
const std::vector<std::string> ref_script_dirs{
    "/ref/engine/sc5000/firmware-1.0.3",
    "/ref/engine/ep/ep-1.1.1",
    "/ref/engine/sc5000/firmware-1.2.0",
    "/ref/engine/sc5000/firmware-1.2.2",
    "/ref/engine/ep/ep-1.2.2",
    "/ref/engine/sc5000/firmware-1.3.1",
    "/ref/engine/sc5000/firmware-1.4.0",
    "/ref/engine/sc5000/firmware-1.5.1",
    "/ref/engine/ep/ep-1.5.1",
    "/ref/engine/sc5000/firmware-1.5.2",
    "/ref/engine/ep/ep-1.6.0",
    "/ref/engine/sc5000/firmware-1.6.0",
    "/ref/engine/ep/ep-1.6.1",
    "/ref/engine/sc5000/firmware-1.6.1",
    "/ref/engine/sc5000/firmware-1.6.2",
    "/ref/engine/desktop/desktop-2.0.0",
    "/ref/engine/sc5000/firmware-2.0.0",
    "/ref/engine/desktop/desktop-2.0.1",
    "/ref/engine/sc5000/firmware-2.0.1",
    "/ref/engine/desktop/desktop-2.0.2",
    "/ref/engine/sc5000/firmware-2.0.2",
    "/ref/engine/desktop/desktop-2.1.0",
    "/ref/engine/sc5000/firmware-2.1.0",
    "/ref/engine/sc5000/firmware-2.1.1",
    "/ref/engine/sc5000/firmware-2.1.2",
    "/ref/engine/desktop/desktop-2.2.0",
    "/ref/engine/sc5000/firmware-2.2.0",
    "/ref/engine/desktop/desktop-2.2.1",
    "/ref/engine/sc5000/firmware-2.2.1",
    "/ref/engine/desktop/desktop-2.2.2",
    "/ref/engine/sc5000/firmware-2.2.2",
    "/ref/engine/desktop/desktop-2.3.0",
    "/ref/engine/sc5000/firmware-2.3.0",
    "/ref/engine/desktop/desktop-2.3.1",
    "/ref/engine/sc5000/firmware-2.3.1",
    "/ref/engine/desktop/desktop-2.3.2",
    "/ref/engine/sc5000/firmware-2.3.2",
    "/ref/engine/desktop/desktop-2.4.0",
    "/ref/engine/sc5000/firmware-2.4.0",
    "/ref/engine/desktop/desktop-3.0.0",
    "/ref/engine/sc5000/firmware-3.0.0",
};
}  // anonymous namespace


BOOST_TEST_DECORATOR(* utf::description(
    "database::verify() with 'reference scripts' for all supported versions"))
BOOST_DATA_TEST_CASE(
    verify__reference_scripts__no_throw, ref_script_dirs, ref_script_dir)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;

    {
        // Arrange
        auto script_path = std::string{STRINGIFY(TESTDATA_DIR) "/"} +
                           ref_script_dir;

        BOOST_TEST_CHECKPOINT("(" << ref_script_dir << ") Creating DB...");
        auto db =
            e::create_database_from_scripts(
            tmp_loc.temp_dir, script_path);

        // Act
        BOOST_TEST_CHECKPOINT("(" << ref_script_dir << ") Verifying DB...");
        db.verify();

        // Assert
        BOOST_CHECK_EQUAL(db.directory(), tmp_loc.temp_dir);
    }
}
