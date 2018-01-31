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

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_PERFORMANCE_DATA_HPP
#define ENGINEPRIME_PERFORMANCE_DATA_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "database.hpp"

namespace engineprime {

class nonexistent_performance_data : public std::invalid_argument
{
public:
	explicit nonexistent_performance_data(int track_id) noexcept :
		invalid_argument{"PerformanceData does not exist in database"},
		track_id_{track_id}
	{}
	virtual ~nonexistent_performance_data() = default;
	int track_id() const noexcept { return track_id_; }
private:
	int track_id_;
};

class corrupt_performance_data : public std::invalid_argument
{
public:
    explicit corrupt_performance_data(int track_id) noexcept :
        invalid_argument{"PerformanceData is corrupted or of unknown format"},
        track_id_{track_id}
    {}
	virtual ~corrupt_performance_data() = default;
	int track_id() const noexcept { return track_id_; }
private:
	int track_id_;
};

enum class musical_key
{
    a_minor = 1,
    g_major,
    e_minor,
    d_major,
    b_minor,
    a_major,
    f_sharp_minor,
    e_major,
    d_flat_minor,
    b_major,
    a_flat_minor,
    f_sharp_major,
    e_flat_minor,
    d_flat_major,
    b_flat_minor,
    a_flat_major,
    f_minor,
    e_flat_major,
    c_minor,
    b_flat_major,
    g_minor,
    f_major,
    d_minor,
    c_major
};

struct pad_colour
{
    uint_least8_t r;
    uint_least8_t g;
    uint_least8_t b;
    uint_least8_t a;
};

struct standard_pad_colours
{
    static constexpr pad_colour pad_1{ 0xEA, 0xC5, 0x32, 0xFF };
    static constexpr pad_colour pad_2{ 0xEA, 0x8F, 0x32, 0xFF };
    static constexpr pad_colour pad_3{ 0xB8, 0x55, 0xBF, 0xFF };
    static constexpr pad_colour pad_4{ 0xBA, 0x2A, 0x41, 0xFF };
    static constexpr pad_colour pad_5{ 0x86, 0xC6, 0x4B, 0xFF };
    static constexpr pad_colour pad_6{ 0x20, 0xC6, 0x7C, 0xFF };
    static constexpr pad_colour pad_7{ 0x00, 0xA8, 0xB1, 0xFF };
    static constexpr pad_colour pad_8{ 0x15, 0x8E, 0xE2, 0xFF };
};

struct track_beat_grid
{
	int first_beat_index;
    double first_beat_sample_offset;
	int last_beat_index;
    double last_beat_sample_offset;
};

struct track_hot_cue_point
{
    bool is_set;
    std::string label;
    double sample_offset;
    pad_colour colour;
};

struct track_loop
{
    bool is_set;
    std::string label;
    double start_sample_offset;
    double end_sample_offset;
    pad_colour colour;
};

// The results of track analysis
class performance_data
{
public:
	performance_data(const database &db, int track_id);
	~performance_data();

	int track_id() const;
    //std::chrono::seconds duration() const;
    double sample_rate() const;
    int_least64_t total_samples() const;
    //const musical_key &key() const;
    double average_loudness() const;
    //const track_beat_grid &default_beat_grid() const;
    //const track_beat_grid &adjusted_beat_grid() const;
    //const std::vector<track_hot_cue_point> &hot_cues() const;
    //double adjusted_main_cue_sample_offset() const;
    //double default_main_cue_sample_offset() const;
    //const std::vector<track_loop> &loops() const;

    /*
	double bpm() const
	{
		return sample_rate * 60 *
			(double)(adjusted_beat_grid.last_beat_index -
			 adjusted_beat_grid.first_beat_index) /
			(adjusted_beat_grid.last_beat_sample_offset -
			 adjusted_beat_grid.first_beat_sample_offset);
	}
    */

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

} // engineprime

#endif // ENGINEPRIME_PERFORMANCE_DATA_HPP
