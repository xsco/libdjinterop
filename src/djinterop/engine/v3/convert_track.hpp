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

#include <djinterop/engine/v3/track_table.hpp>

#include "../v2/convert_track.hpp"

namespace djinterop::engine::v3::convert
{
namespace read
{
inline std::optional<int64_t> album_art_id(int64_t album_art_id)
{
    return v2::convert::read::album_art_id(album_art_id);
}

inline std::optional<double> average_loudness(const track_data_blob& track_data)
{
    return v2::convert::read::average_loudness(track_data);
}

inline std::optional<double> bpm(
    std::optional<double> bpm_analyzed, std::optional<int64_t> bpm)
{
    return v2::convert::read::bpm(bpm_analyzed, bpm);
}

inline std::optional<std::chrono::milliseconds> duration(int64_t length)
{
    return v2::convert::read::duration(length);
}

inline std::optional<musical_key> key(std::optional<int32_t> key)
{
    return v2::convert::read::key(key);
}

inline std::optional<int> rating(int64_t rating)
{
    return v2::convert::read::rating(rating);
}

inline std::optional<unsigned long long> sample_count(
    const track_data_blob& track_data)
{
    return v2::convert::read::sample_count(track_data);
}

inline std::optional<double> sample_rate(const track_data_blob& track_data)
{
    return v2::convert::read::sample_rate(track_data);
}
}  // namespace read

namespace write
{
inline int64_t album_art_id(std::optional<int64_t> album_art_id)
{
    return v2::convert::write::album_art_id(album_art_id);
}

inline double average_loudness(std::optional<double> average_loudness)
{
    return v2::convert::write::average_loudness(average_loudness);
}

inline v2::convert::write::converted_bpm_fields bpm(std::optional<double> bpm)
{
    return v2::convert::write::bpm(bpm);
}

inline int64_t duration(std::optional<std::chrono::milliseconds> duration)
{
    return v2::convert::write::duration(duration);
}

inline v2::convert::write::converted_key_fields key(
    std::optional<musical_key> key)
{
    return v2::convert::write::key(key);
}

inline int64_t rating(std::optional<int32_t> rating)
{
    return v2::convert::write::rating(rating);
}

inline v2::convert::write::converted_sample_count_fields sample_count(
    const std::optional<unsigned long long>& sample_count)
{
    return v2::convert::write::sample_count(sample_count);
}

inline double sample_rate(const std::optional<double>& sample_rate)
{
    return v2::convert::write::sample_rate(sample_rate);
}
}  // namespace write
}  // namespace djinterop::engine::v3::convert
