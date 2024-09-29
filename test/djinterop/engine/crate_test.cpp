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

#define BOOST_TEST_MODULE crate_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>

#include <djinterop/crate.hpp>
#include <djinterop/database.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "example_track_data.hpp"

namespace e = djinterop::engine;
namespace utf = boost::unit_test;

namespace
{
std::vector<std::string> valid_crate_names{"This is a crate", "1234", "."};
std::vector<std::string> invalid_crate_names{"", "Contains ; semicolon"};
}  // namespace

BOOST_TEST_DECORATOR(
    *utf::description("crate::crate() for all supported schema versions"))
BOOST_DATA_TEST_CASE(ctor_copy__copies, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Copying crate via copy ctor...");
    djinterop::crate copy{crate};

    // Assert
    // TODO (mr-smidge): Implement operator== on crate and use that instead.
    BOOST_CHECK_EQUAL(copy.id(), crate.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::operator=() for all supported schema versions"))
BOOST_DATA_TEST_CASE(op_copy_assign__copies, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Copying crate via copy ctor...");
    auto copy = crate;

    // Assert
    // TODO (mr-smidge): Implement operator== on crate and use that instead.
    BOOST_CHECK_EQUAL(copy.id(), crate.id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::add_track() to empty crate for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_track__to_empty_crate__adds, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Adding example track to crate...");
    crate.add_track(track);

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 1);
    // TODO (mr-smidge): Implement operator== on track and use that instead.
    BOOST_CHECK_EQUAL(tracks[0].id(), track.id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::add_track() to non-empty crate for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    add_track__to_nonempty_crate__adds, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    crate.add_track(track1);

    // Act
    crate.add_track(track2);

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 2);
    // Note: no guarantee on ordering.
    // TODO (mr-smidge): Implement operator== on track and use that instead.
    bool track1_first =
        tracks[0].id() == track1.id() && tracks[1].id() == track2.id();
    bool track2_first =
        tracks[0].id() == track2.id() && tracks[1].id() == track1.id();
    BOOST_CHECK(track1_first || track2_first);
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::add_tracks() for all supported schema versions"))
BOOST_DATA_TEST_CASE(add_tracks__adds, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot1{};
    djinterop::track_snapshot snapshot2{};
    populate_track_snapshot(
        snapshot1, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    populate_track_snapshot(
        snapshot2, example_track_data_variation::basic_metadata_only_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example tracks...");
    auto track1 = db.create_track(snapshot1);
    auto track2 = db.create_track(snapshot2);
    std::vector<djinterop::track> tracks{track1, track2};

    // Act
    crate.add_tracks(std::begin(tracks), std::end(tracks));

    // Assert
    auto added_tracks = crate.tracks();
    BOOST_CHECK_EQUAL(added_tracks.size(), tracks.size());
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::children() on empty crate for all supported schema versions"))
BOOST_DATA_TEST_CASE(children__empty__none, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    auto children = crate.children();

    // Assert
    BOOST_CHECK_EQUAL(0, children.size());
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::clear_tracks() on empty crate for all supported schema versions"))
BOOST_DATA_TEST_CASE(clear_tracks__empty__no_effect, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    crate.clear_tracks();

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::clear_tracks() on non-empty crate for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(clear_tracks__nonempty__cleared, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example track...");
    auto track = db.create_track(snapshot);
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Adding example track to crate...");
    crate.add_track(track);

    // Act
    crate.clear_tracks();

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::create_sub_crate() for all supported schema versions"))
BOOST_DATA_TEST_CASE(create_sub_crate__creates, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example sub crate...");
    auto sub_crate = crate.create_sub_crate("Example Sub Crate");

    // Assert
    auto children = crate.children();
    BOOST_CHECK_EQUAL(children.size(), 1);
    // TODO (mr-smidge): Implement operator== on crate and use that instead.
    BOOST_CHECK_EQUAL(children[0].id(), sub_crate.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::remove_track() for track not in crate for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    remove_track__not_in_crate__no_effect, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Attempting to remove track from crate...");
    crate.remove_track(track);

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::remove_track() for track in crate for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(remove_track__in_crate__removed, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example track...");
    auto track = db.create_track(snapshot);
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Adding example track to crate...");
    crate.add_track(track);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Removing track from crate...");
    crate.remove_track(track);

    // Assert
    auto tracks = crate.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::set_name() with valid name for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_name__valid__sets, e::all_versions* valid_crate_names, version,
    crate_name)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Changing crate name...");
    crate.set_name(crate_name);

    // Assert
    BOOST_CHECK_EQUAL(crate.name(), crate_name);
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::set_name() with invalid name for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_name__invalid__throws, e::all_versions* invalid_crate_names, version,
    crate_name)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act/Assert
    BOOST_TEST_CHECKPOINT("(" << version << ") Changing crate name...");
    BOOST_CHECK_THROW(
        crate.set_name(crate_name), djinterop::crate_invalid_name);
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::set_parent() from root to root for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__root_to_root__no_effect, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Moveable Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Setting parent...");
    crate.set_parent(std::nullopt);

    // Assert
    BOOST_CHECK(crate.parent() == std::nullopt);
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::set_parent() from root to non-root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__root_to_nonroot__changes, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Moveable Crate");
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Creating another example crate...");
    auto new_parent_crate = db.create_root_crate("Example Root Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Setting parent...");
    crate.set_parent(new_parent_crate);

    // Assert
    BOOST_REQUIRE(crate.parent() != std::nullopt);
    BOOST_CHECK(crate.parent()->id() == new_parent_crate.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::set_parent() from non-root to root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__nonroot_to_root__changes, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto original_parent_crate = db.create_root_crate("Example Root Crate");
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example sub crate...");
    auto crate = original_parent_crate.create_sub_crate("Moveable Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Setting parent...");
    crate.set_parent(std::nullopt);

    // Assert
    BOOST_CHECK(crate.parent() == std::nullopt);
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::set_parent() from non-root to non-root for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(
    set_parent__nonroot_to_nonroot__changes, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto original_parent_crate = db.create_root_crate("Example Root Crate");
    BOOST_TEST_CHECKPOINT(
        "(" << version << ") Creating another example crate...");
    auto new_parent_crate = db.create_root_crate("Another Example Root Crate");
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example sub crate...");
    auto crate = original_parent_crate.create_sub_crate("Moveable Crate");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Setting parent...");
    crate.set_parent(new_parent_crate);

    // Assert
    BOOST_REQUIRE(crate.parent() != std::nullopt);
    BOOST_CHECK(crate.parent()->id() == new_parent_crate.id());
}

BOOST_TEST_DECORATOR(*utf::description(
    "crate::set_parent() to self for all supported schema versions"))
BOOST_DATA_TEST_CASE(set_parent__self__throws, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Act/Assert
    BOOST_TEST_CHECKPOINT("(" << version << ") Setting parent...");
    BOOST_CHECK_THROW(crate.set_parent(crate), djinterop::crate_invalid_parent);
}

// TODO (mr-smidge): Add a test case expecting an exception when setting a
//  crate's parent in such a way as to cause a cycle in the crate/parent graph.
//  A `crate_invalid_parent` exception should be thrown in this circumstance.

BOOST_TEST_DECORATOR(
    *utf::description("crate::sub_crate_by_name() for extant name for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(sub_crate_by_name__valid__finds, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example sub-crate...");
    auto sub_crate = crate.create_sub_crate("Example Sub Crate");

    // Act
    auto result = crate.sub_crate_by_name("Example Sub Crate");

    // Assert
    BOOST_REQUIRE(result);
    // TODO (mr-smidge): Implement operator== on crate and use that instead.
    BOOST_CHECK_EQUAL(result->id(), sub_crate.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("crate::sub_crate_by_name() for not-found name for all "
                      "supported schema versions"))
BOOST_DATA_TEST_CASE(sub_crate_by_name__invalid__none, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example crate...");
    auto crate = db.create_root_crate("Example Root Crate");
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating example sub-crate...");
    auto sub_crate = crate.create_sub_crate("Example Sub Crate");

    // Act
    auto result = crate.sub_crate_by_name("Does Not Exist");

    // Assert
    BOOST_CHECK(!result);
}
