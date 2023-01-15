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

#include <array>
#include <chrono>
#include <cstdint>

#include <djinterop/engine/v2/loops_blob.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop::engine::v2::convert
{
namespace read
{
inline stdx::optional<djinterop::loop> loop(const loop_blob& l)
{
    return (l.is_start_set || l.is_end_set)
               ? stdx::make_optional(djinterop::loop{
                     l.label, l.start_sample_offset, l.end_sample_offset,
                     l.color})
               : stdx::nullopt;
}

inline std::array<stdx::optional<djinterop::loop>, 8> loops(
    const loops_blob& loops)
{
    std::array<stdx::optional<djinterop::loop>, 8> converted;
    for (auto i = 0; i < loops.loops.size() && i < 8; ++i)
    {
        converted[i] = loop(loops.loops[i]);
    }

    return converted;
}
}  // namespace read

namespace write
{
inline loop_blob loop(stdx::optional<djinterop::loop> loop)
{
    if (!loop)
        return loop_blob::empty();

    return loop_blob{loop->label,
                     loop->start_sample_offset,
                     loop->end_sample_offset,
                     true,
                     true,
                     loop->color};
}

inline loops_blob loops(
    const std::array<stdx::optional<djinterop::loop>, 8>& loops)
{
    loops_blob converted;
    for (auto&& l : loops)
    {
        converted.loops.push_back(loop(l));
    }

    return converted;
}
}  // namespace write
}  // namespace djinterop::engine::v2::convert
