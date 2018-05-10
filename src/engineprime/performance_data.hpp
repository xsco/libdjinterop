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

/**
 * The `nonexistent_performance_data` exception is thrown when a request is
 * made to look up performance data for a given track id in a given database,
 * but there is no such performance data stored.
 */
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

/**
 * The `corrupt_performance_data` exception is thrown when internal expectations
 * of the performance data is stored within a database are not met.
 */
class corrupt_performance_data : public std::logic_error
{
public:
    explicit corrupt_performance_data(int track_id) noexcept :
        logic_error{"PerformanceData is corrupted or of unknown format"},
        track_id_{track_id}
    {}
    explicit corrupt_performance_data(int track_id, const char *msg) noexcept :
        logic_error{msg},
        track_id_{track_id}
    {}
	virtual ~corrupt_performance_data() = default;
	int track_id() const noexcept { return track_id_; }
private:
	int track_id_;
};

/**
 * The `musical_key` enumeration contains a list all known musical keys that a
 * track may be detected to initially follow.
 */
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

/**
 * The `pad_colour` struct holds information about the colour that a given
 * hot cue / loop / etc. pad on the Denon SC5000 prime deck may be lit up as.
 *
 * Note that the alpha channel is typically not used, and is usually set to
 * full brightness.
 */
struct pad_colour
{
    uint_least8_t r;
    uint_least8_t g;
    uint_least8_t b;
    uint_least8_t a;
};

namespace standard_pad_colours
{
    constexpr pad_colour pad_1{ 0xEA, 0xC5, 0x32, 0xFF };
    constexpr pad_colour pad_2{ 0xEA, 0x8F, 0x32, 0xFF };
    constexpr pad_colour pad_3{ 0xB8, 0x55, 0xBF, 0xFF };
    constexpr pad_colour pad_4{ 0xBA, 0x2A, 0x41, 0xFF };
    constexpr pad_colour pad_5{ 0x86, 0xC6, 0x4B, 0xFF };
    constexpr pad_colour pad_6{ 0x20, 0xC6, 0x7C, 0xFF };
    constexpr pad_colour pad_7{ 0x00, 0xA8, 0xB1, 0xFF };
    constexpr pad_colour pad_8{ 0x15, 0x8E, 0xE2, 0xFF };
}

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
    bool is_start_set;
    bool is_end_set;
    std::string label;
    double start_sample_offset;
    double end_sample_offset;
    pad_colour colour;

    bool is_set() const { return is_start_set && is_end_set; }
};

typedef std::vector<track_hot_cue_point>::const_iterator hot_cue_const_iterator;
typedef std::vector<track_loop>::const_iterator track_loop_const_iterator;

/**
 * The results of track analysis
 */
class performance_data
{
public:
    /**
     * \brief Construct performance data, loading from a database
     */
	performance_data(const database &db, int track_id);

    /**
     * \brief Construct an empty performance data record for a given track,
     *        not yet saved in any database
     */
    performance_data(int track_id);

    /**
     * \brief Destructor
     */
	~performance_data();

    /**
     * \brief Gets the id of the track that this performance data relates to
     */
	int track_id() const;

    /**
     * \brief Gets the sample rate of the track
     */
    double sample_rate() const;

    /**
     * \brief Get the total number of samples in the track
     */
    int_least64_t total_samples() const;

    /**
     * \brief Get the initial musical key of the track
     */
    musical_key key() const;

    /**
     * \brief Get the average loudness of the track
     *
     * The loudness value ranges from zero to one, and is typically close to
     * 0.5 for a well-mastered track.  The exact algorithm for determining
     * loudness is not yet known.
     */
    double average_loudness() const;

    /**
     * \brief Get the default beat grid, i.e. the one detected by automated
     *        analysis
     */
    track_beat_grid default_beat_grid() const;

    /**
     * \brief Get the adjusted beat grid, i.e. the one that may have been
     *        adjusted or tweaked by the user
     *
     * Note that if the beat grid has not been adjusted, then this will be equal
     * to the default beatgrid.
     */
    track_beat_grid adjusted_beat_grid() const;

    /**
     * \brief Gets an iterator pointing to the first hot cue slot
     *
     * Note that there are always 8 hot cues per track in an Engine Prime library
     */
    hot_cue_const_iterator hot_cues_begin() const;

    /**
     * \brief Gets an iterator pointing beyond the last hot cue slot
     *
     * Note that there are always 8 hot cues per track in an Engine Prime library
     */
    hot_cue_const_iterator hot_cues_end() const;

    /**
     * \brief Gets the sample at which the main cue point is set, as determined
     *        by automated analysis of the track
     */
    double default_main_cue_sample_offset() const;

    /**
     * \brief Gets the sample at which the main cue point is set, which may have
     *        been adjusted/tweaked by the user
     *
     * Note that if the user has not adjusted the main cue point, this will be
     * equal to the default main cue point.
     */
    double adjusted_main_cue_sample_offset() const;
    
    /**
     * \brief Gets an iterator pointing to the first loop
     *
     * Note that there are always 8 loops per track in an Engine Prime Library.
     */
    track_loop_const_iterator loops_begin() const;

    /**
     * \brief Gets an iterator pointing beyond the last loop
     *
     * Note that there are always 8 loops per track in an Engine Prime Library.
     */
    track_loop_const_iterator loops_end() const;

    /**
     * \brief Gets the duration of the track, in milliseconds
     *
     * This is calculated from the number of samples in the track, and the
     * sample rate.
     */
    std::chrono::milliseconds duration() const
    {
        auto ms = 1000 * total_samples() / (int_least64_t)sample_rate();
        return std::chrono::milliseconds{ms};
    }

    /**
     * \brief Get the BPM of the track
     *
     * This is calculated from the adjusted beat grid associated with the track
     * (which is measured in samples), and the sample rate of the track.
     */
	double bpm() const
	{
		return sample_rate() * 60 *
			(double)(adjusted_beat_grid().last_beat_index -
			 adjusted_beat_grid().first_beat_index) /
			(adjusted_beat_grid().last_beat_sample_offset -
			 adjusted_beat_grid().first_beat_sample_offset);
	}

    void set_sample_rate(double sample_rate);

    void set_total_samples(int_least64_t total_samples);

    void set_key(musical_key key);

    void set_average_loudness(double average_loudness);

    void set_default_beat_grid(track_beat_grid beat_grid);

    void set_adjusted_beat_grid(track_beat_grid beat_grid);

    void set_hot_cues(
            hot_cue_const_iterator begin,
            hot_cue_const_iterator end);

    void set_default_main_cue_sample_offset(double sample_offset);

    void set_adjusted_main_cue_sample_offset(double sample_offset);
    
    void set_loops(
            track_loop_const_iterator begin,
            track_loop_const_iterator end);

    /**
     * \brief Save track performance data to a given database
     */
    void save(const database &database);

private:
	class impl;
	std::unique_ptr<impl> pimpl_;
};

inline bool operator ==(const pad_colour &x, const pad_colour &y)
{
	return x.r == y.r && x.g == y.g && x.b == y.b && x.a == y.a;
}

} // engineprime

#endif // ENGINEPRIME_PERFORMANCE_DATA_HPP
