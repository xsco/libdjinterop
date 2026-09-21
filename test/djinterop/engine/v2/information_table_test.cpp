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

#define BOOST_TEST_MODULE engine_v2_information_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

BOOST_TEST_DECORATOR(*utf::description("get() on a newly-created library"))
BOOST_DATA_TEST_CASE(
    get__new_library__row_at_id_one, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);

    // Act
    auto row = library.information().get();

    // Assert
    BOOST_CHECK_EQUAL(row.id, 1);
}
