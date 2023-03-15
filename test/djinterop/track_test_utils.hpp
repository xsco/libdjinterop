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

#pragma once

#include <boost/test/included/unit_test.hpp>

#include <djinterop/database.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "boost_test_utils.hpp"

template <typename TVersion>
void do_ctor_copy_test(
    const TVersion& version, djinterop::database& db,
    const djinterop::track_snapshot& snapshot)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Copying track...");
    djinterop::track copy{track};

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

template <typename TVersion>
void do_op_copy_assign_test(
    const TVersion& version, djinterop::database& db,
    const djinterop::track_snapshot& snapshot)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << version << ") Creating track...");
    auto track = db.create_track(snapshot);

    // Act
    BOOST_TEST_CHECKPOINT("(" << version << ") Copying track...");
    djinterop::track copy = track;

    // Assert
    BOOST_CHECK_EQUAL(track.id(), copy.id());
}

template <typename TVersion, typename TSnapshot>
void do_snapshot_test(
    const TVersion& version, const TSnapshot& snapshot_type,
    djinterop::database& db, const djinterop::track_snapshot& expected)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << snapshot_type << ") Creating track...");
    auto track = db.create_track(expected);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << snapshot_type
            << ") Fetching track snapshot...");
    auto actual = track.snapshot();

    // Assert
    auto expected_with_id = expected;
    expected_with_id.id = track.id();
    BOOST_CHECK_EQUAL(expected_with_id, actual);
}

template <typename TVersion, typename TSnapshot>
void do_update_test(
    const TVersion& version, const TSnapshot& initial_snapshot_type,
    const TSnapshot& update_snapshot_type, djinterop::database& db,
    const djinterop::track_snapshot& initial,
    const djinterop::track_snapshot& expected)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << initial_snapshot_type << ", "
            << update_snapshot_type << ") Creating track...");
    auto track = db.create_track(initial);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << initial_snapshot_type << ", "
            << update_snapshot_type << ") Updating track...");
    track.update(expected);

    // Assert
    auto expected_with_id = expected;
    expected_with_id.id = track.id();
    auto actual = track.snapshot();
    BOOST_CHECK_EQUAL(expected_with_id, actual);
}
