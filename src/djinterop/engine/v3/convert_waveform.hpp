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
#include <vector>

#include <djinterop/engine/v3/overview_waveform_data_blob.hpp>
#include <djinterop/performance_data.hpp>

#include "../v2/convert_waveform.hpp"

namespace djinterop::engine::v3::convert
{
namespace read
{
inline waveform_entry waveform_entry(const overview_waveform_point& p)
{
    return v2::convert::read::waveform_entry(p);
}

inline std::vector<djinterop::waveform_entry> waveform(
    const overview_waveform_data_blob& w)
{
    return v2::convert::read::waveform(w);
}
}  // namespace read

namespace write
{
inline overview_waveform_point waveform_entry(const waveform_entry& p)
{
    return v2::convert::write::waveform_entry(p);
}

inline overview_waveform_data_blob waveform(
    const std::vector<djinterop::waveform_entry>& w,
    std::optional<unsigned long long> sample_count,
    std::optional<double> sample_rate)
{
    return v2::convert::write::waveform(w, sample_count, sample_rate);
}
}  // namespace write
}  // namespace djinterop::engine::v3::convert
