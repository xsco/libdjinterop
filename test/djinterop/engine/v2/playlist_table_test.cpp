/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either schema 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE engine_v2_playlist_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <chrono>
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

namespace
{
ev2::playlist_row make_playlist_row(
    const std::string& title, int64_t parent_list_id = ev2::PARENT_LIST_ID_NONE,
    int64_t next_list_id = ev2::PLAYLIST_NO_NEXT_LIST_ID)
{
    return ev2::playlist_row{ev2::PLAYLIST_ROW_ID_NONE,
                             title,
                             parent_list_id,
                             true,
                             next_list_id,
                             std::chrono::system_clock::now(),
                             true};
}

ev2::playlist_row make_playlist_row(
    int64_t id, const std::string& title,
    int64_t parent_list_id = ev2::PARENT_LIST_ID_NONE,
    int64_t next_list_id = ev2::PLAYLIST_NO_NEXT_LIST_ID)
{
    return ev2::playlist_row{
        id,   title,        parent_list_id,
        true, next_list_id, std::chrono::system_clock::now(),
        true};
}
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("add() root playlist to empty database"))
BOOST_DATA_TEST_CASE(add__empty_root__adds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto row = make_playlist_row("Example");

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto all_ids = playlist_tbl.all_ids();
    std::set<int64_t> all_ids_set{all_ids.begin(), all_ids.end()};
    BOOST_CHECK_EQUAL(1, all_ids_set.size());
    BOOST_CHECK(all_ids_set.find(id) != all_ids_set.end());
}

BOOST_TEST_DECORATOR(
    *utf::description("add() root playlist to non-empty database"))
BOOST_DATA_TEST_CASE(add__nonempty_root__adds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto prior_row = make_playlist_row("Example (Prior)");
    auto prior_id = playlist_tbl.add(prior_row);
    auto row = make_playlist_row("Example");

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto all_ids = playlist_tbl.all_ids();
    std::set<int64_t> all_ids_set{all_ids.begin(), all_ids.end()};
    BOOST_CHECK_EQUAL(2, all_ids_set.size());
    BOOST_CHECK(all_ids_set.find(prior_id) != all_ids_set.end());
    BOOST_CHECK(all_ids_set.find(id) != all_ids_set.end());
}

BOOST_TEST_DECORATOR(
    *utf::description("add() non-root playlist to empty database"))
BOOST_DATA_TEST_CASE(add__empty_nonroot__adds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto row = make_playlist_row("Example", parent_id);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto all_ids = playlist_tbl.all_ids();
    std::set<int64_t> all_ids_set{all_ids.begin(), all_ids.end()};
    BOOST_CHECK_EQUAL(2, all_ids_set.size());
    BOOST_CHECK(all_ids_set.find(parent_id) != all_ids_set.end());
    BOOST_CHECK(all_ids_set.find(id) != all_ids_set.end());
}

BOOST_TEST_DECORATOR(
    *utf::description("add() non-root playlist to non-empty database"))
BOOST_DATA_TEST_CASE(add__nonempty_nonroot__adds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto prior_row = make_playlist_row("Example (Prior)", parent_id);
    auto prior_id = playlist_tbl.add(prior_row);
    auto row = make_playlist_row("Example", parent_id);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto all_ids = playlist_tbl.all_ids();
    std::set<int64_t> all_ids_set{all_ids.begin(), all_ids.end()};
    BOOST_CHECK_EQUAL(3, all_ids_set.size());
    BOOST_CHECK(all_ids_set.find(parent_id) != all_ids_set.end());
    BOOST_CHECK(all_ids_set.find(prior_id) != all_ids_set.end());
    BOOST_CHECK(all_ids_set.find(id) != all_ids_set.end());
}

BOOST_TEST_DECORATOR(*utf::description("root_ids() after insertion at end"))
BOOST_DATA_TEST_CASE(
    root_ids__after_insert_end__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto prior_row_1 = make_playlist_row("A");
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B");
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row = make_playlist_row(
        "Example", ev2::PARENT_LIST_ID_NONE, ev2::PLAYLIST_NO_NEXT_LIST_ID);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("root_ids() after insertion at beginning"))
BOOST_DATA_TEST_CASE(
    root_ids__after_insert_beginning__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto prior_row_1 = make_playlist_row("A");
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B");
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row =
        make_playlist_row("Example", ev2::PARENT_LIST_ID_NONE, prior_id_1);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(*utf::description("root_ids() after insertion at middle"))
BOOST_DATA_TEST_CASE(
    root_ids__after_insert_middle__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto prior_row_1 = make_playlist_row("A");
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B");
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row =
        make_playlist_row("Example", ev2::PARENT_LIST_ID_NONE, prior_id_2);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(*utf::description("child_ids() after insertion at end"))
BOOST_DATA_TEST_CASE(
    child_ids__after_insert_end__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto prior_row_1 = make_playlist_row("A", parent_id);
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B", parent_id);
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row =
        make_playlist_row("Example", parent_id, ev2::PLAYLIST_NO_NEXT_LIST_ID);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto child_ids = playlist_tbl.child_ids(parent_id);
    auto iter = std::begin(child_ids);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter == std::end(child_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("child_ids() after insertion at beginning"))
BOOST_DATA_TEST_CASE(
    child_ids__after_insert_beginning__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto prior_row_1 = make_playlist_row("A", parent_id);
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B", parent_id);
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row =
        make_playlist_row("Example", parent_id, prior_id_1);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto child_ids = playlist_tbl.child_ids(parent_id);
    auto iter = std::begin(child_ids);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(child_ids));
}

BOOST_TEST_DECORATOR(*utf::description("child_ids() after insertion at middle"))
BOOST_DATA_TEST_CASE(
    child_ids__after_insert_middle__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto prior_row_1 = make_playlist_row("A", parent_id);
    auto prior_id_1 = playlist_tbl.add(prior_row_1);
    auto prior_row_2 = make_playlist_row("B", parent_id);
    auto prior_id_2 = playlist_tbl.add(prior_row_2);
    auto row =
        make_playlist_row("Example", parent_id, prior_id_2);

    // Act
    auto id = playlist_tbl.add(row);

    // Assert
    auto child_ids = playlist_tbl.child_ids(parent_id);
    auto iter = std::begin(child_ids);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(id, *iter++);
    BOOST_REQUIRE(iter != std::end(child_ids));
    BOOST_CHECK_EQUAL(prior_id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(child_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from beginning to middle, same parent"))
BOOST_DATA_TEST_CASE(
    update__beginning_to_middle__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto row_1 = make_playlist_row("A");
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B");
    auto id_2 = playlist_tbl.add(row_2);
    auto row_3 = make_playlist_row("C");
    auto id_3 = playlist_tbl.add(row_3);
    auto row = make_playlist_row(id_1, "A", ev2::PARENT_LIST_ID_NONE, id_3);

    // Act
    playlist_tbl.update(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_2, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_3, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from end to middle, same parent"))
BOOST_DATA_TEST_CASE(
    update__end_to_middle__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto row_1 = make_playlist_row("A");
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B");
    auto id_2 = playlist_tbl.add(row_2);
    auto row_3 = make_playlist_row("C");
    auto id_3 = playlist_tbl.add(row_3);
    auto row = make_playlist_row(id_3, "C", ev2::PARENT_LIST_ID_NONE, id_2);

    // Act
    playlist_tbl.update(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_3, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from middle to beginning, same parent"))
BOOST_DATA_TEST_CASE(
    update__middle_to_beginning__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto row_1 = make_playlist_row("A");
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B");
    auto id_2 = playlist_tbl.add(row_2);
    auto row_3 = make_playlist_row("C");
    auto id_3 = playlist_tbl.add(row_3);
    auto row = make_playlist_row(id_2, "B", ev2::PARENT_LIST_ID_NONE, id_1);

    // Act
    playlist_tbl.update(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_2, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_3, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from middle to end, same parent"))
BOOST_DATA_TEST_CASE(
    update__middle_to_end__ordered, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto row_1 = make_playlist_row("A");
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B");
    auto id_2 = playlist_tbl.add(row_2);
    auto row_3 = make_playlist_row("C");
    auto id_3 = playlist_tbl.add(row_3);
    auto row = make_playlist_row(
        id_2, "B", ev2::PARENT_LIST_ID_NONE, ev2::PLAYLIST_NO_NEXT_LIST_ID);

    // Act
    playlist_tbl.update(row);

    // Assert
    auto root_ids = playlist_tbl.root_ids();
    auto iter = std::begin(root_ids);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_1, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_3, *iter++);
    BOOST_REQUIRE(iter != std::end(root_ids));
    BOOST_CHECK_EQUAL(id_2, *iter++);
    BOOST_REQUIRE(iter == std::end(root_ids));
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from only child to root end"))
BOOST_DATA_TEST_CASE(
    update__only_child_to_root_end__moved, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto row_1 = make_playlist_row("A", parent_id);
    auto id_1 = playlist_tbl.add(row_1);
    auto row = make_playlist_row(
        id_1, "A", ev2::PARENT_LIST_ID_NONE, ev2::PLAYLIST_NO_NEXT_LIST_ID);

    // Act
    playlist_tbl.update(row);

    // Assert
    {
        auto root_ids = playlist_tbl.root_ids();
        auto iter = std::begin(root_ids);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(parent_id, *iter++);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(id_1, *iter++);
        BOOST_REQUIRE(iter == std::end(root_ids));
    }
    {
        auto child_ids = playlist_tbl.child_ids(parent_id);
        auto iter = std::begin(child_ids);
        BOOST_REQUIRE(iter == std::end(child_ids));
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from first child to root end"))
BOOST_DATA_TEST_CASE(
    update__first_child_to_root_end__moved, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto row_1 = make_playlist_row("A", parent_id);
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B", parent_id);
    auto id_2 = playlist_tbl.add(row_2);
    auto row = make_playlist_row(
        id_1, "A", ev2::PARENT_LIST_ID_NONE, ev2::PLAYLIST_NO_NEXT_LIST_ID);

    // Act
    playlist_tbl.update(row);

    // Assert
    {
        auto root_ids = playlist_tbl.root_ids();
        auto iter = std::begin(root_ids);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(parent_id, *iter++);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(id_1, *iter++);
        BOOST_REQUIRE(iter == std::end(root_ids));
    }
    {
        auto child_ids = playlist_tbl.child_ids(parent_id);
        auto iter = std::begin(child_ids);
        BOOST_REQUIRE(iter != std::end(child_ids));
        BOOST_CHECK_EQUAL(id_2, *iter++);
        BOOST_REQUIRE(iter == std::end(child_ids));
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("update() from last child to root beginning"))
BOOST_DATA_TEST_CASE(
    update__last_child_to_root_beginning__moved, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto db_uuid = library.information().get().uuid;
    auto playlist_tbl = library.playlist();
    auto parent_row = make_playlist_row("Parent");
    auto parent_id = playlist_tbl.add(parent_row);
    auto row_1 = make_playlist_row("A", parent_id);
    auto id_1 = playlist_tbl.add(row_1);
    auto row_2 = make_playlist_row("B", parent_id);
    auto id_2 = playlist_tbl.add(row_2);
    auto row = make_playlist_row(
        id_2, "B", ev2::PARENT_LIST_ID_NONE, parent_id);

    // Act
    playlist_tbl.update(row);

    // Assert
    {
        auto root_ids = playlist_tbl.root_ids();
        auto iter = std::begin(root_ids);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(id_2, *iter++);
        BOOST_REQUIRE(iter != std::end(root_ids));
        BOOST_CHECK_EQUAL(parent_id, *iter++);
        BOOST_REQUIRE(iter == std::end(root_ids));
    }
    {
        auto child_ids = playlist_tbl.child_ids(parent_id);
        auto iter = std::begin(child_ids);
        BOOST_REQUIRE(iter != std::end(child_ids));
        BOOST_CHECK_EQUAL(id_1, *iter++);
        BOOST_REQUIRE(iter == std::end(child_ids));
    }
}
