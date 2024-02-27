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
/// Represents the track data blob.
struct DJINTEROP_PUBLIC track_data_blob
{
    /// Sample rate, in hertz.
    double sample_rate;

    /// Number of samples in the track.
    int64_t samples;

    /// Musical key.
    int32_t key;

    /// Number indicating average loudness for low-frequency sounds.
    ///
    /// On real hardware players, the ratio of average loudness across the three
    /// different frequency bands affects how their relative scale in the
    /// overview waveform is rendered.  Furthermore, the values in isolation
    /// affect how the corresponding frequency band is scaled on the
    /// high-resolution waveform, for peaks to occupy the full rendering area.
    ///
    /// The exact scale of the values this field holds are currently unknown.
    /// However, a value of zero results in no rescaling of the waveforms.
    double average_loudness_low;

    /// Number indicating average loudness for mid-frequency sounds.
    ///
    /// On real hardware players, the ratio of average loudness across the three
    /// different frequency bands affects how their relative scale in the
    /// overview waveform is rendered.  Furthermore, the values in isolation
    /// affect how the corresponding frequency band is scaled on the
    /// high-resolution waveform, for peaks to occupy the full rendering area.
    ///
    /// The exact scale of the values this field holds are currently unknown.
    /// However, a value of zero results in no rescaling of the waveforms.
    double average_loudness_mid;

    /// Number indicating average loudness for high-frequency sounds.
    ///
    /// On real hardware players, the ratio of average loudness across the three
    /// different frequency bands affects how their relative scale in the
    /// overview waveform is rendered.  Furthermore, the values in isolation
    /// affect how the corresponding frequency band is scaled on the
    /// high-resolution waveform, for peaks to occupy the full rendering area.
    ///
    /// The exact scale of the values this field holds are currently unknown.
    /// However, a value of zero results in no rescaling of the waveforms.
    double average_loudness_high;

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
    [[nodiscard]] static track_data_blob from_blob(
        const std::vector<std::byte>& blob);

    friend bool operator==(
        const track_data_blob& lhs, const track_data_blob& rhs) noexcept
    {
        return lhs.sample_rate == rhs.sample_rate &&
               lhs.samples == rhs.samples && lhs.key == rhs.key &&
               lhs.average_loudness_low == rhs.average_loudness_low &&
               lhs.average_loudness_mid == rhs.average_loudness_mid &&
               lhs.average_loudness_high == rhs.average_loudness_high &&
               lhs.extra_data == rhs.extra_data;
    }

    friend bool operator!=(
        const track_data_blob& lhs, const track_data_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const track_data_blob& obj) noexcept
    {
        os << "track_data_blob{sample_rate=" << obj.sample_rate
           << ", samples=" << obj.samples << ", key=" << obj.key
           << ", average_loudness_low=" << obj.average_loudness_low
           << ", average_loudness_mid=" << obj.average_loudness_mid
           << ", average_loudness_high=" << obj.average_loudness_high << "}";
        return os;
    }
};

}  // namespace djinterop::engine::v2
