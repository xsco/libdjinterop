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
#include <stdexcept>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

#include "../../boost_test_printable.hpp"
#include "../../boost_test_utils.hpp"
#include "../../temporary_directory.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

const constexpr int64_t EXAMPLE_TRACK_ID_1 = 12345;
const constexpr int64_t EXAMPLE_TRACK_ID_2 = 67890;

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
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID_1, db_uuid};

    // Act
    auto pe_id = playlist_entity_tbl.add_back(pe_row);

    // Assert
    auto entities = playlist_entity_tbl.get_for_list(p_id);
    auto iter = entities.begin();
    BOOST_REQUIRE(iter != entities.end());
    BOOST_CHECK_EQUAL(iter->id, pe_id);
    BOOST_CHECK_EQUAL(iter->list_id, p_id);
    BOOST_CHECK_EQUAL(iter->track_id, pe_row.track_id);
    BOOST_CHECK_EQUAL(iter->database_uuid, db_uuid);
    BOOST_CHECK_EQUAL(
        iter->next_entity_id, ev2::PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID);
    BOOST_CHECK_EQUAL(iter->membership_reference, pe_row.membership_reference);
    ++iter;
    BOOST_REQUIRE(iter == entities.end());
}

BOOST_TEST_DECORATOR(*utf::description("add_back() to non-empty playlist"))
BOOST_DATA_TEST_CASE(
    add_back__nonempty_playlist__adds, e::all_v2_versions, version)
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
    ev2::playlist_entity_row pe_row_1{
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID_1, db_uuid};
    auto pe_id_1 = playlist_entity_tbl.add_back(pe_row_1);
    ev2::playlist_entity_row pe_row_2{
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID_2, db_uuid};

    // Act
    auto pe_id_2 = playlist_entity_tbl.add_back(pe_row_2);

    // Assert
    auto entities = playlist_entity_tbl.get_for_list(p_id);
    auto iter = entities.begin();
    BOOST_REQUIRE(iter != entities.end());
    BOOST_CHECK_EQUAL(iter->id, pe_id_1);
    BOOST_CHECK_EQUAL(iter->list_id, p_id);
    BOOST_CHECK_EQUAL(iter->track_id, pe_row_1.track_id);
    BOOST_CHECK_EQUAL(iter->database_uuid, db_uuid);
    BOOST_CHECK_EQUAL(iter->next_entity_id, pe_id_2);
    BOOST_CHECK_EQUAL(iter->membership_reference, pe_row_1.membership_reference);
    ++iter;
    BOOST_REQUIRE(iter != entities.end());
    BOOST_CHECK_EQUAL(iter->id, pe_id_2);
    BOOST_CHECK_EQUAL(iter->list_id, p_id);
    BOOST_CHECK_EQUAL(iter->track_id, pe_row_2.track_id);
    BOOST_CHECK_EQUAL(iter->database_uuid, db_uuid);
    BOOST_CHECK_EQUAL(
        iter->next_entity_id, ev2::PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID);
    BOOST_CHECK_EQUAL(iter->membership_reference, pe_row_2.membership_reference);
    ++iter;
    BOOST_REQUIRE(iter == entities.end());
}

BOOST_TEST_DECORATOR(
    *utf::description("add_back() same track multiple times idempotently"))
BOOST_DATA_TEST_CASE(
    add_back__same_track_multple_no_throw__idempotent, e::all_v2_versions, version)
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
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID_1, db_uuid};
    auto pe_id = playlist_entity_tbl.add_back(pe_row);

    // Act
    playlist_entity_tbl.add_back(pe_row);

    // Assert
    auto entities = playlist_entity_tbl.get_for_list(p_id);
    auto iter = entities.begin();
    BOOST_REQUIRE(iter != entities.end());
    BOOST_CHECK_EQUAL(iter->id, pe_id);
    BOOST_CHECK_EQUAL(iter->list_id, p_id);
    BOOST_CHECK_EQUAL(iter->track_id, pe_row.track_id);
    BOOST_CHECK_EQUAL(iter->database_uuid, db_uuid);
    BOOST_CHECK_EQUAL(
        iter->next_entity_id, ev2::PLAYLIST_ENTITY_NO_NEXT_ENTITY_ID);
    BOOST_CHECK_EQUAL(iter->membership_reference, pe_row.membership_reference);
    ++iter;
    BOOST_REQUIRE(iter == entities.end());
}

BOOST_TEST_DECORATOR(
    *utf::description("add_back() same track multiple times with throw"))
BOOST_DATA_TEST_CASE(
    add_back__same_track_multple_throw__idempotent, e::all_v2_versions, version)
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
        ev2::PLAYLIST_ENTITY_ROW_ID_NONE, p_id, EXAMPLE_TRACK_ID_1, db_uuid};
    auto pe_id = playlist_entity_tbl.add_back(pe_row);

    // Act/Assert
    BOOST_CHECK_THROW(
        playlist_entity_tbl.add_back(pe_row, true), std::invalid_argument);
}
