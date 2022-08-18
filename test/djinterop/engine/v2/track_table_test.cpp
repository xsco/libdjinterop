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

#define BOOST_TEST_MODULE engine_v2_track_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>
#include <djinterop/engine/v2/track_table.hpp>

#include "../../boost_test_utils.hpp"
#include "../../temporary_directory.hpp"
#include "example_track_row_data.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

namespace
{
const std::vector<example_track_row_type> all_example_track_row_types{
    example_track_row_type::minimal_1,
    example_track_row_type::basic_metadata_only_1,
    example_track_row_type::fully_analysed_1,
};
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("add() with valid track row"))
BOOST_DATA_TEST_CASE(
    add__valid__adds, e::all_v2_versions * all_example_track_row_types, version,
    row_type)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;
    {
        // Arrange
        auto library = ev2::engine_library::create(tmp_loc.temp_dir, version);
        auto track_tbl = library.track();
        ev2::track_row row{0};
        populate_track_row(row_type, row);

        // Act
        auto id = track_tbl.add(row);

        // Assert
        BOOST_CHECK_NE(id, 0);
    }
}

BOOST_TEST_DECORATOR(*utf::description("add() with an existing id"))
BOOST_DATA_TEST_CASE(add__existing_id__throws, e::all_v2_versions, version)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;
    {
        // Arrange
        auto library = ev2::engine_library::create(tmp_loc.temp_dir, version);
        auto track_tbl = library.track();
        ev2::track_row row{123};

        // Act/Assert
        BOOST_CHECK_THROW(track_tbl.add(row), ev2::track_row_id_error);
    }
}
