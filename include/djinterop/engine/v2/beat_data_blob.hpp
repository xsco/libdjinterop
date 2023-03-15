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

#if __cplusplus < 201703L
#error This library needs at least a C++17 compliant compiler
#endif

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/pad_color.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop::engine::v2
{
/// Represents a marker within a beat grid, part of the beat data blob.
///
/// Two or more markers form a beat grid, identifying the position of beats
/// within a track, with the assumption that beats between each pair of markers
/// occur at a constant tempo.
///
/// In Engine databases, the first marker is customarily at beat -4 (i.e. before
/// the start of the track), and the last marker is at beat N+1 (i.e. the first
/// beat after the end of the track).  Any markers in between are custom
/// downbeats and can occur at any point in the track.
struct DJINTEROP_PUBLIC beat_grid_marker_blob
{
    /// Sample offset within the track.
    double sample_offset;

    /// Number of this beat.
    int64_t beat_number;

    /// Number of beats until the next marker, or 0 if done.
    int32_t number_of_beats;

    /// Unknown value, can seemingly be set to 0.
    int32_t unknown_value_1;

    friend bool operator==(
        const beat_grid_marker_blob& lhs,
        const beat_grid_marker_blob& rhs) noexcept
    {
        return lhs.sample_offset == rhs.sample_offset &&
               lhs.beat_number == rhs.beat_number &&
               lhs.number_of_beats == rhs.number_of_beats &&
               lhs.unknown_value_1 == rhs.unknown_value_1;
    }

    friend bool operator!=(
        const beat_grid_marker_blob& lhs,
        const beat_grid_marker_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const beat_grid_marker_blob& obj) noexcept
    {
        os << "beat_grid_marker{sample_offset=" << obj.sample_offset
           << ", beat_number=" << obj.beat_number
           << ", number_of_beats=" << obj.number_of_beats
           << ", unknown_value_1=" << obj.unknown_value_1 << "}";
        return os;
    }
};

/// Represents the beat data blob.
struct DJINTEROP_PUBLIC beat_data_blob
{
    /// Type of collection of beat grid markers.
    typedef std::vector<beat_grid_marker_blob> beat_grid_marker_blobs_type;

    /// Sample rate, in hertz.
    double sample_rate;

    /// Number of samples in the track.
    double samples;

    /// Flag indicating if the beat grid is set.
    uint8_t is_beatgrid_set;

    /// List of markers making up the default beat grid.
    beat_grid_marker_blobs_type default_beat_grid;

    /// List of markers making up the adjusted beat grid.
    beat_grid_marker_blobs_type adjusted_beat_grid;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static beat_data_blob from_blob(
        const std::vector<char>& blob);

    friend bool operator==(
        const beat_data_blob& lhs, const beat_data_blob& rhs) noexcept
    {
        return lhs.sample_rate == rhs.sample_rate &&
               lhs.samples == rhs.samples &&
               lhs.is_beatgrid_set == rhs.is_beatgrid_set &&
               lhs.default_beat_grid == rhs.default_beat_grid &&
               lhs.adjusted_beat_grid == rhs.adjusted_beat_grid;
    }

    friend bool operator!=(
        const beat_data_blob& lhs, const beat_data_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const beat_data_blob& obj) noexcept
    {
        os << "beat_data_blob{sample_rate=" << obj.sample_rate
           << ", samples=" << obj.samples
           << ", is_beatgrid_set=" << (int)obj.is_beatgrid_set
           << ", default_beat_grid=[";
        for (auto iter = obj.default_beat_grid.begin();
             iter != obj.default_beat_grid.end(); ++iter)
        {
            if (iter != obj.default_beat_grid.begin())
                os << ", ";

            os << *iter;
        }
        os << "], adjusted_beat_grid=[";
        for (auto iter = obj.adjusted_beat_grid.begin();
             iter != obj.adjusted_beat_grid.end(); ++iter)
        {
            if (iter != obj.adjusted_beat_grid.begin())
                os << ", ";

            os << *iter;
        }
        os << "]}";
        return os;
    }
};
}  // namespace djinterop::engine::v2
