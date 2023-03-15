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
/// Represents a loop within the loops blob.
struct DJINTEROP_PUBLIC loop_blob
{
    /// Label.
    std::string label;

    /// Sample offset of the start of the loop, or -1 if the loop is not set.
    double start_sample_offset;

    /// Sample offset of end of the loop, or -1 if the loop is not set.
    double end_sample_offset;

    /// Flag indicating whether the start of the loop is set.
    uint8_t is_start_set;

    /// Flag indicating whether the end of the loop is set.
    uint8_t is_end_set;

    /// Pad color.
    pad_color color;

    /// Create an empty loop blob.
    ///
    /// \return Returns a loop blob.
    [[nodiscard]] static loop_blob empty()
    {
        return loop_blob{"", -1, -1, 0, 0, pad_color{0, 0, 0, 0}};
    }

    friend bool operator==(const loop_blob& lhs, const loop_blob& rhs) noexcept
    {
        return lhs.label == rhs.label &&
               lhs.start_sample_offset == rhs.start_sample_offset &&
               lhs.end_sample_offset == rhs.end_sample_offset &&
               lhs.is_start_set == rhs.is_start_set &&
               lhs.is_end_set == rhs.is_end_set && lhs.color == rhs.color;
    }

    friend bool operator!=(const loop_blob& lhs, const loop_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const loop_blob& obj) noexcept
    {
        os << "loop_blob{label=" << obj.label
           << ", start_sample_offset=" << obj.start_sample_offset
           << ", end_sample_offset=" << obj.end_sample_offset
           << ", is_start_set=" << (int)obj.is_start_set
           << ", is_end_set=" << (int)obj.is_end_set << ", color=" << obj.color
           << "}";
        return os;
    }
};

/// Represents the loops blob.
struct DJINTEROP_PUBLIC loops_blob
{
    /// List of loops.
    std::vector<loop_blob> loops;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static loops_blob from_blob(const std::vector<char>& blob);

    friend bool operator==(
        const loops_blob& lhs, const loops_blob& rhs) noexcept
    {
        return lhs.loops == rhs.loops;
    }

    friend bool operator!=(
        const loops_blob& lhs, const loops_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const loops_blob& obj) noexcept
    {
        os << "loops_blob{loops=[";
        for (auto iter = obj.loops.begin(); iter != obj.loops.end(); ++iter)
        {
            if (iter != obj.loops.begin())
                os << ", ";

            os << *iter;
        }
        os << "]}";
        return os;
    }
};
}  // namespace djinterop::engine::v2
