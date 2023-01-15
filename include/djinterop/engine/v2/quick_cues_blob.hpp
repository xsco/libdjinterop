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
#include <djinterop/performance_data.hpp>

namespace djinterop::engine::v2
{
/// Sentinel value for a quick cue sample offset indicating that the quick cue
/// is not set.
static const double QUICK_CUE_SAMPLE_OFFSET_EMPTY = -1;

/// Represents a quick cue in the quick cues blob.
struct DJINTEROP_PUBLIC quick_cue_blob
{
    /// Label.
    std::string label;

    /// Sample offset of cue within the track, or -1 if not set.
    double sample_offset = 0;

    /// Pad color.
    ///
    /// Note that the alpha channel is typically not used, and is usually set to
    /// full brightness.
    pad_color color;

    /// Create an empty quick cue blob.
    ///
    /// \return Returns a quick cue blob.
    [[nodiscard]] static quick_cue_blob empty()
    {
        return quick_cue_blob{
            "", QUICK_CUE_SAMPLE_OFFSET_EMPTY, pad_color{0, 0, 0, 0}};
    }
};

inline bool operator==(const quick_cue_blob& x, const quick_cue_blob& y)
{
    return x.label == y.label && x.sample_offset == y.sample_offset &&
           x.color == y.color;
}

inline bool operator!=(const quick_cue_blob& x, const quick_cue_blob& y)
{
    return !(x == y);
}

/// Represents the quick cues blob.
struct DJINTEROP_PUBLIC quick_cues_blob
{
    /// Type of collection of quick cues.
    typedef std::vector<quick_cue_blob> quick_cue_blobs_type;

    /// List of quick cues.
    quick_cue_blobs_type quick_cues;

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

inline bool operator==(const quick_cues_blob& x, const quick_cues_blob& y)
{
    return x.quick_cues == y.quick_cues &&
           x.adjusted_main_cue == y.adjusted_main_cue &&
           x.is_main_cue_adjusted == y.is_main_cue_adjusted &&
           x.default_main_cue == y.default_main_cue;
}

inline bool operator!=(const quick_cues_blob& x, const quick_cues_blob& y)
{
    return !(x == y);
}

}  // namespace djinterop::engine::v2
