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

#include <djinterop/engine/v2/loops_blob.hpp>

namespace djinterop::engine::v3
{
/// Maximum number of supported loops.
constexpr size_t MAX_LOOPS = djinterop::engine::v2::MAX_LOOPS;

/// Represents a loop within the loops blob.
using loop_blob = djinterop::engine::v2::loop_blob;

/// Represents the loops blob.
using loops_blob = djinterop::engine::v2::loops_blob;

}  // namespace djinterop::engine::v3
