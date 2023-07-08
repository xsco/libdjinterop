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
/// A marker in a beatgrid.
///
/// A beatgrid is formed from a set of markers, each of which identify a given
/// beat at a given sample offset.  Each beat is given a number, which allows
/// for a pair of beatgrid markers to identify a span of audio with a set of
/// beats between them of regular intervals.
struct DJINTEROP_PUBLIC beatgrid_marker
{
    /// Beat index.
    int index = 0;

    // Sample offset within the track.
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

/// Hot cue, representing a named stored location within a track.
struct DJINTEROP_PUBLIC hot_cue
{
    /// Label for the hot cue.
    std::string label;

    /// Sample offset of the cue within the track.
    double sample_offset = 0;

    /// Color of the hot cue.
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
        os << "hot_cue{label=" << obj.label
           << ", sample_offset=" << obj.sample_offset << ", color=" << obj.color
           << "}";
        return os;
    }
};

/// Loop, representing a named repeatable region within a track.
struct DJINTEROP_PUBLIC loop
{
    /// Label for the loop.
    std::string label;

    /// Starting sample offset of the loop within the track.
    double start_sample_offset = 0;

    /// Ending sample offset of the loop within the track.
    double end_sample_offset = 0;

    /// Color of the loop.
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

/// Waveform point.
///
/// Note that waveforms are considered to have a value only in the positive
/// axis.
struct DJINTEROP_PUBLIC waveform_point
{
    /// Waveform value, representing absolute amplitude, in the range `0`-`255`.
    uint8_t value = 0;

    /// Waveform opacity, in the range `0`-`255`, with `0` being fully
    /// transparent and `255` being fully opaque.
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

/// Waveform entry.
///
/// A waveform comprises a set of waveform entries, each of which specifies
/// points on up to three waves.  The three waves represent low, medium, and
/// high frequency audio within a track.
struct DJINTEROP_PUBLIC waveform_entry
{
    /// Waveform point for low-frequency audio.
    waveform_point low;

    /// Waveform point for mid-frequency audio.
    waveform_point mid;

    /// Waveform point for high-frequency audio.
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

/// The `waveform_extents` struct describes the size of a waveform and its
/// relationship to samples of audio.
struct DJINTEROP_PUBLIC waveform_extents
{
    /// The number of entries in the waveform.
    unsigned long long size;

    /// The number of samples that each entry in the waveform represents.
    double samples_per_entry;

    friend bool operator==(
        const waveform_extents& lhs, const waveform_extents& rhs)
    {
        return lhs.size == rhs.size &&
               lhs.samples_per_entry == rhs.samples_per_entry;
    }

    friend bool operator!=(
        const waveform_extents& lhs, const waveform_extents& rhs)
    {
        return !(rhs == lhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const waveform_extents& extents)
    {
        os << "waveform_extents{size=" << extents.size
           << ", samples_per_entry=" << extents.samples_per_entry << "}";
        return os;
    }
};

}  // namespace djinterop

#endif  // DJINTEROP_PERFORMANCE_DATA_HPP
