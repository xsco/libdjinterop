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

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iterator>
#include <string>
#include <vector>
#include <boost/optional.hpp>

#define _MODERN_SQLITE_BOOST_OPTIONAL_SUPPORT
#include "sqlite_modern_cpp.h"

#include "performance_data_format.hpp"

namespace djinterop {
namespace enginelibrary {

struct performance_data_row
{
    performance_data_row(int track_id) : track_id{track_id}
    {}

    int track_id;
    double is_analyzed = 1.0;
    double is_rendered = 0.0;
    track_data_blob track_data;
    // TODO - high_resolution_wave_form_data
    // TODO - overview_wave_form_data
    beat_data_blob beat_data;
    quick_cues_blob quick_cues;
    loops_blob loops;
    double has_serato_values = 0.0;
    double has_rekordbox_values = 0.0;
};

// Select out a row from the PerformanceData table
static performance_data_row extract_performance_data(
        const database &db, int track_id)
{
	sqlite::database m_db{db.performance_db_path()};
	performance_data_row row{track_id};
	int rows_found = 0;
    if (db.version() >= version_firmware_1_0_3)
    {
    	m_db
    		<< "SELECT id, isAnalyzed, isRendered, "
               "trackData, "
               "highResolutionWaveFormData, overviewWaveFormData, "
               "beatData, quickCues, loops, "
               "hasSeratoValues, hasRekordboxValues "
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
                    double has_serato_values,
                    double has_rekordbox_values)
    		{
    			row.is_analyzed = is_analyzed;
                row.is_rendered = is_rendered;
                row.track_data = decode_track_data(id, track_data);
                row.beat_data = decode_beat_data(id, beat_data);
                row.quick_cues = decode_quick_cues(id, quick_cues);
                row.loops = decode_loops(id, loops);
                row.has_serato_values = has_serato_values;
                row.has_rekordbox_values = has_rekordbox_values;
    			++rows_found;
    		};
    }
    else
    {
        // No `hasRekordboxValues` column in this schema version
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
                row.track_data = decode_track_data(id, track_data);
                row.beat_data = decode_beat_data(id, beat_data);
                row.quick_cues = decode_quick_cues(id, quick_cues);
                row.loops = decode_loops(id, loops);
                row.has_serato_values = has_serato_values;
                row.has_rekordbox_values = 0.0;
    			++rows_found;
    		};
    }

	if (rows_found == 0)
		throw nonexistent_performance_data{track_id};

	return row;
}

static track_beat_grid beat_markers_to_beat_grid(
        const std::vector<beat_data_marker_blob> beat_markers)
{
    if (beat_markers.size() == 0)
        return track_beat_grid{};
    if (beat_markers.size() < 2)
        throw std::invalid_argument{"Not enough markers in beat data"};

    auto &first = beat_markers.front();
    auto &last = beat_markers.back();
    return track_beat_grid{
        static_cast<int>(first.beat_index),
        first.sample_offset,
        static_cast<int>(last.beat_index),
        last.sample_offset};
}

static std::vector<beat_data_marker_blob> beat_grid_to_beat_markers(
        const track_beat_grid &beat_grid)
{
    // The beat markers have an int32_t field, the meaning of which is currently
    // unknown.  Some default hex values, that have been observed in the wild,
    // are used below as a temporary measure until the real meaning of the field
    // can be identified.
    std::vector<beat_data_marker_blob> markers;
    markers.push_back(beat_data_marker_blob{
            beat_grid.first_beat_sample_offset,
            beat_grid.first_beat_index,
            beat_grid.last_beat_index - beat_grid.first_beat_index,
            0x7fc9});
    markers.push_back(beat_data_marker_blob{
            beat_grid.last_beat_sample_offset,
            beat_grid.last_beat_index,
            0,
            0x7fff});
    return markers;
}

struct performance_data::impl
{
    impl(const database &db, int track_id) :
        load_db_uuid_{db.uuid()},
        pd_{extract_performance_data(db, track_id)},
        default_beat_grid_{
            beat_markers_to_beat_grid(pd_.beat_data.default_markers)},
        adjusted_beat_grid_{
            beat_markers_to_beat_grid(pd_.beat_data.adjusted_markers)}
    {}

    impl(int track_id) :
        load_db_uuid_{},
        pd_{track_id}
    {
        pd_.track_data.sample_rate = 0;
        pd_.track_data.total_samples = 0;
        pd_.track_data.key = 0;
        pd_.track_data.average_loudness = 0.5;
    }
    
    std::string load_db_uuid_;
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
    pimpl_->pd_.beat_data.sample_rate = sample_rate;
}

void performance_data::set_total_samples(int_least64_t total_samples)
{
    pimpl_->pd_.track_data.total_samples = total_samples;
    pimpl_->pd_.beat_data.total_samples = total_samples;
}

void performance_data::set_key(musical_key key)
{
    pimpl_->pd_.track_data.key = static_cast<int>(key);
}

void performance_data::set_average_loudness(double average_loudness)
{
    pimpl_->pd_.track_data.average_loudness = average_loudness;
}

void performance_data::set_default_beat_grid(const track_beat_grid &beat_grid)
{
    pimpl_->default_beat_grid_ = beat_grid;
    pimpl_->pd_.beat_data.default_markers =
        beat_grid_to_beat_markers(pimpl_->default_beat_grid_);
}

void performance_data::set_default_beat_grid(track_beat_grid &&beat_grid)
{
    pimpl_->default_beat_grid_ = std::move(beat_grid);
    pimpl_->pd_.beat_data.default_markers =
        beat_grid_to_beat_markers(pimpl_->default_beat_grid_);
}

void performance_data::set_adjusted_beat_grid(const track_beat_grid &beat_grid)
{
    pimpl_->adjusted_beat_grid_ = beat_grid;
    pimpl_->pd_.beat_data.adjusted_markers =
        beat_grid_to_beat_markers(pimpl_->adjusted_beat_grid_);
}

void performance_data::set_adjusted_beat_grid(track_beat_grid &&beat_grid)
{
    pimpl_->adjusted_beat_grid_ = std::move(beat_grid);
    pimpl_->pd_.beat_data.adjusted_markers =
        beat_grid_to_beat_markers(pimpl_->adjusted_beat_grid_);
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

    auto cur_size = pimpl_->pd_.quick_cues.hot_cues.size(); 
    if (cur_size < 8)
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

    auto cur_size = pimpl_->pd_.loops.loops.size();
    if (cur_size < 8)
        pimpl_->pd_.loops.loops.resize(8);
}

void performance_data::save(const database &database)
{
	sqlite::database m_db{database.performance_db_path()};
    if (database.version() >= version_firmware_1_0_3)
    {
        m_db <<
            "INSERT OR REPLACE INTO PerformanceData ("
            "  id, isAnalyzed, isRendered, trackData, "
            "  highResolutionWaveFormData, overviewWaveFormData, "
            "  beatData, quickCues, loops, "
            "  hasSeratoValues, hasRekordboxValues)"
            "VALUES ("
            "  ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
            << pimpl_->pd_.track_id
            << pimpl_->pd_.is_analyzed
            << pimpl_->pd_.is_rendered
            << encode_track_data(pimpl_->pd_.track_data)
            << std::vector<char>{} // waveforms not yet implemented
            << std::vector<char>{} // waveforms not yet implemented
            << encode_beat_data(pimpl_->pd_.beat_data)
            << encode_quick_cues(pimpl_->pd_.quick_cues)
            << encode_loops(pimpl_->pd_.loops)
            << pimpl_->pd_.has_serato_values
            << pimpl_->pd_.has_rekordbox_values;
    }
    else
    {
        m_db <<
            "INSERT OR REPLACE INTO PerformanceData ("
            "  id, isAnalyzed, isRendered, trackData, "
            "  highResolutionWaveFormData, overviewWaveFormData, "
            "  beatData, quickCues, loops, "
            "  hasSeratoValues)"
            "VALUES ("
            "  ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
            << pimpl_->pd_.track_id
            << pimpl_->pd_.is_analyzed
            << pimpl_->pd_.is_rendered
            << encode_track_data(pimpl_->pd_.track_data)
            << std::vector<char>{} // waveforms not yet implemented
            << std::vector<char>{} // waveforms not yet implemented
            << encode_beat_data(pimpl_->pd_.beat_data)
            << encode_quick_cues(pimpl_->pd_.quick_cues)
            << encode_loops(pimpl_->pd_.loops)
            << pimpl_->pd_.has_serato_values;
    }

    pimpl_->load_db_uuid_ = database.uuid();
}

void normalise_beat_grid(track_beat_grid &beat_grid, double last_sample)
{
    double samples_per_beat =
        (
            beat_grid.last_beat_sample_offset -
            beat_grid.first_beat_sample_offset) /
        (
            beat_grid.last_beat_index -
            beat_grid.first_beat_index);

    // Adjust first beat sample offset to be aligned to beat -4
    double first_adjust_offset =
        (beat_grid.first_beat_index + 4) * samples_per_beat;
    beat_grid.first_beat_index = -4;
    beat_grid.first_beat_sample_offset -= first_adjust_offset;

    // Work out what beat number is just beyond the last sample
    int last_beats_adjust =
        1 + 
        ((last_sample - beat_grid.last_beat_sample_offset) / samples_per_beat);
    // Adjust last beat sample offset
    double last_adjust_offset = last_beats_adjust * samples_per_beat;
    beat_grid.last_beat_index += last_beats_adjust;
    beat_grid.last_beat_sample_offset += last_adjust_offset;
}

} // enginelibrary
} // djinterop

