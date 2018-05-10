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

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iterator>
#include <string>
#include <vector>
#include "sqlite_modern_cpp.h"
#include "performance_data_format.hpp"

namespace engineprime {

struct performance_data_row
{
    performance_data_row(int track_id) : track_id{track_id}
    {}

    int track_id;
    double is_analyzed = true;
    double is_rendered = true;
    track_data_blob track_data;
    // TODO - high_resolution_wave_form_data
    // TODO - overview_wave_form_data
    beat_data_blob beat_data;
    quick_cues_blob quick_cues;
    loops_blob loops;
    double has_serato_values = 0.0;
};

// Select out a row from the PerformanceData table
static performance_data_row extract_performance_data(
        const std::string &performance_db_path, int track_id)
{
	sqlite::database m_db{performance_db_path};
	performance_data_row row{track_id};
	int rows_found = 0;
	m_db
		<< "SELECT id, isAnalyzed, isRendered, "
           "trackData, "
           "highResolutionWaveFormData, overviewWaveFormData, "
           "beatData, quickCues, loops, "
           "hasSeratoValues "
		   "FROM PerformanceData WHERE id = :1"
		<< track_id
		>> [&](
                int id,
                double is_analyzed,
                double is_rendered,
                std::vector<char> track_data,
                std::vector<char> high_resolution_wave_form_data,
                std::vector<char> overview_wave_form_data,
                std::vector<char> beat_data,
                std::vector<char> quick_cues,
                std::vector<char> loops,
                double has_serato_values)
		{
			row.is_analyzed = is_analyzed;
            row.is_rendered = is_rendered;
            extract_track_data(id, track_data, row.track_data);
            extract_beat_data(id, beat_data, row.beat_data);
            extract_quick_cues(id, quick_cues, row.quick_cues);
            extract_loops(id, loops, row.loops);

            row.has_serato_values = has_serato_values;
			++rows_found;
		};

	if (rows_found == 0)
		throw nonexistent_performance_data{track_id};

	return row;
}

struct performance_data::impl
{
    impl(const database &db, int track_id) :
        pd_{extract_performance_data(db.performance_db_path(), track_id)}
    {
        // Fill in tailored default beat grid info from blob
        default_beat_grid_.first_beat_index =
            pd_.beat_data.default_markers[0].beat_index;
        default_beat_grid_.first_beat_sample_offset =
            pd_.beat_data.default_markers[0].sample_offset;
        auto ldmi = pd_.beat_data.default_num_beatgrid_markers - 1;
        default_beat_grid_.last_beat_index =
            pd_.beat_data.default_markers[ldmi].beat_index;
        default_beat_grid_.last_beat_sample_offset =
            pd_.beat_data.default_markers[ldmi].sample_offset;

        // Fill in tailored adjusted beat grid info from blob
        adjusted_beat_grid_.first_beat_index =
            pd_.beat_data.adjusted_markers[0].beat_index;
        adjusted_beat_grid_.first_beat_sample_offset =
            pd_.beat_data.adjusted_markers[0].sample_offset;
        auto lami = pd_.beat_data.adjusted_num_beatgrid_markers - 1;
        adjusted_beat_grid_.last_beat_index =
            pd_.beat_data.adjusted_markers[lami].beat_index;
        adjusted_beat_grid_.last_beat_sample_offset =
            pd_.beat_data.adjusted_markers[lami].sample_offset;
    }

    impl(int track_id) : pd_{track_id}
    {}
    
    performance_data_row pd_;
    track_beat_grid default_beat_grid_;
    track_beat_grid adjusted_beat_grid_;
};


performance_data::performance_data(
        const database &database, int track_id) :
    pimpl_{new impl{database, track_id}}
{}

performance_data::performance_data(int track_id) : pimpl_{new impl{track_id}}
{}

performance_data::~performance_data() = default;

int performance_data::track_id() const
{
    return pimpl_->pd_.track_id;
}

double performance_data::sample_rate() const
{
    return pimpl_->pd_.track_data.sample_rate;
}

int_least64_t performance_data::total_samples() const
{
    return pimpl_->pd_.track_data.total_samples;
}

musical_key performance_data::key() const
{
    return static_cast<musical_key>(pimpl_->pd_.track_data.key);
}

double performance_data::average_loudness() const
{
    return pimpl_->pd_.track_data.average_loudness;
}

track_beat_grid performance_data::default_beat_grid() const
{
    return pimpl_->default_beat_grid_;
}

track_beat_grid performance_data::adjusted_beat_grid() const
{
    return pimpl_->adjusted_beat_grid_;
}

hot_cue_const_iterator performance_data::hot_cues_begin() const
{
    return pimpl_->pd_.quick_cues.hot_cues.begin();
}

hot_cue_const_iterator performance_data::hot_cues_end() const
{
    return pimpl_->pd_.quick_cues.hot_cues.end();
}

double performance_data::adjusted_main_cue_sample_offset() const
{
    return pimpl_->pd_.quick_cues.adjusted_main_cue_sample_offset;
}

double performance_data::default_main_cue_sample_offset() const
{
    return pimpl_->pd_.quick_cues.default_main_cue_sample_offset;
}

track_loop_const_iterator performance_data::loops_begin() const
{
    return pimpl_->pd_.loops.loops.begin();
}

track_loop_const_iterator performance_data::loops_end() const
{
    return pimpl_->pd_.loops.loops.end();
}

void performance_data::set_sample_rate(double sample_rate)
{
    pimpl_->pd_.track_data.sample_rate = sample_rate;
}

void performance_data::set_total_samples(int_least64_t total_samples)
{
    pimpl_->pd_.track_data.total_samples = total_samples;
}

void performance_data::set_key(musical_key key)
{
    pimpl_->pd_.track_data.key = static_cast<int>(key);
}

void performance_data::set_average_loudness(double average_loudness)
{
    pimpl_->pd_.track_data.average_loudness = average_loudness;
}

void performance_data::set_default_beat_grid(track_beat_grid beat_grid)
{
    pimpl_->default_beat_grid_ = beat_grid;

    // TODO - update the beat_data blob as well
    // adjust beat grid to span from beat -4 to last usable beat in a bar of 4
}

void performance_data::set_adjusted_beat_grid(track_beat_grid beat_grid)
{
    pimpl_->adjusted_beat_grid_ = beat_grid;

    // TODO - update the beat_data blob as well
}

void performance_data::set_hot_cues(
        hot_cue_const_iterator begin,
        hot_cue_const_iterator end)
{
    pimpl_->pd_.quick_cues.hot_cues.clear();
    int count = 8;
    std::copy_if(
            begin, end,
            std::back_inserter(pimpl_->pd_.quick_cues.hot_cues),
            [&count](hot_cue_const_iterator::reference)
            {
                if (count-- > 0)
                    return true;
                return false;
            });
    if (pimpl_->pd_.quick_cues.hot_cues.size() < 8)
        pimpl_->pd_.quick_cues.hot_cues.resize(8);
}

void performance_data::set_default_main_cue_sample_offset(double sample_offset)
{
    pimpl_->pd_.quick_cues.default_main_cue_sample_offset = sample_offset;
}

void performance_data::set_adjusted_main_cue_sample_offset(double sample_offset)
{
    pimpl_->pd_.quick_cues.adjusted_main_cue_sample_offset = sample_offset;
}

void performance_data::set_loops(
        track_loop_const_iterator begin,
        track_loop_const_iterator end)
{
    pimpl_->pd_.loops.loops.clear();
    int count = 8;
    std::copy_if(
            begin, end,
            std::back_inserter(pimpl_->pd_.loops.loops),
            [&count](track_loop_const_iterator::reference)
            {
                if (count-- > 0)
                    return true;
                return false;
            });
    if (pimpl_->pd_.loops.loops.size() < 8)
        pimpl_->pd_.loops.loops.resize(8);
}

void performance_data::save(const database &database)
{
    // TODO - implement save of performance data to DB
}

} // namespace engineprime
