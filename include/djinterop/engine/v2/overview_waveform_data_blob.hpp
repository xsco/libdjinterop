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

#include <cstddef>
#include <cstdint>
#include <ostream>
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

    friend bool operator==(
        const overview_waveform_point& lhs,
        const overview_waveform_point& rhs) noexcept
    {
        return lhs.low_value == rhs.low_value &&
               lhs.mid_value == rhs.mid_value &&
               lhs.high_value == rhs.high_value;
    }

    friend bool operator!=(
        const overview_waveform_point& lhs,
        const overview_waveform_point& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const overview_waveform_point& obj) noexcept
    {
        os << "overview_waveform_point{low_value=" << obj.low_value
           << ", mid_value=" << obj.mid_value
           << ", high_value=" << obj.high_value << "}";
        return os;
    }
};

/// Represents the overview waveform data blob.
struct DJINTEROP_PUBLIC overview_waveform_data_blob
{
    /// Number of samples per waveform data point.  May be fractional.
    double samples_per_waveform_point;

    /// Waveform points.
    std::vector<overview_waveform_point> waveform_points;

    /// Maximum values across all waveform points.
    overview_waveform_point maximum_point;

    /// Extra data (if any) found in a decoded blob.
    std::vector<std::byte> extra_data;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<std::byte> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static overview_waveform_data_blob from_blob(
        const std::vector<std::byte>& blob);

    friend bool operator==(
        const overview_waveform_data_blob& lhs,
        const overview_waveform_data_blob& rhs) noexcept
    {
        return lhs.samples_per_waveform_point ==
                   rhs.samples_per_waveform_point &&
               lhs.waveform_points == rhs.waveform_points &&
               lhs.maximum_point == rhs.maximum_point &&
               lhs.extra_data == rhs.extra_data;
    }

    friend bool operator!=(
        const overview_waveform_data_blob& lhs,
        const overview_waveform_data_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const overview_waveform_data_blob& obj) noexcept
    {
        os << "overview_waveform_data_blob{samples_per_waveform_point="
           << obj.samples_per_waveform_point << ", waveform_points=[#"
           << obj.waveform_points.size()
           << "], maximum_point=" << obj.maximum_point << "}";
        return os;
    }
};
}  // namespace djinterop::engine::v2
