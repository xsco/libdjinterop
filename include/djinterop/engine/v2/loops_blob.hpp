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
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/pad_color.hpp>

namespace djinterop::engine::v2
{
/// Represents a loop within the loops blob.
struct DJINTEROP_PUBLIC loop_blob
{
    /// Label.
    std::string label;

    /// Sample offset of the start of the loop.
    double start_sample_offset;

    /// Sample offset of end of the loop.
    double end_sample_offset;

    /// Flag indicating whether the start of the loop is set.
    uint8_t is_start_set;

    /// Flag indicating whether the end of the loop is set.
    uint8_t is_end_set;

    /// Pad color.
    pad_color color;
};

/// Represents the loops blob.
struct DJINTEROP_PUBLIC loops_blob
{
    /// List of loops.
    std::vector<stdx::optional<loop_blob> > loops;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static loops_blob from_blob(
            const std::vector<char>& blob);
};

}  // namespace djinterop::engine::v2
