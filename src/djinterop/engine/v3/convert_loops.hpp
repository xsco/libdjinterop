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

#include <optional>

#include <djinterop/engine/v3/loops_blob.hpp>
#include <djinterop/performance_data.hpp>

#include "../v2/convert_loops.hpp"

namespace djinterop::engine::v3::convert
{
namespace read
{
inline std::optional<loop> loop(const loop_blob& l)
{
    return v2::convert::read::loop(l);
}

inline std::vector<std::optional<djinterop::loop>> loops(
    const loops_blob& loops)
{
    return v2::convert::read::loops(loops);
}
}  // namespace read

namespace write
{
inline loop_blob loop(std::optional<loop> loop)
{
    return v2::convert::write::loop(loop);
}

inline loops_blob loops(
    const std::vector<std::optional<djinterop::loop>>& loops)
{
    return v2::convert::write::loops(loops);
}
}  // namespace write
}  // namespace djinterop::engine::v3::convert
