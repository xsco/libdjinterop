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

    /// Number (possibly?) indicating average loudness.
    double average_loudness;

    /// Musical key.
    int32_t key;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static track_data_blob from_blob(
        const std::vector<char>& blob);

    friend bool operator==(
        const track_data_blob& lhs, const track_data_blob& rhs) noexcept
    {
        return lhs.sample_rate == rhs.sample_rate &&
               lhs.samples == rhs.samples &&
               lhs.average_loudness == rhs.average_loudness &&
               lhs.key == rhs.key;
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
           << ", samples=" << obj.samples
           << ", average_loudness=" << obj.average_loudness
           << ", key=" << obj.key << "}";
        return os;
    }
};

}  // namespace djinterop::engine::v2
