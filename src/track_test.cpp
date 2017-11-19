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
#include <engineprime/track.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE track_test

#include <boost/test/unit_test.hpp>
#include <string>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el1"};


BOOST_AUTO_TEST_CASE (all_track_ids__sample_db__expected_ids)
{
    // Arrange
    ep::database db{sample_path};
    
    // Act
    auto results = ep::all_track_ids(db);
    
    // Assert
    BOOST_CHECK_EQUAL(results.size(), 7);
    BOOST_CHECK_EQUAL(results[0], 1);
    BOOST_CHECK_EQUAL(results[1], 2);
    BOOST_CHECK_EQUAL(results[2], 3);
    BOOST_CHECK_EQUAL(results[3], 4);
    BOOST_CHECK_EQUAL(results[4], 5);
    BOOST_CHECK_EQUAL(results[5], 6);
    BOOST_CHECK_EQUAL(results[6], 7);
}

