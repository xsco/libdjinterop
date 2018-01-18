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
#include <string>
#include <vector>
#include <cstdint>
#include "sqlite_modern_cpp.h"

namespace engineprime {


struct track_data_blob
{
    double sample_rate;
    uint64_t total_samples;
    double average_loudness;
    uint32_t key;
};

struct performance_data_row
{
    int id;
    double is_analyzed;
    double is_rendered;
    track_data_blob track_data;
    // TODO - high_resolution_wave_form_data
    // TODO - overview_wave_form_data
    // TODO - beat_data
    // TODO - quick_cues
    // TODO - loops
    double has_serato_values;
};

// Select out a row from the PerformanceData table
static performance_data_row select_performance_data_row(
        const std::string &performance_db_path, int id)
{
	sqlite::database m_db{performance_db_path};
	performance_data_row row;
	int rows_found = 0;
	m_db
		<< "SELECT id, isAnalyzed, isRendered, trackData, "
           "highResolutionWaveFormData, overviewWaveFormData, beatData, "
           "quickCues, loops, hasSeratoValues "
		   "FROM PerformanceData WHERE id = :1"
		<< id
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
			row = performance_data_row{
                id,
                is_analyzed,
                is_rendered};

            // Uncompress BLOBs using boost::iostreams
            // http://www.boost.org/doc/libs/1_58_0/libs/iostreams/doc/index.html

            row.has_serato_values = has_serato_values;
			++rows_found;
		};

    // TODO - uncompress the blobs on the fly
    // TODO - can the vectors be written directly to structs, perhaps via std::raw_storage_iterator?

	if (rows_found == 0)
		throw nonexistent_performance_data{id};

	return row;
}

struct performance_data::impl
{
    impl(const database &db, int track_id) :
		track_id_{track_id}
    {}
    
    int track_id_;
};


performance_data::performance_data(
        const database &database, int track_id) :
    pimpl_{new impl{database, track_id}}
{}

performance_data::~performance_data() = default;

int performance_data::track_id() const { return pimpl_->track_id_; }

} // namespace engineprime
