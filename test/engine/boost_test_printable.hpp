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
#include <ostream>

#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop
{
inline std::ostream& operator<<(
    std::ostream& o, const djinterop::beatgrid_marker& v)
{
    o << "beatgrid_marker{index=" << v.index
      << ", sample_offset=" << v.sample_offset << "}";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const djinterop::pad_color& v)
{
    o << "pad_color{r=" << v.r << ", g=" << v.g << ", b=" << v.b
      << ", a=" << v.a;
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const djinterop::hot_cue& v)
{
    o << "loop{label=" << v.label << ", sample_offset=" << v.sample_offset
      << ", color=" << v.color << "}";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const djinterop::loop& v)
{
    o << "loop{label=" << v.label
      << ", start_sample_offset=" << v.start_sample_offset
      << ", end_sample_offset=" << v.end_sample_offset << ", color=" << v.color
      << "}";
    return o;
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::musical_key& v)
{
    o << "musical_key{" << (int)v << "}";
    return o;
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::sampling_info& v)
{
    o << "sampling_info{sample_rate=" << v.sample_rate
      << ", sample_count=" << v.sample_count << "}";
    return o;
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::waveform_point& v)
{
    o << "waveform_point{value=" << v.value << ", opacity=" << v.opacity;
    return o;
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::waveform_entry& v)
{
    o << "waveform_entry{low=" << v.low << ", mid=" << v.mid
      << ", high=" << v.high << "}";
    return o;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const djinterop::stdx::optional<T> v)
{
    if (v)
    {
        o << *v;
    }
    else
    {
        o << "(null)";
    }

    return o;
}

}  // namespace djinterop

/// Printable optional wrapper.
///
/// Boost does not know how to print std::optional, and we cannot add overloads
/// for operator<< as it lives in the std namespace, which is forbidden to be
/// extended.  This thin wrapper provides appropriate support, at the syntactic
/// expense of having to wrap optional types at the time of comparison.
template <typename T>
struct printable_optional
{
    djinterop::stdx::optional<T> value;
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const printable_optional<T>& v)
{
    if (v.value)
    {
        return o << *v.value;
    }
    else
    {
        return o << "nullopt";
    }
}

template <typename Clock, typename Duration>
std::ostream& operator<<(
    std::ostream& o,
    const printable_optional<std::chrono::time_point<Clock, Duration> >& v)
{
    if (v.value)
    {
        return o << v.value->time_since_epoch().count();
    }
    else
    {
        return o << "nullopt";
    }
}

template <typename Rep, typename Period>
std::ostream& operator<<(
    std::ostream& o,
    const printable_optional<std::chrono::duration<Rep, Period> >& v)
{
    if (v.value)
    {
        return o << v.value->count();
    }
    else
    {
        return o << "nullopt";
    }
}

template <class T, class U>
bool operator==(
    const printable_optional<T>& lhs, const printable_optional<U>& rhs)
{
    return (lhs.value && rhs.value && *lhs.value == *rhs.value) ||
           (!lhs.value && !rhs.value);
}

template <class T, class U>
bool operator!=(
    const printable_optional<T>& lhs, const printable_optional<U>& rhs)
{
    return !(lhs == rhs);
}

template <typename T>
auto pr(const djinterop::stdx::optional<T>& opt)
{
    return printable_optional<T>{opt};
}
