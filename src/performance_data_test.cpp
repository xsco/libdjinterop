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
#include <engineprime/performance_data.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE schema_test 

#include <iostream>
#include <cstdio>
#include <boost/test/unit_test.hpp>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el2"};

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_fields)
{
	// Arrange
	ep::database db{sample_path};

	// Act
	ep::performance_data p{db, 1};

    // Assert
    BOOST_CHECK_EQUAL(p.track_id(), 1);
    BOOST_CHECK_EQUAL(p.sample_rate(), 44100.0d);
    BOOST_CHECK_EQUAL(p.total_samples(), 17452800);
    BOOST_CHECK_EQUAL(p.average_loudness(), 0.520831584930419921875d);
    // TODO - check other fields
}

