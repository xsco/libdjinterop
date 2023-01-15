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
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::engine::v2
{
/// Represents a point on the overview waveform.
struct DJINTEROP_PUBLIC overview_waveform_point
{
    /// Low-frequency waveform point value.
    uint8_t low_value;

    /// Mid-frequency waveform point value.
    uint8_t mid_value;

    /// High-frequency waveform point value.
    uint8_t high_value;
};

inline bool operator==(
    const overview_waveform_point& x, const overview_waveform_point& y)
{
    return x.low_value == y.low_value && x.mid_value == y.mid_value &&
           x.high_value == y.high_value;
}

inline bool operator!=(
    const overview_waveform_point& x, const overview_waveform_point& y)
{
    return !(x == y);
}

/// Represents the overview waveform data blob.
struct DJINTEROP_PUBLIC overview_waveform_data_blob
{
    /// Number of samples per waveform data point.  May be fractional.
    double samples_per_waveform_point;

    /// Waveform points.
    std::vector<overview_waveform_point> waveform_points;

    /// Maximum values across all waveform points.
    overview_waveform_point maximum_point;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static overview_waveform_data_blob from_blob(
        const std::vector<char>& blob);
};

inline bool operator==(
    const overview_waveform_data_blob& x, const overview_waveform_data_blob& y)
{
    return x.samples_per_waveform_point == y.samples_per_waveform_point &&
           x.waveform_points == y.waveform_points &&
           x.maximum_point == y.maximum_point;
}

inline bool operator!=(
    const overview_waveform_data_blob& x, const overview_waveform_data_blob& y)
{
    return !(x == y);
}

}  // namespace djinterop::engine::v2
