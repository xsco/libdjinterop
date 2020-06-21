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

#define BOOST_TEST_MODULE enginelibrary_test
#include <boost/test/included/unit_test.hpp>

#include <djinterop/enginelibrary.hpp>

namespace el = djinterop::enginelibrary;


BOOST_AUTO_TEST_CASE(operators_equality__various__expected)
{
    // Arrange/Act/Assert
    BOOST_CHECK(el::version_1_6_0 == el::version_1_6_0);
    BOOST_CHECK(el::version_1_7_1 == el::version_1_7_1);
    BOOST_CHECK(!(el::version_1_6_0 == el::version_1_7_1));
    BOOST_CHECK(!(el::version_1_7_1 == el::version_1_6_0));
    BOOST_CHECK(el::version_1_6_0 != el::version_1_7_1);
    BOOST_CHECK(el::version_1_7_1 != el::version_1_6_0);
    BOOST_CHECK(!(el::version_1_6_0 != el::version_1_6_0));
    BOOST_CHECK(!(el::version_1_7_1 != el::version_1_7_1));
}

BOOST_AUTO_TEST_CASE(operators_ordering__various__expected)
{
    // Arrange/Act/Assert
    BOOST_CHECK(el::version_1_6_0 <= el::version_1_6_0);
    BOOST_CHECK(el::version_1_7_1 <= el::version_1_7_1);
    BOOST_CHECK(el::version_1_6_0 <= el::version_1_7_1);
    BOOST_CHECK(!(el::version_1_7_1 <= el::version_1_6_0));
    BOOST_CHECK(!(el::version_1_6_0 < el::version_1_6_0));
    BOOST_CHECK(!(el::version_1_7_1 < el::version_1_7_1));
    BOOST_CHECK(el::version_1_6_0 < el::version_1_7_1);
    BOOST_CHECK(!(el::version_1_7_1 < el::version_1_6_0));
    BOOST_CHECK(el::version_1_6_0 >= el::version_1_6_0);
    BOOST_CHECK(el::version_1_7_1 >= el::version_1_7_1);
    BOOST_CHECK(!(el::version_1_6_0 >= el::version_1_7_1));
    BOOST_CHECK(el::version_1_7_1 >= el::version_1_6_0);
    BOOST_CHECK(!(el::version_1_6_0 > el::version_1_6_0));
    BOOST_CHECK(!(el::version_1_7_1 > el::version_1_7_1));
    BOOST_CHECK(!(el::version_1_6_0 > el::version_1_7_1));
    BOOST_CHECK(el::version_1_7_1 > el::version_1_6_0);
}
