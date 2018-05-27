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
#include <djinterop/enginelibrary/performance_data.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE performance_data_test

#include <iostream>
#include <cstdio>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace el = djinterop::enginelibrary;
namespace fs = boost::filesystem;
namespace c = std::chrono;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el3"};

namespace boost {
namespace test_tools {

    template<>
    struct print_log_value<std::chrono::milliseconds>
    {
        void operator ()(std::ostream &os, const c::milliseconds &o)
        {
            os << o.count();
        }
    };

    template<>
    struct print_log_value<el::musical_key>
    {
        void operator ()(std::ostream &os, el::musical_key o)
        {
            os << static_cast<int>(o);
        }
    };

    template<>
    struct print_log_value<el::pad_colour>
    {
        void operator ()(std::ostream &os, el::pad_colour o)
        {
            os << boost::format("(%02X,%02X,%02X,%02X)") % o.r % o.g % o.b % o.a;
        }
    };
} // test_tools
} // boost

static fs::path create_temp_dir()
{
    fs::path temp_dir{fs::temp_directory_path()};
    temp_dir /= fs::unique_path();
    if (!fs::create_directory(temp_dir))
    {
        throw std::runtime_error{"Failed to create tmp_dir"};
    }
    std::cout << "Created temp dir at " << temp_dir.string() << std::endl;
    return temp_dir;
}

static void remove_temp_dir(const fs::path &temp_dir)
{
    fs::remove_all(temp_dir);
    std::cout << "Removed temp dir at " << temp_dir.string() << std::endl;
}

static void populate_track_1(el::performance_data &p)
{
    // Track data fields
    p.set_sample_rate(44100);
    p.set_total_samples(17452800);
    p.set_key(el::musical_key::a_minor);
    p.set_average_loudness(0.520831584930419921875);

    // Beat data fields
    p.set_default_beat_grid(el::track_beat_grid{
            -4,
            -83316.78,
            812,
            17470734.439});
    p.set_adjusted_beat_grid(el::track_beat_grid{
            -4,
            -84904.768,
            812,
            17469046.451});

    // Quick cue fields
    std::vector<el::track_hot_cue_point> cues;
    cues.emplace_back(true, "Cue 1", 1377924.5, el::standard_pad_colours::pad_1);
    cues.emplace_back();
    cues.emplace_back(true, "Cue 3", 5508265.964, el::standard_pad_colours::pad_3);
    cues.emplace_back();
    cues.emplace_back(true, "Cue 5", 8261826.939, el::standard_pad_colours::pad_5);
    cues.emplace_back(true, "Cue 6", 9638607.427, el::standard_pad_colours::pad_6);
    p.set_hot_cues(std::begin(cues), std::end(cues));
    p.set_adjusted_main_cue_sample_offset(1377924.5);
    p.set_default_main_cue_sample_offset(1144.012);

    // Loop fields
    std::vector<el::track_loop> loops;
    loops.emplace_back(
            true, true, "Loop 1",
            1144.012, 345339.134, el::standard_pad_colours::pad_1);
    loops.emplace_back(
            true, true,
            "Loop 2", 2582607.427, 2754704.988, el::standard_pad_colours::pad_2);
    loops.emplace_back();
    loops.emplace_back(
            true, true,
            "Loop 4", 4131485.476, 4303583.037, el::standard_pad_colours::pad_4);
    p.set_loops(std::begin(loops), std::end(loops));
}

static void check_track_1(const el::performance_data &p)
{
    // Track data fields
    BOOST_CHECK_CLOSE(p.sample_rate(), 44100.0, 0.001);
    BOOST_CHECK_EQUAL(p.total_samples(), 17452800);
    BOOST_CHECK_EQUAL(p.key(), el::musical_key::a_minor);
    BOOST_CHECK_CLOSE(p.average_loudness(), 0.520831584930419921875, 0.001);
    BOOST_CHECK_EQUAL(p.duration(), c::milliseconds{395755});

    // Beat data fields
    auto default_beat_grid = p.default_beat_grid();
    BOOST_CHECK_EQUAL(default_beat_grid.first_beat_index, -4);
    BOOST_CHECK_CLOSE(default_beat_grid.first_beat_sample_offset, -83316.78, 0.001);
    BOOST_CHECK_EQUAL(default_beat_grid.last_beat_index, 812);
    BOOST_CHECK_CLOSE(default_beat_grid.last_beat_sample_offset, 17470734.439, 0.001);
    auto adjusted_beat_grid = p.adjusted_beat_grid();
    BOOST_CHECK_EQUAL(adjusted_beat_grid.first_beat_index, -4);
    BOOST_CHECK_CLOSE(adjusted_beat_grid.first_beat_sample_offset, -84904.768, 0.001);
    BOOST_CHECK_EQUAL(adjusted_beat_grid.last_beat_index, 812);
    BOOST_CHECK_CLOSE(adjusted_beat_grid.last_beat_sample_offset, 17469046.451, 0.001);
    BOOST_CHECK_CLOSE(p.bpm(), 123, 0.001);

    // Quick cue fields
    auto hot_cue = p.hot_cues_begin();
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 1");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 1377924.5, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, el::standard_pad_colours::pad_1);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 3");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 5508265.964, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, el::standard_pad_colours::pad_3);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 5");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 8261826.939, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, el::standard_pad_colours::pad_5);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 6");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 9638607.427, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, el::standard_pad_colours::pad_6);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue == p.hot_cues_end());
    BOOST_CHECK_CLOSE(p.adjusted_main_cue_sample_offset(), 1377924.5, 0.001);
    BOOST_CHECK_CLOSE(p.default_main_cue_sample_offset(), 1144.012, 0.001);

    // Loop fields
    auto loop = p.loops_begin();
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 1");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 1144.012, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 345339.134, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, el::standard_pad_colours::pad_1);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 2");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 2582607.427, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 2754704.988, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, el::standard_pad_colours::pad_2);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 4");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 4131485.476, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 4303583.037, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, el::standard_pad_colours::pad_4);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop == p.loops_end());
}

static void populate_track_2(el::performance_data &p)
{
    // Track data fields
    p.set_sample_rate(48000);
    p.set_total_samples(10795393);
    p.set_key(el::musical_key::b_minor);
    p.set_average_loudness(0.5);

    // Beat data fields
    p.set_default_beat_grid(el::track_beat_grid{
            -4,
            -107595.55,
            402,
            10820254.92});
    p.set_adjusted_beat_grid(el::track_beat_grid{
            -4,
            -107595.55,
            402,
            10820254.92});

    // Quick cue fields
    std::vector<el::track_hot_cue_point> cues;
    cues.emplace_back();
    cues.emplace_back(true, "Cue 2", 1234567.89, el::standard_pad_colours::pad_2);
    p.set_hot_cues(std::begin(cues), std::end(cues));
    p.set_adjusted_main_cue_sample_offset(1234500.01);
    p.set_default_main_cue_sample_offset(12345.678);

    // Loop fields
    std::vector<el::track_loop> loops;
    loops.emplace_back();
    loops.emplace_back(
            true, true,
            "Loop 2", 2345600, 2345700, el::standard_pad_colours::pad_2);
    p.set_loops(std::begin(loops), std::end(loops));
}

static void check_track_2(const el::performance_data &p)
{
    // Track data fields
    BOOST_CHECK_CLOSE(p.sample_rate(), 48000.0, 0.001);
    BOOST_CHECK_EQUAL(p.total_samples(), 10795393);
    BOOST_CHECK_EQUAL(p.key(), el::musical_key::b_minor);
    BOOST_CHECK_CLOSE(p.average_loudness(), 0.5, 0.001);
    BOOST_CHECK_EQUAL(p.duration(), c::milliseconds{224904});

    // Beat data fields
    auto default_beat_grid = p.default_beat_grid();
    BOOST_CHECK_EQUAL(default_beat_grid.first_beat_index, -4);
    BOOST_CHECK_CLOSE(default_beat_grid.first_beat_sample_offset, -107595.55, 0.001);
    BOOST_CHECK_EQUAL(default_beat_grid.last_beat_index, 402);
    BOOST_CHECK_CLOSE(default_beat_grid.last_beat_sample_offset, 10820254.92, 0.001);
    auto adjusted_beat_grid = p.adjusted_beat_grid();
    BOOST_CHECK_EQUAL(adjusted_beat_grid.first_beat_index, -4);
    BOOST_CHECK_CLOSE(adjusted_beat_grid.first_beat_sample_offset, -107595.55, 0.001);
    BOOST_CHECK_EQUAL(adjusted_beat_grid.last_beat_index, 402);
    BOOST_CHECK_CLOSE(adjusted_beat_grid.last_beat_sample_offset, 10820254.92, 0.001);
    BOOST_CHECK_CLOSE(p.bpm(), 107, 0.001);

    // Quick cue fields
    auto hot_cue = p.hot_cues_begin();
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 2");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 1234567.89, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, el::standard_pad_colours::pad_2);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_REQUIRE(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue == p.hot_cues_end());
    BOOST_CHECK_CLOSE(p.adjusted_main_cue_sample_offset(), 1234500.01, 0.001);
    BOOST_CHECK_CLOSE(p.default_main_cue_sample_offset(), 12345.678, 0.001);

    // Loop fields
    auto loop = p.loops_begin();
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 2");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 2345600, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 2345700, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, el::standard_pad_colours::pad_2);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_REQUIRE(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop == p.loops_end());
}

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_fields)
{
    // Arrange
    el::database db{sample_path};

    // Act
    el::performance_data p{db, 1};

    // Assert
    BOOST_CHECK_EQUAL(p.track_id(), 1);
    check_track_1(p);
}

BOOST_AUTO_TEST_CASE (setters__good_values__values_stored)
{
    // Arrange
    el::performance_data p{123};

    // Act
    populate_track_1(p);

    // Assert
    BOOST_CHECK_EQUAL(p.track_id(), 123);
    check_track_1(p);
}

BOOST_AUTO_TEST_CASE (save__new_track_no_values__doesnt_throw)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = create_database(temp_dir.string(), el::version_1_7_1);
    el::performance_data p{123};

    // Act/Assert
    BOOST_CHECK_NO_THROW(p.save(db));
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (ctor__new_track_no_values__doesnt_throw)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = create_database(temp_dir.string(), el::version_1_7_1);
    el::performance_data p{123};
    p.save(db);

    // Act/Assert
    // Note use of () for ctor invocation rather than {}, as Boost's macros
    // are confused by {} in the statement below.
    BOOST_CHECK_NO_THROW(el::performance_data p_reloaded(db, 123));
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__new_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = create_database(temp_dir.string(), el::version_1_7_1);
    el::performance_data p{123};
    populate_track_1(p);

    // Act
    p.save(db);

    // Assert
    check_track_1(p);
    el::performance_data p_reloaded{db, 123};
    BOOST_CHECK_EQUAL(p_reloaded.track_id(), 123);
    check_track_1(p_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__existing_track__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = create_database(temp_dir.string(), el::version_1_7_1);
    el::performance_data p{1};
    populate_track_1(p);
    p.save(db);
    populate_track_2(p);

    // Act
    p.save(db);

    // Assert
    check_track_2(p);
    el::performance_data p_reloaded{db, 1};
    BOOST_CHECK_EQUAL(p_reloaded.track_id(), 1);
    check_track_2(p_reloaded);
    remove_temp_dir(temp_dir);
}

