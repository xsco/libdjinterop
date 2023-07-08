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

#include <djinterop/performance_data.hpp>

namespace djinterop::engine::util
{
constexpr const unsigned long long OVERVIEW_WAVEFORM_SIZE = 1024;

inline int64_t waveform_quantisation_number(double sample_rate)
{
    // A few numbers written to the waveform performance data are rounded
    // to multiples of a particular "quantisation number", that is equal to
    // the sample rate divided by 105, and then rounded to the nearest
    // multiple of two.
    return (static_cast<int64_t>(sample_rate) / 210) * 2;
}

inline waveform_extents calculate_high_resolution_waveform_extents(
    unsigned long long sample_count, double sample_rate)
{
    // In high-resolution waveforms, the samples-per-entry is the same as
    // the quantisation number.
    auto qn = waveform_quantisation_number(sample_rate);

    if (sample_count == 0 || qn == 0)
    {
        return {0, 0};
    }

    auto size = (sample_count + qn - 1) / qn;  // Ceiling division
    return waveform_extents{size, static_cast<double>(qn)};
}

inline waveform_extents calculate_overview_waveform_extents(
    unsigned long long sample_count, double sample_rate)
{
    // An overview waveform always has a fixed number of entries, and the number
    // of samples that each one represents must be calculated from the true
    // sample count by rounding the number of samples to the quantisation number
    // first.
    auto qn = waveform_quantisation_number(sample_rate);

    if (sample_count == 0 || qn == 0)
    {
        return {0, 0};
    }

    auto rounded_sample_count = (sample_count / qn) * qn;
    auto spe =
        static_cast<double>(rounded_sample_count) / OVERVIEW_WAVEFORM_SIZE;
    return waveform_extents{OVERVIEW_WAVEFORM_SIZE, spe};
}
}  // namespace djinterop::engine::util
