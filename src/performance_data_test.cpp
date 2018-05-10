/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <engineprime/performance_data.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE performance_data_test

#include <iostream>
#include <cstdio>
#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
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
    struct print_log_value<ep::musical_key>
    {
        void operator ()(std::ostream &os, ep::musical_key o)
        {
            os << static_cast<int>(o);
        }
    };

    template<>
    struct print_log_value<ep::pad_colour>
    {
        void operator ()(std::ostream &os, ep::pad_colour o)
        {
            os << boost::format("(%02X,%02X,%02X,%02X)") % o.r % o.g % o.b % o.a;
        }
    };
} // test_tools
} // boost

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_track_data_fields)
{
    // Arrange
    ep::database db{sample_path};

    // Act
    ep::performance_data p{db, 1};

    // Assert
    BOOST_CHECK_EQUAL(p.track_id(), 1);
    BOOST_CHECK_CLOSE(p.sample_rate(), 44100.0, 0.001);
    BOOST_CHECK_EQUAL(p.total_samples(), 17452800);
    BOOST_CHECK_EQUAL(p.key(), ep::musical_key::a_minor);
    BOOST_CHECK_CLOSE(p.average_loudness(), 0.520831584930419921875, 0.001);
    BOOST_CHECK_EQUAL(p.duration(), c::milliseconds{395755});
}

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_beat_data_fields)
{
    // Arrange
    ep::database db{sample_path};

    // Act
    ep::performance_data p{db, 1};

    // Assert
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
}

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_quick_cue_fields)
{
    // Arrange
    ep::database db{sample_path};

    // Act
    ep::performance_data p{db, 1};

    // Assert
    auto hot_cue = p.hot_cues_begin();
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 1");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 1377924.5, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, ep::standard_pad_colours::pad_1);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 3");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 5508265.964, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, ep::standard_pad_colours::pad_3);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 5");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 8261826.939, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, ep::standard_pad_colours::pad_5);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, true);
    BOOST_CHECK_EQUAL(hot_cue->label, "Cue 6");
    BOOST_CHECK_CLOSE(hot_cue->sample_offset, 9638607.427, 0.001);
    BOOST_CHECK_EQUAL(hot_cue->colour, ep::standard_pad_colours::pad_6);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue != p.hot_cues_end());
    BOOST_CHECK_EQUAL(hot_cue->is_set, false);
    ++hot_cue;
    BOOST_CHECK(hot_cue == p.hot_cues_end());
    BOOST_CHECK_CLOSE(p.adjusted_main_cue_sample_offset(), 1377924.5, 0.001);
    BOOST_CHECK_CLOSE(p.default_main_cue_sample_offset(), 1144.012, 0.001);
}

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_loop_fields)
{
    // Arrange
    ep::database db{sample_path};

    // Act
    ep::performance_data p{db, 1};

    // Assert
    auto loop = p.loops_begin();
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 1");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 1144.012, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 345339.134, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, ep::standard_pad_colours::pad_1);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 2");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 2582607.427, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 2754704.988, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, ep::standard_pad_colours::pad_2);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, true);
    BOOST_CHECK_EQUAL(loop->is_end_set, true);
    BOOST_CHECK_EQUAL(loop->label, "Loop 4");
    BOOST_CHECK_CLOSE(loop->start_sample_offset, 4131485.476, 0.001);
    BOOST_CHECK_CLOSE(loop->end_sample_offset, 4303583.037, 0.001);
    BOOST_CHECK_EQUAL(loop->colour, ep::standard_pad_colours::pad_4);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop != p.loops_end());
    BOOST_CHECK_EQUAL(loop->is_start_set, false);
    BOOST_CHECK_EQUAL(loop->is_end_set, false);
    ++loop;
    BOOST_CHECK(loop == p.loops_end());
}

