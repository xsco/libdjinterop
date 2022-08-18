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
#include <djinterop/track_snapshot.hpp>

#define BOOST_TEST_MODULE track_snapshot_test
#include <boost/test/included/unit_test.hpp>

#include <cstdint>

BOOST_AUTO_TEST_CASE(ctor__default__no_id)
{
    // Arrange/Act
    djinterop::track_snapshot snapshot{};

    // Assert
    BOOST_CHECK(!snapshot.id);
}

BOOST_AUTO_TEST_CASE(ctor__specify_id__id_populated)
{
    // Arrange
    int64_t expected_id = 1234;

    // Act
    djinterop::track_snapshot snapshot{expected_id};

    // Assert
    BOOST_CHECK(snapshot.id);
    BOOST_CHECK_EQUAL(expected_id, *snapshot.id);
}

