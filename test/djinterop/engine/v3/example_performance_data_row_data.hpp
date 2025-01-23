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
#include <cstdint>
#include <ostream>
#include <string>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v3/performance_data_table.hpp>

#include "../../boost_test_utils.hpp"

enum class example_performance_data_row_type
{
    minimal_1,
    fully_analysed_1,
};

inline std::ostream& operator<<(std::ostream& o, example_performance_data_row_type v)
{
    switch (v)
    {
        case example_performance_data_row_type::minimal_1: o << "minimal_1"; break;
        case example_performance_data_row_type::fully_analysed_1:
            o << "fully_analysed_1";
            break;
        default: o << "unknown"; break;
    }

    return o;
}

inline void populate_performance_data_row(
    example_performance_data_row_type row_type, djinterop::engine::v3::performance_data_row& r,
    const djinterop::engine::engine_schema& schema)
{
    using namespace std::string_literals;
    namespace ev = djinterop::engine;
    namespace ev3 = djinterop::engine::v3;
    switch (row_type)
    {
        case example_performance_data_row_type::minimal_1:
        {
            r.active_on_load_loops = 123;
            break;
        }

        case example_performance_data_row_type::fully_analysed_1:
        {
            r.track_data = ev3::track_data_blob{41000, 5424300, 1, 0.5, 0.5, 0.5};
            r.overview_waveform_data = ev3::overview_waveform_data_blob{};
            auto beatgrid = std::vector<ev3::beat_grid_marker_blob>{};
            beatgrid.push_back(ev3::beat_grid_marker_blob{0, 1, 16, 0});
            beatgrid.push_back(ev3::beat_grid_marker_blob{313600, 16, 0, 0});
            r.beat_data =
                ev3::beat_data_blob{41000, 5424300, 1, beatgrid, beatgrid};
            std::vector<ev3::quick_cue_blob> quick_cues_list{};
            quick_cues_list.push_back(ev3::quick_cue_blob{
                "First cue", 12345, ev::standard_pad_colors::pad_1});
            r.quick_cues =
                ev3::quick_cues_blob{quick_cues_list, 11111, true, 22222};
            r.loops = ev3::loops_blob{};
            r.active_on_load_loops = 123;

            break;
        }

        default: assertm("Unrecognised performance data row type", false);
    }
}