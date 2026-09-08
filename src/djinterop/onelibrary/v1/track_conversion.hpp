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

#include <string>

#include <djinterop/musical_key.hpp>
#include <djinterop/onelibrary/v1/content_table.hpp>
#include <djinterop/track_snapshot.hpp>
namespace djinterop::onelibrary::v1
{
/// Build a track snapshot from a content row.
[[nodiscard]] track_snapshot to_snapshot(const content_row& row);

/// Interpret the key notation that rekordbox writes, such as `F#m` or `Bb`.
///
/// Returns no value for a notation that is not recognised, rather than
/// guessing.
[[nodiscard]] std::optional<musical_key> parse_musical_key(
    const std::string& name);

}  // namespace djinterop::onelibrary::v1
