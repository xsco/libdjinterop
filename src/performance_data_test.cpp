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
    BOOST_CHECK_EQUAL(p.default_beat_grid().first_beat_index, -4);
    BOOST_CHECK_CLOSE(p.default_beat_grid().first_beat_sample_offset, -83316.78, 0.001);
    BOOST_CHECK_EQUAL(p.default_beat_grid().last_beat_index, 812);
    BOOST_CHECK_CLOSE(p.default_beat_grid().last_beat_sample_offset, 17470734.439, 0.001);
    BOOST_CHECK_EQUAL(p.adjusted_beat_grid().first_beat_index, -4);
    BOOST_CHECK_CLOSE(p.adjusted_beat_grid().first_beat_sample_offset, -84904.768, 0.001);
    BOOST_CHECK_EQUAL(p.adjusted_beat_grid().last_beat_index, 812);
    BOOST_CHECK_CLOSE(p.adjusted_beat_grid().last_beat_sample_offset, 17469046.451, 0.001);
    BOOST_CHECK_CLOSE(p.bpm(), 123, 0.001);
}

BOOST_AUTO_TEST_CASE (ctor__track_1__correct_quick_cue_fields)
{
    // Arrange
    ep::database db{sample_path};

    // Act
    ep::performance_data p{db, 1};

    // Assert
    BOOST_CHECK_EQUAL(p.hot_cues().size(), 8);
    BOOST_CHECK_EQUAL(p.hot_cues()[0].is_set, true);
    BOOST_CHECK_EQUAL(p.hot_cues()[0].label, "Cue 1");
    BOOST_CHECK_CLOSE(p.hot_cues()[0].sample_offset, 1377924.5, 0.001);
    BOOST_CHECK_EQUAL(p.hot_cues()[0].colour, ep::standard_pad_colours::pad_1);
    BOOST_CHECK_EQUAL(p.hot_cues()[1].is_set, false);
    BOOST_CHECK_EQUAL(p.hot_cues()[2].is_set, true);
    BOOST_CHECK_EQUAL(p.hot_cues()[2].label, "Cue 3");
    BOOST_CHECK_CLOSE(p.hot_cues()[2].sample_offset, 5508265.964, 0.001);
    BOOST_CHECK_EQUAL(p.hot_cues()[2].colour, ep::standard_pad_colours::pad_3);
    BOOST_CHECK_EQUAL(p.hot_cues()[3].is_set, false);
    BOOST_CHECK_EQUAL(p.hot_cues()[4].is_set, true);
    BOOST_CHECK_EQUAL(p.hot_cues()[4].label, "Cue 5");
    BOOST_CHECK_CLOSE(p.hot_cues()[4].sample_offset, 8261826.939, 0.001);
    BOOST_CHECK_EQUAL(p.hot_cues()[4].colour, ep::standard_pad_colours::pad_5);
    BOOST_CHECK_EQUAL(p.hot_cues()[5].is_set, true);
    BOOST_CHECK_EQUAL(p.hot_cues()[5].label, "Cue 6");
    BOOST_CHECK_CLOSE(p.hot_cues()[5].sample_offset, 9638607.427, 0.001);
    BOOST_CHECK_EQUAL(p.hot_cues()[5].colour, ep::standard_pad_colours::pad_6);
    BOOST_CHECK_EQUAL(p.hot_cues()[6].is_set, false);
    BOOST_CHECK_EQUAL(p.hot_cues()[7].is_set, false);
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
    BOOST_CHECK_EQUAL(p.loops().size(), 8);
    BOOST_CHECK_EQUAL(p.loops()[0].is_start_set, true);
    BOOST_CHECK_EQUAL(p.loops()[0].is_end_set, true);
    BOOST_CHECK_EQUAL(p.loops()[0].label, "Loop 1");
    BOOST_CHECK_CLOSE(p.loops()[0].start_sample_offset, 1144.012, 0.001);
    BOOST_CHECK_CLOSE(p.loops()[0].end_sample_offset, 345339.134, 0.001);
    BOOST_CHECK_EQUAL(p.loops()[0].colour, ep::standard_pad_colours::pad_1);
    BOOST_CHECK_EQUAL(p.loops()[1].is_start_set, true);
    BOOST_CHECK_EQUAL(p.loops()[1].is_end_set, true);
    BOOST_CHECK_EQUAL(p.loops()[1].label, "Loop 2");
    BOOST_CHECK_CLOSE(p.loops()[1].start_sample_offset, 2582607.427, 0.001);
    BOOST_CHECK_CLOSE(p.loops()[1].end_sample_offset, 2754704.988, 0.001);
    BOOST_CHECK_EQUAL(p.loops()[1].colour, ep::standard_pad_colours::pad_2);
    BOOST_CHECK_EQUAL(p.loops()[2].is_start_set, false);
    BOOST_CHECK_EQUAL(p.loops()[2].is_end_set, false);
    BOOST_CHECK_EQUAL(p.loops()[3].is_start_set, true);
    BOOST_CHECK_EQUAL(p.loops()[3].is_end_set, true);
    BOOST_CHECK_EQUAL(p.loops()[3].label, "Loop 4");
    BOOST_CHECK_CLOSE(p.loops()[3].start_sample_offset, 4131485.476, 0.001);
    BOOST_CHECK_CLOSE(p.loops()[3].end_sample_offset, 4303583.037, 0.001);
    BOOST_CHECK_EQUAL(p.loops()[3].colour, ep::standard_pad_colours::pad_4);
    BOOST_CHECK_EQUAL(p.loops()[4].is_start_set, false);
    BOOST_CHECK_EQUAL(p.loops()[4].is_end_set, false);
    BOOST_CHECK_EQUAL(p.loops()[5].is_start_set, false);
    BOOST_CHECK_EQUAL(p.loops()[5].is_end_set, false);
    BOOST_CHECK_EQUAL(p.loops()[6].is_start_set, false);
    BOOST_CHECK_EQUAL(p.loops()[6].is_end_set, false);
    BOOST_CHECK_EQUAL(p.loops()[7].is_start_set, false);
    BOOST_CHECK_EQUAL(p.loops()[7].is_end_set, false);
}

