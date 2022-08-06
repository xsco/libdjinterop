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

#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/pad_color.hpp>

namespace djinterop::engine::v2
{
/// Represents a quick cue in the quick cues blob.
struct DJINTEROP_PUBLIC quick_cue_blob
{
    /// Label.
    std::string label;

    /// Sample offset within the track.
    double sample_offset = 0;

    /// Pad color.
    ///
    /// Note that the alpha channel is typically not used, and is usually set to
    /// full brightness.
    pad_color color;
};

/// Represents the quick cues blob.
struct DJINTEROP_PUBLIC quick_cues_blob
{
    /// List of quick cues.
    std::vector<stdx::optional<quick_cue_blob> > quick_cues;

    /// Adjusted main cue point.
    double adjusted_main_cue;

    /// Flag indicating whether the main cue point has been adjusted from the
    /// default.
    bool is_main_cue_adjusted;

    /// Default cue point.
    double default_main_cue;

    /// Encode this struct into binary blob form.
    ///
    /// \return Returns a byte array.
    [[nodiscard]] std::vector<char> to_blob() const;

    /// Decode an instance of this struct from binary blob form.
    ///
    /// \param blob Binary blob.
    /// \return Returns a decoded instance of this struct.
    [[nodiscard]] static quick_cues_blob from_blob(
        const std::vector<char>& blob);
};

}  // namespace djinterop::engine::v2
