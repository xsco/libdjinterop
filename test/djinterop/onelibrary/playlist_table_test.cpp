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

#define BOOST_TEST_MODULE onelibrary_playlist_table_test
#include <boost/test/included/unit_test.hpp>

#include <string>

#include <djinterop/onelibrary/v1/playlist_table.hpp>

#include "../boost_test_printable.hpp"
#include "onelibrary_fixture.hpp"

namespace utf = boost::unit_test;

namespace
{
/// A device holding a tree of playlists, built once for the whole module.
///
///     Sets                 (no parent recorded)
///      +- Warm Up          (sequence 1)
///      |   +- Peak Time    (sequence 1)
///      +- Cool Down        (sequence 2)
///     Practice             (parent of zero, which means the same as none)
///
/// The two spellings of a root are both present, as rekordbox writes both.
const onelibrary_device& device()
{
    static const onelibrary_device instance{{
        "INSERT INTO playlist (playlist_id, sequenceNo, name, "
        "playlist_id_parent) VALUES (1, 1, 'Sets', NULL), "
        "(2, 1, 'Warm Up', 1), (3, 2, 'Cool Down', 1), "
        "(4, 1, 'Peak Time', 2), (5, 2, 'Practice', 0);",

        "INSERT INTO content (content_id, title) VALUES (10, 'Alpha'), "
        "(11, 'Beta'), (12, 'Gamma');",

        // The second track of `Warm Up` is the one that `Peak Time` also
        // holds.
        "INSERT INTO playlist_content VALUES (2, 11, 1), (2, 10, 2), "
        "(4, 10, 1);",
    }};

    return instance;
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("get() reads one row of the tree"))
BOOST_AUTO_TEST_CASE(get__a_child__reads_its_row)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto row = playlists.get(2);

    // Assert
    BOOST_REQUIRE(row);
    BOOST_CHECK_EQUAL(row->id, 2);
    BOOST_CHECK_EQUAL(row->name, "Warm Up");
    BOOST_REQUIRE(row->parent_id);
    BOOST_CHECK_EQUAL(*row->parent_id, 1);
    BOOST_CHECK_EQUAL(row->sequence_number.value(), 1);
}

BOOST_TEST_DECORATOR(
    *utf::description("get() treats a parent of zero as no parent"))
BOOST_AUTO_TEST_CASE(get__a_parent_of_zero__reads_as_no_parent)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto row = playlists.get(5);

    // Assert
    BOOST_REQUIRE(row);
    BOOST_CHECK(!row->parent_id);
}

BOOST_TEST_DECORATOR(*utf::description("get() for a row that is not there"))
BOOST_AUTO_TEST_CASE(get__an_unknown_playlist__is_absent)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act / Assert
    BOOST_CHECK(!playlists.get(404));
    BOOST_CHECK(!playlists.exists(404));
    BOOST_CHECK(playlists.exists(1));
}

BOOST_TEST_DECORATOR(
    *utf::description("root_ids() finds both spellings of a root"))
BOOST_AUTO_TEST_CASE(root_ids__both_spellings__are_roots)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto ids = playlists.root_ids();

    // Assert
    BOOST_REQUIRE_EQUAL(ids.size(), 2u);
    BOOST_CHECK_EQUAL(ids[0], 1);
    BOOST_CHECK_EQUAL(ids[1], 5);
}

BOOST_TEST_DECORATOR(*utf::description("child_ids() is in sibling order"))
BOOST_AUTO_TEST_CASE(child_ids__several_siblings__are_in_sequence)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto ids = playlists.child_ids(1);

    // Assert
    BOOST_REQUIRE_EQUAL(ids.size(), 2u);
    BOOST_CHECK_EQUAL(ids[0], 2);
    BOOST_CHECK_EQUAL(ids[1], 3);
}

BOOST_TEST_DECORATOR(*utf::description("descendant_ids() is breadth first"))
BOOST_AUTO_TEST_CASE(descendant_ids__a_deep_tree__is_breadth_first)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto ids = playlists.descendant_ids(1);

    // Assert
    BOOST_REQUIRE_EQUAL(ids.size(), 3u);
    BOOST_CHECK_EQUAL(ids[0], 2);  // Warm Up
    BOOST_CHECK_EQUAL(ids[1], 3);  // Cool Down
    BOOST_CHECK_EQUAL(ids[2], 4);  // Peak Time, one level deeper
}

BOOST_TEST_DECORATOR(
    *utf::description("find_root() and find_child() look up by name"))
BOOST_AUTO_TEST_CASE(find__a_known_name__is_found_at_its_own_level)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act / Assert
    BOOST_CHECK(playlists.find_root("Sets") == 1);
    BOOST_CHECK(playlists.find_child(1, "Cool Down") == 3);

    // A child is not a root, and a name from elsewhere is not a child.
    BOOST_CHECK(!playlists.find_root("Warm Up"));
    BOOST_CHECK(!playlists.find_child(1, "Peak Time"));
}

BOOST_TEST_DECORATOR(*utf::description("track_ids() is in playlist order"))
BOOST_AUTO_TEST_CASE(track_ids__a_populated_playlist__is_in_order)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto ids = playlists.track_ids(2);

    // Assert: the sequence numbers, and not the order the rows were written.
    BOOST_REQUIRE_EQUAL(ids.size(), 2u);
    BOOST_CHECK_EQUAL(ids[0], 11);
    BOOST_CHECK_EQUAL(ids[1], 10);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlists_containing() for a track held twice"))
BOOST_AUTO_TEST_CASE(playlists_containing__a_shared_track__finds_each_holder)
{
    // Arrange
    const auto playlists = device().library.playlist();

    // Act
    const auto holders = playlists.playlists_containing(10);

    // Assert
    BOOST_REQUIRE_EQUAL(holders.size(), 2u);
    BOOST_CHECK_EQUAL(holders[0], 2);
    BOOST_CHECK_EQUAL(holders[1], 4);

    // A track in no playlist at all is held by nothing.
    BOOST_CHECK(playlists.playlists_containing(12).empty());
}

BOOST_TEST_DECORATOR(
    *utf::description("descendant_ids() terminates on a tree with a cycle"))
BOOST_AUTO_TEST_CASE(descendant_ids__a_cycle__terminates)
{
    // Arrange: nothing in the schema stops a playlist being its own ancestor.
    const onelibrary_device cyclic{{
        "INSERT INTO playlist (playlist_id, sequenceNo, name, "
        "playlist_id_parent) VALUES (1, 1, 'One', 2), (2, 1, 'Two', 1);",
    }};
    const auto playlists = cyclic.library.playlist();

    // Act
    const auto ids = playlists.descendant_ids(1);

    // Assert
    BOOST_REQUIRE(!ids.empty());
    BOOST_CHECK_EQUAL(ids[0], 2);
}
