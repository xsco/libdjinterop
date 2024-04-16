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

#if __cplusplus < 202002L
#error This library needs at least a C++20 compliant compiler
#endif

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

#include <djinterop/config.hpp>
#include <djinterop/pad_color.hpp>

namespace djinterop::engine::v2
{
/// Maximum number of supported quick cues.
constexpr const size_t MAX_QUICK_CUES = 8;

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

    friend bool operator==(
        const quick_cue_blob& lhs, const quick_cue_blob& rhs) noexcept
    {
        return lhs.label == rhs.label &&
               lhs.sample_offset == rhs.sample_offset && lhs.color == rhs.color;
    }

    friend bool operator!=(
        const quick_cue_blob& lhs, const quick_cue_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const quick_cue_blob& obj) noexcept
    {
        os << "quick_cue_blob{label=" << obj.label
           << ", sample_offset=" << obj.sample_offset << ", color=" << obj.color
           << "}";
        return os;
    }
};

/// Represents the quick cues blob.
struct DJINTEROP_PUBLIC quick_cues_blob
{
    /// Type of collection of quick cues.
    using quick_cue_blobs_type = std::vector<quick_cue_blob>;

    /// List of quick cues.
    quick_cue_blobs_type quick_cues;

    /// Adjusted main cue point.
    double adjusted_main_cue;

    /// Flag indicating whether the main cue point has been adjusted from the
    /// default.
    bool is_main_cue_adjusted;

    /// Default cue point.
    double default_main_cue;

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
    [[nodiscard]] static quick_cues_blob from_blob(
        const std::vector<std::byte>& blob);

    friend bool operator==(
        const quick_cues_blob& lhs, const quick_cues_blob& rhs) noexcept
    {
        return lhs.quick_cues == rhs.quick_cues &&
               lhs.adjusted_main_cue == rhs.adjusted_main_cue &&
               lhs.is_main_cue_adjusted == rhs.is_main_cue_adjusted &&
               lhs.default_main_cue == rhs.default_main_cue &&
               lhs.extra_data == rhs.extra_data;
    }

    friend bool operator!=(
        const quick_cues_blob& lhs, const quick_cues_blob& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& os, const quick_cues_blob& obj) noexcept
    {
        os << "quick_cues_blob{quick_cues=[";
        for (auto iter = obj.quick_cues.begin(); iter != obj.quick_cues.end();
             ++iter)
        {
            if (iter != obj.quick_cues.begin())
                os << ", ";

            os << *iter;
        }
        os << "], adjusted_main_cue=" << obj.adjusted_main_cue
           << ", is_main_cue_adjusted=" << obj.is_main_cue_adjusted
           << ", default_main_cue=" << obj.default_main_cue << "}";
        return os;
    }
};
}  // namespace djinterop::engine::v2
