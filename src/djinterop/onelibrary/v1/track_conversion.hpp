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
#include <optional>
#include <string>

#include <djinterop/musical_key.hpp>
#include <djinterop/onelibrary/v1/content_table.hpp>
#include <djinterop/track_snapshot.hpp>
namespace djinterop::onelibrary::v1
{
// Each column is interpreted in one place, shared by `to_snapshot` and the
// single-field accessors of `track_impl`, so that the two cannot disagree.
// rekordbox writes zero for a numeric field it does not know.

[[nodiscard]] std::optional<double> to_bpm(std::optional<int64_t> bpm_x100);

[[nodiscard]] std::optional<std::chrono::milliseconds> to_duration(
    std::optional<std::chrono::seconds> length);

/// Interpret a count, such as a track number, a year or a bitrate.
[[nodiscard]] std::optional<int> to_positive_int(std::optional<int64_t> value);

[[nodiscard]] std::optional<int> to_rating(std::optional<int64_t> stars);

/// Paths are absolute within the device, as `/Contents/...`, whereas djinterop
/// wants them relative to its root.
[[nodiscard]] std::string to_relative_path(const std::string& path);

/// The inverse of `to_relative_path`.
[[nodiscard]] std::string to_device_path(const std::string& relative_path);

[[nodiscard]] std::optional<double> to_sample_rate(
    std::optional<int64_t> sampling_rate);

[[nodiscard]] std::optional<unsigned long long> to_sample_count(
    std::optional<std::chrono::seconds> length,
    std::optional<int64_t> sampling_rate);

/// Build a track snapshot from a content row.
[[nodiscard]] track_snapshot to_snapshot(const content_row& row);

/// Interpret the key notation that rekordbox writes, such as `F#m` or `Bb`.
///
/// Returns no value for a notation that is not recognised, rather than
/// guessing.
[[nodiscard]] std::optional<musical_key> parse_musical_key(
    const std::string& name);

}  // namespace djinterop::onelibrary::v1
