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

#include <chrono>
#include <cstdint>
#include <vector>

#include <djinterop/engine/v2/quick_cues_blob.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop::engine::v2::convert
{
namespace read
{
inline stdx::optional<djinterop::hot_cue> hot_cue(
    const quick_cue_blob& quick_cue)
{
    return quick_cue.sample_offset == QUICK_CUE_SAMPLE_OFFSET_EMPTY
               ? stdx::nullopt
               : stdx::make_optional(djinterop::hot_cue{
                     quick_cue.label, quick_cue.sample_offset,
                     quick_cue.color});
}

inline std::vector<stdx::optional<djinterop::hot_cue>> hot_cues(
    const quick_cues_blob& quick_cues)
{
    std::vector<stdx::optional<djinterop::hot_cue>> converted;
    converted.reserve(quick_cues.quick_cues.size());
    for (auto&& c : quick_cues.quick_cues)
        converted.push_back(hot_cue(c));

    return converted;
}

inline stdx::optional<double> main_cue(double main_cue)
{
    return main_cue != 0 ? stdx::make_optional(main_cue) : stdx::nullopt;
}
}  // namespace read

namespace write
{
inline quick_cue_blob hot_cue(const stdx::optional<hot_cue>& hot_cue)
{
    if (!hot_cue)
        return quick_cue_blob::empty();

    return quick_cue_blob{
        hot_cue->label, hot_cue->sample_offset, hot_cue->color};
}

inline std::vector<quick_cue_blob> hot_cues(
    const std::vector<stdx::optional<djinterop::hot_cue>>& cues)
{
    if (cues.size() > MAX_QUICK_CUES)
        throw djinterop::hot_cues_overflow{
            "Number of hot cues to write exceeds maximum"};

    std::vector<quick_cue_blob> converted;
    converted.reserve(cues.size());
    for (auto&& cue : cues)
    {
        converted.push_back(hot_cue(cue));
    }

    // Additional hot cues are written to pad to the maximum potential number.
    while (converted.size() < MAX_QUICK_CUES)
        converted.push_back(quick_cue_blob::empty());

    return converted;
}

inline double main_cue(stdx::optional<double> main_cue)
{
    return main_cue.value_or(0);
}
}  // namespace write
}  // namespace djinterop::engine::v2::convert
