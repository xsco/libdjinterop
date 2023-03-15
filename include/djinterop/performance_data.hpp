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
#ifndef DJINTEROP_PERFORMANCE_DATA_HPP
#define DJINTEROP_PERFORMANCE_DATA_HPP

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <cstdint>
#include <ostream>
#include <string>

#include <djinterop/config.hpp>
#include <djinterop/pad_color.hpp>

namespace djinterop
{
struct sampling_info
{
    double sample_rate = 0;  // usually 44100.0 or 48000.0
    int64_t sample_count = 0;

    friend bool operator==(
        const sampling_info& lhs, const sampling_info& rhs) noexcept
    {
        return lhs.sample_rate == rhs.sample_rate &&
               lhs.sample_count == rhs.sample_count;
    }

    friend bool operator!=(
        const sampling_info& lhs, const sampling_info& rhs) noexcept
    {
        return !(rhs == lhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::sampling_info& obj) noexcept
    {
        os << "sampling_info{sample_rate=" << obj.sample_rate
          << ", sample_count=" << obj.sample_count << "}";
        return os;
    }
};

struct beatgrid_marker
{
    int32_t index = 0;
    double sample_offset = 0;

    friend bool operator==(
        const beatgrid_marker& lhs, const beatgrid_marker& rhs) noexcept
    {
        return lhs.index == rhs.index && lhs.sample_offset == rhs.sample_offset;
    }

    friend bool operator!=(
        const beatgrid_marker& lhs, const beatgrid_marker& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::beatgrid_marker& obj) noexcept
    {
        os << "beatgrid_marker{index=" << obj.index
          << ", sample_offset=" << obj.sample_offset << "}";
        return os;
    }
};

struct hot_cue
{
    std::string label;
    double sample_offset = 0;
    pad_color color;

    friend bool operator==(const hot_cue& lhs, const hot_cue& rhs) noexcept
    {
        return lhs.label == rhs.label &&
               lhs.sample_offset == rhs.sample_offset && lhs.color == rhs.color;
    }

    friend bool operator!=(const hot_cue& lhs, const hot_cue& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::hot_cue& obj) noexcept
    {
        os << "loop{label=" << obj.label
          << ", sample_offset=" << obj.sample_offset << ", color=" << obj.color
          << "}";
        return os;
    }
};

struct loop
{
    std::string label;
    double start_sample_offset = 0;
    double end_sample_offset = 0;
    pad_color color;

    friend bool operator==(const loop& lhs, const loop& rhs) noexcept
    {
        return lhs.label == rhs.label &&
               lhs.start_sample_offset == rhs.start_sample_offset &&
               lhs.end_sample_offset == rhs.end_sample_offset &&
               lhs.color == rhs.color;
    }

    friend bool operator!=(const loop& lhs, const loop& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::loop& obj) noexcept
    {
        os << "loop{label=" << obj.label
          << ", start_sample_offset=" << obj.start_sample_offset
          << ", end_sample_offset=" << obj.end_sample_offset
          << ", color=" << obj.color << "}";
        return os;
    }
};

struct waveform_point
{
    uint8_t value = 0;
    uint8_t opacity = 255;

    friend bool operator==(
        const waveform_point& lhs, const waveform_point& rhs) noexcept
    {
        return lhs.value == rhs.value && lhs.opacity == rhs.opacity;
    }

    friend bool operator!=(
        const waveform_point& lhs, const waveform_point& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::waveform_point& obj) noexcept
    {
        os << "waveform_point{value=" << (int)obj.value
          << ", opacity=" << (int)obj.opacity;
        return os;
    }
};

/**
 * A single high-resolution waveform entry
 *
 * Note that, when rendering the high-resolution waveform, each individual
 * band is scaled so that the largest value across the entire waveform hits the
 * top of the display.  Note also that the mid frequency is always drawn over
 * the low, and the high frequency is always drawn over the low and mid, meaning
 * that very loud high-frequency sounds will hide any low or mid activity on the
 * waveform rendering.
 *
 * A further note is that when the opacity is set to zero, this appears to
 * translate into roughly 50% opacity on a real rendering.
 */
struct waveform_entry
{
    waveform_point low;
    waveform_point mid;
    waveform_point high;

    friend bool operator==(
        const waveform_entry& lhs, const waveform_entry& rhs) noexcept
    {
        return lhs.low == rhs.low && lhs.mid == rhs.mid && lhs.high == rhs.high;
    }

    friend bool operator!=(
        const waveform_entry& lhs, const waveform_entry& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const djinterop::waveform_entry& obj) noexcept
    {
        os << "waveform_entry{low=" << obj.low << ", mid=" << obj.mid
          << ", high=" << obj.high << "}";
        return os;
    }
};

}  // namespace djinterop

#endif  // DJINTEROP_PERFORMANCE_DATA_HPP
