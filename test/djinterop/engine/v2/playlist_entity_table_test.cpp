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

#define BOOST_TEST_MODULE engine_v2_playlist_entity_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <cstdint>
#include <sstream>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

#include "../../boost_test_printable.hpp"
#include "../../boost_test_utils.hpp"
#include "../../temporary_directory.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

const constexpr int64_t EXAMPLE_TRACK_ID = 12345;

BOOST_TEST_DECORATOR(*utf::description("add_back() to empty playlist"))
BOOST_DATA_TEST_CASE(
    add_back__empty_playlist__adds, e::all_v2_versions, version)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(version);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto playlist_entity_tbl = library.playlist_entity();
    ev2::playlist_row p_row{
        ev2::PLAYLIST_ROW_ID_NONE, "Example playlist", ev2::PARENT_LIST_ID_NONE,
        true};
    auto p_id = playlist_tbl.add(p_row);
    ev2::playlist_entity_row pe_row{
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID, db_uuid};

    // Act
    auto pe_id = playlist_entity_tbl.add_back(pe_row);

    // Assert
    auto entities = playlist_entity_tbl.get_for_list(p_id);
    auto iter = entities.begin();
    BOOST_REQUIRE(iter != entities.end());
    BOOST_CHECK_EQUAL(iter->id, pe_id);
    BOOST_CHECK_EQUAL(iter->list_id, p_id);
    BOOST_CHECK_EQUAL(iter->database_uuid, db_uuid);
    BOOST_CHECK_EQUAL(
        iter->next_entity_id, ev2::PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID);
    BOOST_CHECK_EQUAL(iter->membership_reference, pe_row.membership_reference);
    ++iter;
    BOOST_REQUIRE(iter == entities.end());
}
