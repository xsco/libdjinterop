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

#include <djinterop/engine/v2/track_table.hpp>

#include "../../util/convert.hpp"

namespace djinterop::engine::v2::convert
{
namespace read
{
inline std::optional<int64_t> album_art_id(int64_t album_art_id)
{
    if (album_art_id == ALBUM_ART_ID_NONE)
    {
        return std::nullopt;
    }

    return std::make_optional(album_art_id);
}

inline std::optional<double> average_loudness(
    const track_data_blob& track_data)
{
    return track_data.average_loudness_low != 0
               ? std::make_optional(track_data.average_loudness_low)
               : std::nullopt;
}

inline std::optional<double> bpm(
    std::optional<double> bpm_analyzed, std::optional<int64_t> bpm)
{
    // Prefer the analysed BPM on account of typically being more accurate, if
    // it is available.
    if (bpm_analyzed)
    {
        return bpm_analyzed;
    }

    return djinterop::util::optional_static_cast<double>(bpm);
}

inline std::optional<std::chrono::milliseconds> duration(int64_t length)
{
    if (length == 0)
        return std::nullopt;

    return std::chrono::milliseconds{length * 1000};
}

inline std::optional<djinterop::musical_key> key(std::optional<int32_t> key)
{
    return djinterop::util::optional_static_cast<djinterop::musical_key>(key);
}

inline std::optional<int> rating(int64_t rating)
{
    if (rating == RATING_NONE)
    {
        return std::nullopt;
    }

    return std::make_optional(static_cast<int>(rating));
}

inline std::optional<unsigned long long> sample_count(
    const track_data_blob& track_data)
{
    if (track_data.samples == 0)
    {
        return std::nullopt;
    }

    return std::make_optional(
        static_cast<unsigned long long>(track_data.samples));
}

inline std::optional<double> sample_rate(const track_data_blob& track_data)
{
    return track_data.sample_rate != 0
               ? std::make_optional(track_data.sample_rate)
               : std::nullopt;
}
}  // namespace read

namespace write
{
inline int64_t album_art_id(std::optional<int64_t> album_art_id)
{
    return album_art_id.value_or(ALBUM_ART_ID_NONE);
}

inline double average_loudness(std::optional<double> average_loudness)
{
    return average_loudness.value_or(0);
}

struct converted_bpm_fields
{
    std::optional<double> bpm_analyzed;
    std::optional<int64_t> bpm;
};

inline converted_bpm_fields bpm(std::optional<double> bpm)
{
    // Deliberate choice to override the BPM as determined by analysis.  This
    // results in the 'least astonishment' for a caller if they set then get the
    // BPM of a track.
    return {bpm, djinterop::util::optional_static_cast<int64_t>(bpm)};
}

inline int64_t duration(std::optional<std::chrono::milliseconds> duration)
{
    return duration.value_or(std::chrono::milliseconds{}).count() / 1000;
}

struct converted_key_fields
{
    std::optional<int32_t> key;
    int32_t track_data_key;
};

inline converted_key_fields key(std::optional<djinterop::musical_key> key)
{
    if (key)
    {
        auto converted = static_cast<int32_t>(*key);
        return {std::make_optional(converted), converted};
    }

    return {std::nullopt, 0};
}

inline int64_t rating(std::optional<int32_t> rating)
{
    return std::clamp(rating.value_or(RATING_NONE), 0, 100);
}

struct converted_sample_count_fields
{
    int64_t track_data_samples;
    double beat_data_samples;
};

inline converted_sample_count_fields sample_count(
    const std::optional<unsigned long long>& sample_count)
{
    auto s = sample_count.value_or(0);
    return {static_cast<int64_t>(s), (double)s};
}

inline double sample_rate(const std::optional<double>& sample_rate)
{
    return sample_rate.value_or(0);
}
}  // namespace write
}  // namespace djinterop::engine::v2::convert
