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

#include <cstddef>
#include <cstdint>
#include <vector>

#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop
{
enum class musical_key;

namespace engine::v1
{
struct beat_data
{
    stdx::optional<double> sample_rate;
    stdx::optional<double> sample_count;
    std::vector<beatgrid_marker> default_beatgrid;
    std::vector<beatgrid_marker> adjusted_beatgrid;

    beat_data() noexcept = default;

    friend bool operator==(
        const beat_data& first, const beat_data& second) noexcept
    {
        return first.sample_rate == second.sample_rate &&
               first.sample_count == second.sample_count &&
               first.default_beatgrid == second.default_beatgrid &&
               first.adjusted_beatgrid == second.adjusted_beatgrid;
    }

    std::vector<std::byte> encode() const;
    static beat_data decode(const std::vector<std::byte>& compressed_data);
};

struct high_res_waveform_data
{
    double samples_per_entry;
    std::vector<waveform_entry> waveform;

    high_res_waveform_data() noexcept = default;

    friend bool operator==(
        const high_res_waveform_data& first,
        const high_res_waveform_data& second) noexcept
    {
        return first.samples_per_entry == second.samples_per_entry &&
               first.waveform == second.waveform;
    }

    std::vector<std::byte> encode() const;
    static high_res_waveform_data decode(
        const std::vector<std::byte>& compressed_data);
};

struct loops_data
{
    std::vector<stdx::optional<loop> > loops;

    loops_data() noexcept = default;

    friend bool operator==(
        const loops_data& first, const loops_data& second) noexcept
    {
        return first.loops == second.loops;
    }

    std::vector<std::byte> encode() const;
    static loops_data decode(
        const std::vector<std::byte>& raw_data);  // not compressed
};

struct overview_waveform_data
{
    double samples_per_entry;
    std::vector<waveform_entry> waveform;

    overview_waveform_data() noexcept = default;

    friend bool operator==(
        const overview_waveform_data& first,
        const overview_waveform_data& second) noexcept
    {
        return first.samples_per_entry == second.samples_per_entry &&
               first.waveform == second.waveform;
    }

    std::vector<std::byte> encode() const;
    static overview_waveform_data decode(
        const std::vector<std::byte>& compressed_data);
};

struct quick_cues_data
{
    std::vector<stdx::optional<hot_cue> > hot_cues;
    double adjusted_main_cue = 0;
    double default_main_cue = 0;

    quick_cues_data() noexcept = default;

    friend bool operator==(
        const quick_cues_data& first, const quick_cues_data& second) noexcept
    {
        return first.hot_cues == second.hot_cues &&
               first.adjusted_main_cue == second.adjusted_main_cue &&
               first.default_main_cue == second.default_main_cue;
    }

    std::vector<std::byte> encode() const;
    static quick_cues_data decode(const std::vector<std::byte>& compressed_data);
};

struct track_data
{
    stdx::optional<double> sample_rate;
    stdx::optional<int64_t> sample_count;
    stdx::optional<double> average_loudness;  // range (0, 1]
    stdx::optional<musical_key> key;

    track_data() noexcept = default;

    friend bool operator==(
        const track_data& first, const track_data& second) noexcept
    {
        return first.sample_rate == second.sample_rate &&
               first.sample_count == second.sample_count &&
               first.average_loudness == second.average_loudness &&
               first.key == second.key;
    }

    std::vector<std::byte> encode() const;
    static track_data decode(const std::vector<std::byte>& compressed_data);
};

}  // namespace engine::v1
}  // namespace djinterop
