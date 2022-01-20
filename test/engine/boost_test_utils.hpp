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

#include <cassert>

#include <djinterop/track_snapshot.hpp>

#include "boost_test_printable.hpp"

/// Assertion macro allowing a custom message to be printed in case of failure.
#define assertm(exp, msg) assert(((void)msg, exp))

inline void assert_track_snapshot_equal(
    const djinterop::track_snapshot& expected,
    const djinterop::track_snapshot& actual, bool check_id = true)
{
    if (check_id)
    {
        BOOST_CHECK_EQUAL(pr(expected.id), pr(actual.id));
    }

    BOOST_TEST(
        expected.adjusted_beatgrid == actual.adjusted_beatgrid,
        boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(
        pr(expected.adjusted_main_cue), pr(actual.adjusted_main_cue));
    BOOST_CHECK_EQUAL(pr(expected.album), pr(actual.album));
    BOOST_CHECK_EQUAL(pr(expected.artist), pr(actual.artist));
    BOOST_CHECK_EQUAL(
        pr(expected.average_loudness), pr(actual.average_loudness));
    BOOST_CHECK_EQUAL(pr(expected.bitrate), pr(actual.bitrate));
    BOOST_CHECK_EQUAL(pr(expected.bpm), pr(actual.bpm));
    BOOST_CHECK_EQUAL(pr(expected.comment), pr(actual.comment));
    BOOST_CHECK_EQUAL(pr(expected.composer), pr(actual.composer));
    BOOST_TEST(
        expected.default_beatgrid == actual.default_beatgrid,
        boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(
        pr(expected.default_main_cue), pr(actual.default_main_cue));
    BOOST_CHECK_EQUAL(pr(expected.duration), pr(actual.duration));
    BOOST_CHECK_EQUAL(pr(expected.file_bytes), pr(actual.file_bytes));
    BOOST_CHECK_EQUAL(pr(expected.genre), pr(actual.genre));
    BOOST_TEST(
        expected.hot_cues == actual.hot_cues, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(pr(expected.key), pr(actual.key));
    BOOST_CHECK_EQUAL(
        pr(expected.last_accessed_at), pr(actual.last_accessed_at));
    BOOST_CHECK_EQUAL(
        pr(expected.last_modified_at), pr(actual.last_modified_at));
    BOOST_CHECK_EQUAL(pr(expected.last_played_at), pr(actual.last_played_at));
    BOOST_TEST(
        expected.loops == actual.loops, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(pr(expected.publisher), pr(actual.publisher));
    BOOST_CHECK_EQUAL(pr(expected.rating), pr(actual.rating));
    BOOST_CHECK_EQUAL(pr(expected.relative_path), pr(actual.relative_path));
    BOOST_CHECK_EQUAL(pr(expected.sampling), pr(actual.sampling));
    BOOST_CHECK_EQUAL(pr(expected.title), pr(actual.title));
    BOOST_CHECK_EQUAL(pr(expected.track_number), pr(actual.track_number));
    BOOST_TEST(
        expected.waveform == actual.waveform, boost::test_tools::per_element());
    BOOST_CHECK_EQUAL(pr(expected.year), pr(actual.year));
}
