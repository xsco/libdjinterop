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
#include <djinterop/database.hpp>

#define BOOST_TEST_MODULE database_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>

#include <djinterop/crate.hpp>
#include <djinterop/engine/engine.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "example_track_data.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace utf = boost::unit_test;
namespace e = djinterop::engine;


BOOST_TEST_DECORATOR(* utf::description(
    "database::create_root_crate() for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    create_root_crate__supported_version__creates, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating root crate...");
    auto crate = db.create_root_crate("Example Root Crate");

    // Assert
    BOOST_CHECK_NE(crate.id(), 0);
    BOOST_CHECK(crate.parent() == std::nullopt);
}

BOOST_TEST_DECORATOR(* utf::description(
    "database::create_root_crate_after() for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    create_root_crate_after__supported_version__creates, e::all_v2_versions,
    version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);
    auto crate_a = db.create_root_crate("Example Root Crate A");
    auto crate_b = db.create_root_crate("Example Root Crate B");
    auto crate_d = db.create_root_crate("Example Root Crate D");
    auto crate_e = db.create_root_crate("Example Root Crate E");

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating root crate after another...");
    auto crate = db.create_root_crate_after("Example Root Crate C", crate_b);

    // Assert
    auto crates = db.root_crates();
    BOOST_CHECK_EQUAL(5, crates.size());
    BOOST_CHECK(crates[0].id() == crate_a.id());
    BOOST_CHECK(crates[1].id() == crate_b.id());
    BOOST_CHECK(crates[2].id() == crate.id());
    BOOST_CHECK(crates[3].id() == crate_d.id());
    BOOST_CHECK(crates[4].id() == crate_e.id());
}

BOOST_TEST_DECORATOR(* utf::description(
    "database::create_track() for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    create_track__supported_version__creates, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Assert
    BOOST_CHECK_NE(track.id(), 0);
}

BOOST_TEST_DECORATOR(* utf::description(
    "database::remove_track() for all supported schema versions"))
BOOST_DATA_TEST_CASE(
    remove_track__supported_version__removes, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);

    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    db.remove_track(track);

    // Assert
    auto tracks = db.tracks();
    BOOST_CHECK_EQUAL(tracks.size(), 0);
}

BOOST_TEST_DECORATOR(* utf::description(
    "database::verify() for all supported versions"))
BOOST_DATA_TEST_CASE(verify__no_throw, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    // Act/Assert
    BOOST_TEST_CHECKPOINT("(" << version << ") Verifying DB...");
    db.verify();
}

BOOST_TEST_DECORATOR(*utf::description("database::tracks(), all schema versions"))
BOOST_DATA_TEST_CASE(tracks__expected_ids, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);

    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Fetching tracks...");
    auto results = db.tracks();

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 1);
    BOOST_CHECK_EQUAL(results[0].id(), track.id());
}

BOOST_TEST_DECORATOR(*utf::description("database::tracks_by_relative_path(), valid path, all schema versions"))
BOOST_DATA_TEST_CASE(tracks_by_relative_path__valid_path__expected_ids, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);

    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Fetching tracks...");
    auto results = db.tracks_by_relative_path(*snapshot.relative_path);

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 1);
    BOOST_CHECK_EQUAL(results[0].id(), track.id());
}

BOOST_TEST_DECORATOR(*utf::description("database::tracks_by_relative_path(), invalid path, all schema versions"))
BOOST_DATA_TEST_CASE(tracks_by_relative_path__invalid_path__no_ids, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Fetching tracks...");
    auto results = db.tracks_by_relative_path("Does Not Exist.mp3");

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 0);
}

BOOST_TEST_DECORATOR(*utf::description("database::track_by_id(), valid id, all schema versions"))
BOOST_DATA_TEST_CASE(track_by_id__valid_id__expected_ids, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::minimal_1,
        example_track_data_usage::create, version);

    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Fetching tracks...");
    auto result = db.track_by_id(track.id());

    // Assert
    BOOST_CHECK(result);
    BOOST_CHECK_EQUAL(result->id(), track.id());
}

BOOST_TEST_DECORATOR(*utf::description("database::track_by_id(), invalid id, all schema versions"))
BOOST_DATA_TEST_CASE(track_by_id__invalid_id__no_ids, e::all_versions, version)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating temporary database...");
    auto db = e::create_temporary_database(version);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Fetching tracks...");
    auto result = db.track_by_id(123);

    // Assert
    BOOST_CHECK(!result);
}
