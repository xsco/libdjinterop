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

#define BOOST_TEST_MODULE engine_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <vector>

#include <djinterop/engine/engine.hpp>

#include "../temporary_directory.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;

BOOST_TEST_DECORATOR(
    *utf::description("create_database() with all supported schema versions"))
BOOST_DATA_TEST_CASE(
    create_database__valid_version__creates_verified, e::all_versions, version)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;

    {
        // Arrange/Act
        auto db = e::create_database(tmp_loc.temp_dir, version);

        // Assert
        BOOST_CHECK_NO_THROW(db.verify());
        BOOST_CHECK_EQUAL(db.directory(), tmp_loc.temp_dir);
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() with a non-existent path"))
BOOST_AUTO_TEST_CASE(load_database__fake_path__throw)
{
    // Note separate scope to ensure no locks are held on the temporary dir.
    temporary_directory tmp_loc;

    {
        // Arrange/Act/Assert
        e::engine_version unused{};
        BOOST_CHECK_THROW(
            auto db =
                e::load_database(tmp_loc.temp_dir + "/does_not_exist", unused),
            djinterop::database_not_found);
    }
}

struct waveform_extents_test_case
{
    unsigned long long sample_count;
    double sample_rate;
    djinterop::waveform_extents expected;

    friend std::ostream& operator<<(
        std::ostream& os, const waveform_extents_test_case& obj)
    {
        os << "waveform_extents_test_case{sample_count=" << obj.sample_count
           << ", sample_rate=" << obj.sample_rate
           << ", expected=" << obj.expected << "}";
        return os;
    }
};

std::vector<waveform_extents_test_case> overview_waveform_extents_test_cases{
    {0, 44100, {0, 0}},
    {0, 48000, {0, 0}},
    {1, 44100, {1024, 0}},
    {1, 48000, {1024, 0}},
    {456 * 1024, 48000, {1024, 456}},
    {912 * 1024, 48000, {1024, 912}},
    {456 * 8, 48000, {1024, 3.5625}},
    {420 * 1024, 44100, {1024, 420}},
    {840 * 1024, 44100, {1024, 840}},
    {420 * 8, 44100, {1024, 3.28125}},
};

BOOST_TEST_DECORATOR(*utf::description("calculate_overview_waveform_extents()"))
BOOST_DATA_TEST_CASE(
    calculate_overview_waveform_extents__valid__expected,
    overview_waveform_extents_test_cases, test_case)
{
    // Arrange/Act
    auto actual = e::calculate_overview_waveform_extents(
        test_case.sample_count, test_case.sample_rate);

    // Assert
    BOOST_CHECK_EQUAL(test_case.expected, actual);
}

std::vector<waveform_extents_test_case>
    high_resolution_waveform_extents_test_cases{
        {0, 44100, {0, 0}},      {0, 48000, {0, 0}},
        {1, 48000, {1, 456}},    {455, 48000, {1, 456}},
        {456, 48000, {1, 456}},  {912, 48000, {2, 456}},
        {1824, 48000, {4, 456}}, {1825, 48000, {5, 456}},
        {1, 44100, {1, 420}},    {419, 44100, {1, 420}},
        {420, 44100, {1, 420}},  {840, 44100, {2, 420}},
        {1680, 44100, {4, 420}}, {1681, 44100, {5, 420}},
    };

BOOST_TEST_DECORATOR(
    *utf::description("calculate_high_resolution_waveform_extents()"))
BOOST_DATA_TEST_CASE(
    calculate_high_resolution_waveform_extents__valid__expected,
    high_resolution_waveform_extents_test_cases, test_case)
{
    // Arrange/Act
    auto actual = e::calculate_high_resolution_waveform_extents(
        test_case.sample_count, test_case.sample_rate);

    // Assert
    BOOST_CHECK_EQUAL(test_case.expected, actual);
}
