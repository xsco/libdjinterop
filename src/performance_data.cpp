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
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include "sqlite_modern_cpp.h"

namespace io = boost::iostreams;

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

// Uncompress a zlib'ed BLOB
static void uncompress(
        int track_id,
        const std::vector<char> &compressed,
        std::vector<char> &uncompressed)
{
    if (compressed.size() < 4)
        throw corrupt_performance_data(track_id);

    auto apparent_size = *reinterpret_cast<const uint32_t *>(compressed.data());
    if (apparent_size == 0)
        throw corrupt_performance_data(track_id);

    uncompressed.clear();
    uncompressed.reserve(apparent_size);

    io::filtering_ostream out;
    out.push(io::zlib_decompressor{});
    out.push(io::back_inserter(uncompressed));
    io::write(out, &compressed[4], compressed.size() - 4);
}

// Extract an int32_t from a big-endian encoded raw value at ptr address
static int32_t extract_int32_be(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 4; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*ptr++);
    }
    return val;
}

// Extract an int64_t from a big-endian encoded raw value at ptr address
static int64_t extract_int64_be(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 8; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*ptr++);
    }
    return val;
}

// Extract a double from a big-endian encoded raw value at ptr address
static double extract_double_be(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 8; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*ptr++);
    }
    return *reinterpret_cast<double *>(&val);
}

// Extract track data from a blob
static void extract_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data,
        track_data_blob &track_data)
{
    std::vector<char> raw_data;
    uncompress(track_id, compressed_track_data, raw_data);
    auto ptr = raw_data.data();

    // track_data should always be 28 bytes long
    if (raw_data.size() != 28)
        throw corrupt_performance_data(track_id);

    // Extract values from raw data
    track_data.sample_rate      = extract_double_be(ptr);
    track_data.total_samples    = extract_int64_be(ptr + 8);
    track_data.average_loudness = extract_double_be(ptr + 16);
    track_data.key              = extract_int32_be(ptr + 24);
}

// Select out a row from the PerformanceData table
static performance_data_row extract_performance_data(
        const std::string &performance_db_path, int id)
{
	sqlite::database m_db{performance_db_path};
	performance_data_row row;
	int rows_found = 0;
	m_db
		<< "SELECT id, isAnalyzed, isRendered, "
           "trackData, "
           "highResolutionWaveFormData, overviewWaveFormData, beatData, "
           "quickCues, loops, "
           "hasSeratoValues "
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

            extract_track_data(id, track_data, row.track_data);

            row.has_serato_values = has_serato_values;
			++rows_found;
		};

	if (rows_found == 0)
		throw nonexistent_performance_data{id};

	return row;
}

struct performance_data::impl
{
    impl(const database &db, int track_id) :
		track_id_{track_id},
        pd_{extract_performance_data(db.performance_db_path(), track_id)}
    {}
    
    int track_id_;
    performance_data_row pd_;
};


performance_data::performance_data(
        const database &database, int track_id) :
    pimpl_{new impl{database, track_id}}
{}

performance_data::~performance_data() = default;

int performance_data::track_id() const { return pimpl_->track_id_; }
// TODO - duration
double performance_data::sample_rate() const { return pimpl_->pd_.track_data.sample_rate; }
int_least64_t performance_data::total_samples() const { return pimpl_->pd_.track_data.total_samples; }
double performance_data::average_loudness() const { return pimpl_->pd_.track_data.average_loudness; }

} // namespace engineprime
