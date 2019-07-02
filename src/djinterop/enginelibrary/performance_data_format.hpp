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

#if __cplusplus < 201402L && _MSVC_LANG < 201402L
#error This library needs at least a C++14 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP
#define DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP

#include <cstdint>
#include <vector>

#include <boost/optional.hpp>

#include <djinterop/performance_data.hpp>

namespace djinterop
{
namespace enginelibrary
{
struct beat_data
{
    beat_data() noexcept = default;

    boost::optional<sampling_info> sampling;
    std::vector<beatgrid_marker> default_beatgrid;
    std::vector<beatgrid_marker> adjusted_beatgrid;

    std::vector<char> encode() const;
    static beat_data decode(const std::vector<char>& compressed_data);
};

struct high_res_waveform_data
{
    high_res_waveform_data() noexcept = default;

    double samples_per_entry;
    std::vector<waveform_entry> waveform;

    std::vector<char> encode() const;
    static high_res_waveform_data decode(
        const std::vector<char>& compressed_data);
};

struct loops_data
{
    loops_data() = default;

    std::array<boost::optional<loop>, 8> loops;  // Don't use curly braces here!

    std::vector<char> encode() const;
    static loops_data decode(
        const std::vector<char>& raw_data);  // not compressed
};

struct overview_waveform_data
{
    overview_waveform_data() noexcept = default;

    double samples_per_entry;
    std::vector<waveform_entry> waveform;

    std::vector<char> encode() const;
    static overview_waveform_data decode(
        const std::vector<char>& compressed_data);
};

struct quick_cues_data
{
    quick_cues_data() = default;

    std::array<boost::optional<hot_cue>, 8> hot_cues;
    double adjusted_main_cue = 0;
    double default_main_cue = 0;

    std::vector<char> encode() const;

    static quick_cues_data decode(const std::vector<char>& compressed_data);
};

struct track_data
{
    track_data() noexcept = default;

    boost::optional<sampling_info> sampling;
    boost::optional<double> average_loudness;  // range (0, 1]
    boost::optional<musical_key> key;

    std::vector<char> encode() const;

    static track_data decode(const std::vector<char>& compressed_data);
};

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_PERFORMANCE_DATA_FORMAT_HPP
