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

#include <vector>

#include <djinterop/engine/v3/quick_cues_blob.hpp>
#include <djinterop/performance_data.hpp>

#include "../v2/convert_hot_cues.hpp"

namespace djinterop::engine::v3::convert
{
namespace read
{
inline std::optional<hot_cue> hot_cue(const quick_cue_blob& quick_cue)
{
    return v2::convert::read::hot_cue(quick_cue);
}

inline std::vector<std::optional<djinterop::hot_cue>> hot_cues(
    const quick_cues_blob& quick_cues)
{
    return v2::convert::read::hot_cues(quick_cues);
}

inline std::optional<double> main_cue(double main_cue)
{
    return v2::convert::read::main_cue(main_cue);
}
}  // namespace read

namespace write
{
inline quick_cue_blob hot_cue(const std::optional<hot_cue>& hot_cue)
{
    return v2::convert::write::hot_cue(hot_cue);
}

inline std::vector<quick_cue_blob> hot_cues(
    const std::vector<std::optional<djinterop::hot_cue>>& cues)
{
    return v2::convert::write::hot_cues(cues);
}

inline double main_cue(std::optional<double> main_cue)
{
    return v2::convert::write::main_cue(main_cue);
}
}  // namespace write
}  // namespace djinterop::engine::v3::convert
