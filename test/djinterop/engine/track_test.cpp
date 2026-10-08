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
#include <djinterop/engine/v2/engine_library.hpp>
#include <djinterop/engine/v3/engine_library.hpp>
#include <djinterop/track_snapshot.hpp>

#include "example_track_data.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace c = std::chrono;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;
namespace ev3 = djinterop::engine::v3;
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

BOOST_TEST_DECORATOR(
    *utf::description("track update keeps columns not in the snapshot, all "
                      "v2 schema versions"))
BOOST_DATA_TEST_CASE(
    update__v2_unsnapshotted_columns__keeps, e::supported_v2_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto library = ev2::engine_library::create_temporary(schema);
    auto db = library.database();

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating track...");
    auto track = db.create_track(snapshot);

    const auto album_art_id = library.album_art().add(ev2::album_art_row{
        ev2::ALBUM_ART_ROW_ID_NONE,
        ev2::album_art_text_hash{"0123456789abcdef0123456789abcdef01234567"},
        {}});
    const c::system_clock::time_point date_added{c::seconds{1700000000}};
    library.track().set_album_art_id(track.id(), album_art_id);
    library.track().set_date_added(track.id(), date_added);
    library.track().set_is_played(track.id(), true);

    auto modified = track.snapshot();
    modified.title = "Updated Title";

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Updating track...");
    track.update(modified);

    // Assert
    auto row = library.track().get(track.id());
    BOOST_REQUIRE(row);
    BOOST_CHECK(row->title == std::optional<std::string>{"Updated Title"});
    BOOST_CHECK_EQUAL(row->album_art_id, album_art_id);
    BOOST_CHECK(row->date_added == date_added);
    BOOST_CHECK(row->is_played);
}

BOOST_TEST_DECORATOR(
    *utf::description("track update keeps columns not in the snapshot, all "
                      "v3 schema versions"))
BOOST_DATA_TEST_CASE(
    update__v3_unsnapshotted_columns__keeps, e::supported_v3_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto library = ev3::engine_library::create_temporary(schema);
    auto db = library.database();

    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        snapshot, example_track_data_variation::fully_analysed_1,
        example_track_data_usage::create, schema);

    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating track...");
    auto track = db.create_track(snapshot);

    const auto album_art_id = library.album_art().add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE,
        ev3::album_art_text_hash{"0123456789abcdef0123456789abcdef01234567"},
        {}});
    const c::system_clock::time_point date_added{c::seconds{1700000000}};
    library.track().set_album_art_id(track.id(), album_art_id);
    library.track().set_date_added(track.id(), date_added);
    library.track().set_is_played(track.id(), true);

    auto modified = track.snapshot();
    modified.title = "Updated Title";

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Updating track...");
    track.update(modified);

    // Assert
    auto row = library.track().get(track.id());
    BOOST_REQUIRE(row);
    BOOST_CHECK(row->title == std::optional<std::string>{"Updated Title"});
    BOOST_CHECK_EQUAL(row->album_art_id, album_art_id);
    BOOST_CHECK(row->date_added == date_added);
    BOOST_CHECK(row->is_played);
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
