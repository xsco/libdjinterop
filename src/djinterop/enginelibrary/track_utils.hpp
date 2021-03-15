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

namespace djinterop::enginelibrary::util
{

/// Calculate the quantisation number for waveforms, given a sample rate.
///
/// A few numbers written to the waveform performance data are rounded
/// to multiples of a particular "quantisation number", that is equal to
/// the sample rate divided by 105, and then rounded to the nearest
/// multiple of two.
inline int64_t waveform_quantisation_number(int64_t sample_rate)
{
    return (sample_rate / 210) * 2;
}

/// Get the required number of samples per waveform entry.
///
/// The waveform for a track is provided merely as a set of waveform points,
/// and so the scale of it is only meaningful when a relationship between
/// the waveform and the samples it represents is known.  This function
/// provides the required number of samples per waveform entry that should
/// be understood when constructing or reading waveforms in Engine Prime format.
inline int64_t required_waveform_samples_per_entry(double sample_rate)
{
    // In high-resolution waveforms, the samples-per-entry is the same as
    // the quantisation number.
    return waveform_quantisation_number(sample_rate);
}


}  // namespace djinterop::enginelibrary::util
