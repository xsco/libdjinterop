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
#include <djinterop/track.hpp>

#define BOOST_TEST_MODULE track_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <djinterop/enginelibrary.hpp>
#include <djinterop/track_snapshot.hpp>

#include "boost_test_utils.hpp"
#include "example_track_data.hpp"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace c = std::chrono;
namespace el = djinterop::enginelibrary;
namespace utf = boost::unit_test;

namespace
{
const std::vector<example_track_type> creatable_snapshot_types{
    example_track_type::minimal_1,
    example_track_type::basic_metadata_only_1,
    example_track_type::fully_analysed_1,
};

struct snapshot_type_pair
{
    example_track_type initial;
    example_track_type updated;
};

std::ostream& operator<<(std::ostream& o, const snapshot_type_pair v)
{
    o << "snapshot_type_pair(initial=" << v.initial << ", updated=" << v.updated
      << ")";
    return o;
}

const std::vector<snapshot_type_pair> updatable_snapshot_type_pairs{
    snapshot_type_pair{
        example_track_type::minimal_1,
        example_track_type::basic_metadata_only_1},
    snapshot_type_pair{
        example_track_type::minimal_1, example_track_type::fully_analysed_1},
    snapshot_type_pair{
        example_track_type::basic_metadata_only_1,
        example_track_type::minimal_1},
    snapshot_type_pair{
        example_track_type::basic_metadata_only_1,
        example_track_type::fully_analysed_1},
    snapshot_type_pair{
        example_track_type::fully_analysed_1, example_track_type::minimal_1},
    snapshot_type_pair{
        example_track_type::fully_analysed_1,
        example_track_type::basic_metadata_only_1},
};

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("copy constructor, all schema versions"))
BOOST_DATA_TEST_CASE(
    ctor__supported_version_copy__copies, el::all_versions, version)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        example_track_type::fully_analysed_1, version, snapshot);
    auto track = db.create_track(snapshot);

    // Act
    djinterop::track copy{track};

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

BOOST_TEST_DECORATOR(*utf::description("copy assignment, all schema versions"))
BOOST_DATA_TEST_CASE(
    op_copy_assign__supported_version_copy__copies, el::all_versions, version)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        example_track_type::fully_analysed_1, version, snapshot);
    auto track = db.create_track(snapshot);

    // Act
    djinterop::track copy = track;

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

BOOST_TEST_DECORATOR(
    *utf::description("track snapshot matches created details, all schema "
                      "versions, all snapshots"))
BOOST_DATA_TEST_CASE(
    snapshot__supported_version__same,
    el::all_versions* creatable_snapshot_types, version, snapshot_type)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot expected{};
    populate_track_snapshot(snapshot_type, version, expected);
    auto track = db.create_track(expected);

    // Act
    auto actual = track.snapshot();

    // Assert
    assert_track_snapshot_equal(expected, actual, false);
}

BOOST_TEST_DECORATOR(
    *utf::description("track update with a new snapshot updates correctly, "
                      "all schema versions, all snapshot combinations"))
BOOST_DATA_TEST_CASE(
    update__supported_version__updates,
    el::all_versions* updatable_snapshot_type_pairs, version,
    snapshot_type_pair)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot initial{};
    populate_track_snapshot(snapshot_type_pair.initial, version, initial);
    auto track = db.create_track(initial);
    djinterop::track_snapshot expected{};
    populate_track_snapshot(snapshot_type_pair.updated, version, expected);

    // Act
    track.update(expected);

    // Assert
    auto actual = track.snapshot();
    assert_track_snapshot_equal(expected, actual, false);
}

// TODO (mr-smidge): Add tests for each getter/setter

BOOST_TEST_DECORATOR(
    *utf::description("set zero average loudness, all schema versions"))
BOOST_DATA_TEST_CASE(
    set_average_loudness__zero__no_loudness, el::all_versions, version)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        example_track_type::fully_analysed_1, version, snapshot);
    auto track = db.create_track(snapshot);

    // Act
    track.set_average_loudness(0);

    // Assert
    BOOST_CHECK(track.average_loudness() == djinterop::stdx::nullopt);
}

BOOST_TEST_DECORATOR(
    *utf::description("set zero sampling rate, all schema versions"))
BOOST_DATA_TEST_CASE(
    set_sampling__zero_rate__no_sampling, el::all_versions, version)
{
    // Arrange
    auto db = el::create_temporary_database(version);
    djinterop::track_snapshot snapshot{};
    populate_track_snapshot(
        example_track_type::fully_analysed_1, version, snapshot);
    auto track = db.create_track(snapshot);

    // Act
    track.set_sampling(djinterop::sampling_info{});

    // Assert
    BOOST_CHECK(track.sampling() == djinterop::stdx::nullopt);
}
