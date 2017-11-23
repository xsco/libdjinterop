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

#ifndef ENGINEPRIME_TRACK_HPP
#define ENGINEPRIME_TRACK_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "database.hpp"

namespace engineprime {

// Artwork for tracks
// TODO - move to dedicated file
struct album_art
{
    typedef uint_least8_t image_data_type;

    std::string hash_hex_str;
    std::vector<image_data_type> image_data;
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
    const pad_colour pad_1{ 0xEA, 0xC5, 0x32, 0xFF };
    const pad_colour pad_2{ 0xEA, 0x8F, 0x32, 0xFF };
    const pad_colour pad_3{ 0xB8, 0x55, 0xBF, 0xFF };
    const pad_colour pad_4{ 0xBA, 0x2A, 0x41, 0xFF };
    const pad_colour pad_5{ 0x86, 0xC6, 0x4B, 0xFF };
    const pad_colour pad_6{ 0x20, 0xC6, 0x7C, 0xFF };
    const pad_colour pad_7{ 0x00, 0xA8, 0xB1, 0xFF };
    const pad_colour pad_8{ 0x15, 0x8E, 0xE2, 0xFF };
};

// Track metadata (i.e. as stored in the file)
struct track_metadata
{
    int track_number;
    std::chrono::seconds duration;
    int bpm;
    int year;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string comment;
    std::string publisher;
    std::string composer;
};

struct track_beat_grid
{
    double first_beat_sample_offset;
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
struct track_analysis
{
    std::chrono::seconds duration;
    double sample_rate;
    int_least64_t total_samples;
    /*TODO const*/ int bpm;
    musical_key key;
    double average_loudness;
    track_beat_grid default_beat_grid;
    track_beat_grid adjusted_beat_grid;
    std::vector<track_hot_cue_point> hot_cues;
    double adjusted_main_cue_sample_offset;
    double default_main_cue_sample_offset;
    std::vector<track_loop> loops;
};

class track
{
public:
    track(const database &database, int id);
    ~track();
    
    int id() const;
	const std::string &path() const;
	const std::string &filename() const;
	const std::string &file_extension() const;
    
    // TODO - move to dedicated file
    /*
    std::shared_ptr<album_art> art() const;
    bool has_art() const;
    */

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

std::vector<int> all_track_ids(const database &database);

} // engineprime

#endif // ENGINEPRIME_TRACK_HPP
