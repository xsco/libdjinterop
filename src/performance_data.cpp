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
    int64_t total_samples;
    double average_loudness;
    int32_t key;
};

struct beat_data_marker_blob
{
    double sample_offset;
    int64_t beat_index;
    int32_t beats_until_next_marker;
    int32_t unknown_field_1;
};

struct beat_data_blob
{
    double sample_rate;
    int64_t total_samples;
    int8_t is_beat_data_set;
    int64_t default_num_beatgrid_markers;
    std::vector<beat_data_marker_blob> default_markers;
    int64_t adjusted_num_beatgrid_markers;
    std::vector<beat_data_marker_blob> adjusted_markers;
};

struct performance_data_row
{
    int id;
    double is_analyzed;
    double is_rendered;
    track_data_blob track_data;
    // TODO - high_resolution_wave_form_data
    // TODO - overview_wave_form_data
    beat_data_blob beat_data;
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

// Extract an int8_t from a raw value at ptr address
static int32_t extract_int8(const char *ptr)
{
    return static_cast<const unsigned char>(*ptr);
}

// Extract an int32_t from a little-endian encoded raw value at ptr address
static int32_t extract_int32_le(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 4; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*(ptr + 3 - i));
    }
    return val;
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

// Extract an int64_t from a little-endian encoded raw value at ptr address
static int64_t extract_int64_le(const char *ptr)
{
    int64_t val = 0;
    for (int i = 0; i < 8; ++i)
    {
        val <<= 8;
        val |= static_cast<const unsigned char>(*(ptr + 7 - i));
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

// Extract a double from a little-endian encoded raw value at ptr address
static double extract_double_le(const char *ptr)
{
    auto val = extract_int64_le(ptr);
    return *reinterpret_cast<double *>(&val);
}

// Extract a double from a big-endian encoded raw value at ptr address
static double extract_double_be(const char *ptr)
{
    auto val = extract_int64_be(ptr);
    return *reinterpret_cast<double *>(&val);
}

// Extract track data from a blob
static void extract_track_data(
        int track_id,
        const std::vector<char> &compressed_track_data,
        track_data_blob &track_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_track_data, raw_data);
    auto ptr = raw_data.data();

    // track_data should always be 28 bytes long
    if (raw_data.size() != 28)
        throw corrupt_performance_data{track_id};

    // Extract values from raw data
    track_data.sample_rate      = extract_double_be(ptr     );
    track_data.total_samples    = extract_int64_be (ptr + 8 );
    track_data.average_loudness = extract_double_be(ptr + 16);
    track_data.key              = extract_int32_be (ptr + 24);
}

// Extract beat data from a blob
static void extract_beat_data(
        int track_id,
        const std::vector<char> &compressed_beat_data,
        beat_data_blob &beat_data)
{
    // Uncompress
    std::vector<char> raw_data;
    uncompress(track_id, compressed_beat_data, raw_data);
    auto ptr = raw_data.data();

    // Minimum length of 129 bytes
    if (raw_data.size() < 129)
        throw corrupt_performance_data{track_id};

    // Extract fixed-location values from raw data
    beat_data.sample_rate      = extract_double_be(ptr     );
    beat_data.total_samples    = extract_int64_be (ptr + 8 );
    beat_data.is_beat_data_set = extract_int8     (ptr + 16);

    // Default beat grid
    beat_data.default_num_beatgrid_markers = extract_int64_be(ptr + 17);
    if (beat_data.default_num_beatgrid_markers < 2)
        throw corrupt_performance_data{track_id};
    beat_data.default_markers.reserve(beat_data.default_num_beatgrid_markers);
    auto marker_ptr = ptr + 25;
    for (int i = 0; i < beat_data.default_num_beatgrid_markers; ++i)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        auto &marker = beat_data.default_markers[i];
        marker.sample_offset           = extract_double_le(marker_ptr     );
        marker.beat_index              = extract_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = extract_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = extract_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }

    // Adjusted beat grid
    marker_ptr = ptr + 25 + 8 + (beat_data.default_num_beatgrid_markers * 24);
    beat_data.adjusted_num_beatgrid_markers = extract_int64_be(marker_ptr - 8);
    if (beat_data.adjusted_num_beatgrid_markers < 2)
        throw corrupt_performance_data{track_id};
    beat_data.adjusted_markers.reserve(beat_data.adjusted_num_beatgrid_markers);
    for (int i = 0; i < beat_data.adjusted_num_beatgrid_markers; ++i)
    {
        if (marker_ptr - ptr > raw_data.size())
            throw corrupt_performance_data{track_id};
        auto &marker = beat_data.adjusted_markers[i];
        marker.sample_offset           = extract_double_le(marker_ptr     );
        marker.beat_index              = extract_int64_le (marker_ptr + 8 );
        marker.beats_until_next_marker = extract_int32_le (marker_ptr + 16);
        marker.unknown_field_1         = extract_int32_le (marker_ptr + 20);
        marker_ptr += 24;
    }
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
           "highResolutionWaveFormData, overviewWaveFormData, "
           "beatData, quickCues, loops, "
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
            extract_beat_data(id, beat_data, row.beat_data);

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
    
    int track_id_;
    performance_data_row pd_;
    track_beat_grid default_beat_grid_;
    track_beat_grid adjusted_beat_grid_;
};


performance_data::performance_data(
        const database &database, int track_id) :
    pimpl_{new impl{database, track_id}}
{}

performance_data::~performance_data() = default;

int performance_data::track_id() const { return pimpl_->track_id_; }
double performance_data::sample_rate() const { return pimpl_->pd_.track_data.sample_rate; }
int_least64_t performance_data::total_samples() const { return pimpl_->pd_.track_data.total_samples; }
musical_key performance_data::key() const { return static_cast<musical_key>(pimpl_->pd_.track_data.key); }
double performance_data::average_loudness() const { return pimpl_->pd_.track_data.average_loudness; }

} // namespace engineprime
