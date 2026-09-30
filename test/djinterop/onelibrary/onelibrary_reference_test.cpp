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

#define BOOST_TEST_MODULE onelibrary_reference_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>

#include <djinterop/onelibrary/onelibrary.hpp>

#include "../temporary_directory.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace utf = boost::unit_test;
namespace onelib = djinterop::onelibrary;

namespace
{
const std::vector<std::string> ref_script_dirs{
    "/ref/onelibrary/djay-5-6-8",
};
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description(
    "onelibrary::create_from_scripts() with 'reference scripts'"))
BOOST_DATA_TEST_CASE(
    create_from_scripts__reference_scripts__valid, ref_script_dirs,
    ref_script_dir)
{
    temporary_directory tmp_loc;

    auto script_dir = std::string{STRINGIFY(TESTDATA_DIR) "/"} + ref_script_dir;

    auto lib =
        onelib::onelibrary::create_from_scripts(tmp_loc.temp_dir, script_dir);

    BOOST_CHECK_NO_THROW(lib.verify());
    BOOST_CHECK_EQUAL(lib.directory(), tmp_loc.temp_dir);
}
