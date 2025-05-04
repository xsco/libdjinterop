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

#define BOOST_TEST_MODULE playlist_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>

#include <djinterop/playlist.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "../boost_test_utils.hpp"
#include "example_track_data.hpp"

namespace e = djinterop::engine;
namespace utf = boost::unit_test;

namespace
{
std::vector<std::string> valid_playlist_names{"This is a playlist", "1234", "."};
std::vector<std::string> invalid_playlist_names{"", "Contains ; semicolon"};
}  // namespace

BOOST_TEST_DECORATOR(
    *utf::description("playlist::playlist() for all supported schema versions"))
BOOST_DATA_TEST_CASE(ctor_copy__copies, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Copying playlist via copy ctor...");
    djinterop::playlist copy{playlist};

    // Assert
    BOOST_CHECK(copy == playlist);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::operator=() for all supported schema versions"))
BOOST_DATA_TEST_CASE(op_copy_assign__copies, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Copying playlist via copy ctor...");
    auto copy = playlist;

    // Assert
    BOOST_CHECK(copy == playlist);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::add_track_back() to empty playlist for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_track_back__to_empty_playlist__adds, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Adding example track to playlist...");
    playlist.add_track_back(track);

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 1);
    // TODO (mr-smidge): Implement operator== on track and use that instead.
    BOOST_CHECK_EQUAL(tracks[0].id(), track.id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::add_track_back() to non-empty playlist for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    add_track_back__to_nonempty_playlist__adds, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    playlist.add_track_back(track1);

    // Act
    playlist.add_track_back(track2);

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 2);
    // TODO (mr-smidge): Implement operator== on track and use that instead.
    BOOST_CHECK_EQUAL(tracks[0].id(), track1.id());
    BOOST_CHECK_EQUAL(tracks[1].id(), track2.id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::add_track_back() when already in playlist for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    add_track_back_already_in_playlist_accepts_or_throws, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot1{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example track...");
    auto track1 = db.create_track(snapshot1);
    playlist.add_track_back(track1);

    if (db.supports_feature(djinterop::feature::playlists_support_duplicate_tracks))
    {
        // Act
        playlist.add_track_back(track1);

        // Assert
        auto tracks = playlist.tracks();
        BOOST_REQUIRE_EQUAL(2, tracks.size());
        BOOST_CHECK_EQUAL(track1.id(), tracks[0].id());
        BOOST_CHECK_EQUAL(track1.id(), tracks[1].id());
    }
    else
    {
        // Act/Assert
        BOOST_CHECK_THROW(playlist.add_track_back(track1), djinterop::track_already_in_playlist);
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::add_tracks_back() for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_tracks_back__adds, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    std::vector<djinterop::track> tracks{track1, track2};

    // Act
    playlist.add_tracks_back(std::begin(tracks), std::end(tracks));

    // Assert
    auto added_tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(added_tracks.size(), tracks.size());
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::add_track_after() at end for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_track_after_last_adds_at_end, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    playlist.add_track_back(track1);

    // Act
    playlist.add_track_after(track2, track1);

    // Assert
    auto added_tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(2, added_tracks.size());
    BOOST_CHECK_EQUAL(track1.id(), added_tracks[0].id());
    BOOST_CHECK_EQUAL(track2.id(), added_tracks[1].id());
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::add_track_after() in middle for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_track_after_middle_adds_in_middle, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    djinterop::track_snapshot snapshot3{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, schema);
    populate_track_snapshot(
        snapshot3, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    auto track3 = db.create_track(snapshot3);
    playlist.add_track_back(track1);
    playlist.add_track_back(track2);

    // Act
    playlist.add_track_after(track3, track1);

    // Assert
    auto added_tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(3, added_tracks.size());
    BOOST_CHECK_EQUAL(track1.id(), added_tracks[0].id());
    BOOST_CHECK_EQUAL(track3.id(), added_tracks[1].id());
    BOOST_CHECK_EQUAL(track2.id(), added_tracks[2].id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::children() on empty playlist for all supported schema versions"))
BOOST_DATA_TEST_CASE(children__empty__none, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    auto children = playlist.children();

    // Assert
    BOOST_CHECK_EQUAL(0, children.size());
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::clear_tracks() on empty playlist for all supported schema versions"))
BOOST_DATA_TEST_CASE(clear_tracks__empty__no_effect, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    playlist.clear_tracks();

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::clear_tracks() on non-empty playlist for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(clear_tracks__nonempty__cleared, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example track...");
    auto track = db.create_track(snapshot);
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Adding example track to playlist...");
    playlist.add_track_back(track);

    // Act
    playlist.clear_tracks();

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::create_sub_playlist() for all supported schema versions"))
BOOST_DATA_TEST_CASE(create_sub_playlist__creates, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example sub playlist...");
    auto sub_playlist = playlist.create_sub_playlist("Example Sub Playlist");

    // Assert
    auto children = playlist.children();
    BOOST_CHECK_EQUAL(children.size(), 1);
    BOOST_CHECK(children[0] == sub_playlist);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::remove_track() for track not in playlist for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    remove_track__not_in_playlist__no_effect, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Attempting to remove track from playlist...");
    playlist.remove_track(track);

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::remove_track() for track in playlist for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(remove_track__in_playlist__removed, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example track...");
    auto track = db.create_track(snapshot);
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Adding example track to playlist...");
    playlist.add_track_back(track);

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Removing track from playlist...");
    playlist.remove_track(track);

    // Assert
    auto tracks = playlist.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::set_name() with valid name for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_name__valid__sets, e::supported_schemas* valid_playlist_names, schema,
    playlist_name)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Changing playlist name...");
    playlist.set_name(playlist_name);

    // Assert
    BOOST_CHECK_EQUAL(playlist.name(), playlist_name);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::set_name() with invalid name for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_name__invalid__throws, e::supported_schemas* invalid_playlist_names, schema,
    playlist_name)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act/Assert
    BOOST_TEST_CHECKPOINT("(" << schema << ") Changing playlist name...");
    BOOST_CHECK_THROW(
        playlist.set_name(playlist_name), djinterop::playlist_invalid_name);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::set_parent() from root to root for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__root_to_root__no_effect, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Moveable Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Setting parent...");
    playlist.set_parent(std::nullopt);

    // Assert
    BOOST_CHECK(playlist.parent() == std::nullopt);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::set_parent() from root to non-root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__root_to_nonroot__changes, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Moveable Playlist");
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Creating another example playlist...");
    auto new_parent_playlist = db.create_root_playlist("Example Root Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Setting parent...");
    playlist.set_parent(new_parent_playlist);

    // Assert
    BOOST_REQUIRE(playlist.parent() != std::nullopt);
    BOOST_CHECK(playlist.parent() == new_parent_playlist);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::set_parent() from non-root to root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__nonroot_to_root__changes, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto original_parent_playlist = db.create_root_playlist("Example Root Playlist");
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example sub playlist...");
    auto playlist = original_parent_playlist.create_sub_playlist("Moveable Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Setting parent...");
    playlist.set_parent(std::nullopt);

    // Assert
    BOOST_CHECK(playlist.parent() == std::nullopt);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::set_parent() from non-root to non-root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__nonroot_to_nonroot__changes, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto original_parent_playlist = db.create_root_playlist("Example Root Playlist");
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Creating another example playlist...");
    auto new_parent_playlist = db.create_root_playlist("Another Example Root Playlist");
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example sub playlist...");
    auto playlist = original_parent_playlist.create_sub_playlist("Moveable Playlist");

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Setting parent...");
    playlist.set_parent(new_parent_playlist);

    // Assert
    BOOST_REQUIRE(playlist.parent() != std::nullopt);
    BOOST_CHECK(playlist.parent() == new_parent_playlist);
}

BOOST_TEST_DECORATOR(*utf::description(
    "playlist::set_parent() to self for all supported schema versions"))
BOOST_DATA_TEST_CASE(set_parent__self__throws, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");

    // Act/Assert
    BOOST_TEST_CHECKPOINT("(" << schema << ") Setting parent...");
    BOOST_CHECK_THROW(playlist.set_parent(playlist), djinterop::playlist_invalid_parent);
}

// TODO (mr-smidge): Add a test case expecting an exception when setting a
//  playlist's parent in such a way as to cause a cycle in the playlist/parent graph.
//  A `playlist_invalid_parent` exception should be thrown in this circumstance.

BOOST_TEST_DECORATOR(
    *utf::description("playlist::sub_playlist_by_name() for extant name for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(sub_playlist_by_name__valid__finds, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example sub-playlist...");
    auto sub_playlist = playlist.create_sub_playlist("Example Sub Playlist");

    // Act
    auto result = playlist.sub_playlist_by_name("Example Sub Playlist");

    // Assert
    BOOST_REQUIRE(result);
    BOOST_CHECK(*result == sub_playlist);
}

BOOST_TEST_DECORATOR(
    *utf::description("playlist::sub_playlist_by_name() for not-found name for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(sub_playlist_by_name__invalid__none, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);
    DJINTEROP_TEST_NEEDS_FEATURE(db, djinterop::feature::supports_nested_playlists);
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example playlist...");
    auto playlist = db.create_root_playlist("Example Root Playlist");
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating example sub-playlist...");
    auto sub_playlist = playlist.create_sub_playlist("Example Sub Playlist");

    // Act
    auto result = playlist.sub_playlist_by_name("Does Not Exist");

    // Assert
    BOOST_CHECK(!result);
}
