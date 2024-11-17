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
#include <djinterop/track.hpp>

#define BOOST_TEST_MODULE track_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <djinterop/engine/engine.hpp>
#include <djinterop/track_snapshot.hpp>

#include "example_track_data.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace c = std::chrono;
namespace e = djinterop::engine;
namespace utf = boost::unit_test;

namespace
{
const std::vector<example_track_data_variation> creatable_snapshot_types{
    example_track_data_variation::minimal_1,
    example_track_data_variation::basic_metadata_only_1,
    example_track_data_variation::fully_analysed_1,
};

struct snapshot_type_pair
{
    example_track_data_variation initial;
    example_track_data_variation updated;
};

std::ostream& operator<<(std::ostream& o, const snapshot_type_pair v)
{
    o << "snapshot_type_pair(initial=" << v.initial << ", updated=" << v.updated
      << ")";
    return o;
}

const std::vector<snapshot_type_pair> updatable_snapshot_type_pairs{
    snapshot_type_pair{
        example_track_data_variation::minimal_1,
        example_track_data_variation::basic_metadata_only_1},
    snapshot_type_pair{
        example_track_data_variation::minimal_1,
        example_track_data_variation::fully_analysed_1},
    snapshot_type_pair{
        example_track_data_variation::basic_metadata_only_1,
        example_track_data_variation::minimal_1},
    snapshot_type_pair{
        example_track_data_variation::basic_metadata_only_1,
        example_track_data_variation::fully_analysed_1},
    snapshot_type_pair{
        example_track_data_variation::fully_analysed_1,
        example_track_data_variation::minimal_1},
    snapshot_type_pair{
        example_track_data_variation::fully_analysed_1,
        example_track_data_variation::basic_metadata_only_1},
};

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("copy constructor, all schema versions"))
BOOST_DATA_TEST_CASE(
    ctor__supported_version_copy__copies, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Copying track...");
    djinterop::track copy{track};

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

BOOST_TEST_DECORATOR(*utf::description("copy assignment, all schema versions"))
BOOST_DATA_TEST_CASE(
    op_copy_assign__supported_version_copy__copies, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Copying track...");
    djinterop::track copy = track;

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("track snapshot matches created details, all schema "
                      "versions, all snapshots"))
BOOST_DATA_TEST_CASE(
    snapshot__supported_version__same,
    e::supported_schemas * creatable_snapshot_types, schema, snapshot_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type
            << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, snapshot_type, example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type
            << ") Fetching track snapshot...");
    auto actual = track.snapshot();

    // Assert
    djinterop::track_snapshot expected{};
    populate_track_snapshot(
        expected, snapshot_type, example_track_data_usage::fetch, schema);
    BOOST_CHECK_EQUAL(expected, actual);
}

BOOST_TEST_DECORATOR(
    *utf::description("track update with a new snapshot updates correctly, "
                      "all schema versions, all snapshot combinations"))
BOOST_DATA_TEST_CASE(
    update__supported_version__updates,
    e::supported_schemas * updatable_snapshot_type_pairs, schema,
    snapshot_type_pair)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type_pair
            << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type_pair.initial << ", "
            << snapshot_type_pair.updated << ") Creating track...");
    djinterop::track_snapshot initial{};
    populate_track_snapshot(
        initial, snapshot_type_pair.initial, example_track_data_usage::create,
        schema);
    auto track = db.create_track(initial);

    djinterop::track_snapshot modified{};
    populate_track_snapshot(
        modified, snapshot_type_pair.updated, example_track_data_usage::update,
        schema);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << snapshot_type_pair.initial << ", "
            << snapshot_type_pair.updated << ") Updating track...");
    track.update(modified);

    // Assert
    djinterop::track_snapshot expected{};
    populate_track_snapshot(
        expected, snapshot_type_pair.updated, example_track_data_usage::fetch,
        schema);
    auto actual = track.snapshot();
    BOOST_CHECK_EQUAL(expected, actual);
}

// TODO (mr-smidge): Add tests for each getter/setter
//  Note that some fields should only be tested on certain schema versions!

BOOST_TEST_DECORATOR(
    *utf::description("set zero average loudness, all schema versions"))
BOOST_DATA_TEST_CASE(
    set_average_loudness__zero__no_loudness, e::supported_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto db = e::create_temporary_database(schema);

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Setting zero average loudness...");
    track.set_average_loudness(0);

    // Assert
    BOOST_CHECK(track.average_loudness() == std::nullopt);
}
