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

#include <djinterop/engine/v2/quick_cues_blob.hpp>

namespace djinterop::engine::v3
{
/// Maximum number of supported quick cues.
constexpr size_t MAX_QUICK_CUES = djinterop::engine::v2::MAX_QUICK_CUES;

/// Sentinel value for a quick cue sample offset indicating that the quick cue
/// is not set.
constexpr double QUICK_CUE_SAMPLE_OFFSET_EMPTY = djinterop::engine::v2::QUICK_CUE_SAMPLE_OFFSET_EMPTY;

/// Represents a quick cue in the quick cues blob.
using quick_cue_blob = djinterop::engine::v2::quick_cue_blob;

/// Represents the quick cues blob.
using quick_cues_blob = djinterop::engine::v2::quick_cues_blob;

}  // namespace djinterop::engine::v3
