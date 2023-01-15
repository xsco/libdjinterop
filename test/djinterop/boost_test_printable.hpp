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

#include <djinterop/engine/v2/beat_data_blob.hpp>
#include <djinterop/engine/v2/change_log_table.hpp>
#include <djinterop/engine/v2/information_table.hpp>
#include <djinterop/engine/v2/loops_blob.hpp>
#include <djinterop/engine/v2/overview_waveform_data_blob.hpp>
#include <djinterop/engine/v2/playlist_entity_table.hpp>
#include <djinterop/engine/v2/playlist_table.hpp>
#include <djinterop/engine/v2/quick_cues_blob.hpp>
#include <djinterop/engine/v2/track_data_blob.hpp>
#include <djinterop/engine/v2/track_table.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace
{
template <typename T>
std::ostream& print(std::ostream& os, const T& v)
{
    return os << v;
}

template <typename T>
std::ostream& print(std::ostream& os, const djinterop::stdx::optional<T>& v)
{
    if (v)
        return print(os, *v);
    else
        return os << "nullopt";
}

template <typename Clock, typename Duration>
std::ostream& print(
    std::ostream& os,
    const djinterop::stdx::optional<std::chrono::time_point<Clock, Duration> >&
        v)
{
    if (v)
        return os << v->time_since_epoch().count();
    else
        return os << "nullopt";
}

template <typename Clock, typename Duration>
std::ostream& print(
    std::ostream& os, const std::chrono::time_point<Clock, Duration>& v)
{
    return os << v.time_since_epoch().count();
}

template <typename T>
std::ostream& print(std::ostream& os, const std::vector<T>& v)
{
    os << "[";
    for (auto ii = v.begin(); ii != v.end(); ++ii)
    {
        if (ii != v.begin())
            os << ", ";

        os << *ii;
    }
    os << "]";
    return os;
}
}  // anonymous namespace

// TODO (mr-smidge): Move stream output operators for all custom types into main library.
namespace djinterop
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const stdx::optional<T> v)
{
    return print(os, v);
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::beatgrid_marker& v)
{
    o << "beatgrid_marker{index=" << v.index
      << ", sample_offset=" << v.sample_offset << "}";
    return o;
}

inline std::ostream& operator<<(std::ostream& o, const djinterop::pad_color& v)
{
    o << "pad_color{r=" << (int)v.r << ", g=" << (int)v.g << ", b=" << (int)v.b
      << ", a=" << (int)v.a;
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
    o << "waveform_point{value=" << (int)v.value
      << ", opacity=" << (int)v.opacity;
    return o;
}

inline std::ostream& operator<<(
    std::ostream& o, const djinterop::waveform_entry& v)
{
    o << "waveform_entry{low=" << v.low << ", mid=" << v.mid
      << ", high=" << v.high << "}";
    return o;
}

namespace engine::v2
{
template <typename T>
std::ostream& operator<<(std::ostream& os, const stdx::optional<T>& v)
{
    return print(os, v);
}

template <typename Clock, typename Duration>
std::ostream& operator<<(
    std::ostream& os, const std::chrono::time_point<Clock, Duration>& v)
{
    return print(os, v);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    return print(os, v);
}

inline std::ostream& operator<<(
    std::ostream& os, const beat_grid_marker_blob& blob)
{
    os << "beat_grid_marker(sample_offset=" << blob.sample_offset
       << ", beat_number=" << blob.beat_number
       << ", number_of_beats=" << blob.number_of_beats
       << ", unknown_value_1=" << blob.unknown_value_1 << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const beat_data_blob& blob)
{
    os << "beat_data_blob(sample_rate=" << blob.sample_rate
       << ", samples=" << blob.samples
       << ", is_beatgrid_set=" << (int)blob.is_beatgrid_set
       << ", default_beat_grid=" << blob.default_beat_grid
       << ", adjusted_beat_grid=" << blob.adjusted_beat_grid << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quick_cue_blob& blob)
{
    os << "quick_cue_blob{label=" << blob.label
       << ", sample_offset=" << blob.sample_offset << ", color=" << blob.color
       << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const quick_cues_blob& blob)
{
    os << "quick_cues_blob{quick_cues=" << blob.quick_cues
       << ", adjusted_main_cue=" << blob.adjusted_main_cue
       << ", is_main_cue_adjusted=" << blob.is_main_cue_adjusted
       << ", default_main_cue=" << blob.default_main_cue << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const loop_blob& blob)
{
    os << "loop_blob{label=" << blob.label
       << ", start_sample_offset=" << blob.start_sample_offset
       << ", end_sample_offset=" << blob.end_sample_offset
       << ", is_start_set=" << (int)blob.is_start_set
       << ", is_end_set=" << (int)blob.is_end_set << ", color=" << blob.color << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const loops_blob& blob)
{
    os << "loops_blob{loops=" << blob.loops << "}";
    return os;
}

inline std::ostream& operator<<(
    std::ostream& os, const overview_waveform_data_blob& unused)
{
    os << "overview_waveform_data_blob{}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const track_data_blob& blob)
{
    os << "track_data_blob{sample_rate=" << blob.sample_rate
       << ", samples=" << blob.samples
       << ", average_loudness=" << blob.average_loudness << ", key=" << blob.key
       << "}";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const track_row& row)
{
    os << "track_row{id=" << row.id << ", play_order=" << row.play_order
       << ", length=" << row.length << ", bpm=" << row.bpm
       << ", year=" << row.year << ", path=" << row.path
       << ", filename=" << row.filename << ", bitrate=" << row.bitrate
       << ", bpm_analyzed=" << row.bpm_analyzed
       << ", album_art_id=" << row.album_art_id
       << ", file_bytes=" << row.file_bytes << ", title=" << row.title
       << ", artist=" << row.artist << ", album=" << row.album
       << ", genre=" << row.genre << ", comment=" << row.comment
       << ", label=" << row.label << ", composer=" << row.composer
       << ", remixer=" << row.remixer << ", key=" << row.key
       << ", rating=" << row.rating << ", album_art=" << row.album_art
       << ", time_last_played=" << row.time_last_played
       << ", is_played=" << row.is_played << ", file_type=" << row.file_type
       << ", is_analyzed=" << row.is_analyzed
       << ", date_created=" << row.date_created
       << ", date_added=" << row.date_added
       << ", is_available=" << row.is_available
       << ", is_metadata_of_packed_track_changed="
       << row.is_metadata_of_packed_track_changed
       << ", is_performance_data_of_packed_track_changed="
       << row.is_performance_data_of_packed_track_changed
       << ", played_indicator=" << row.played_indicator
       << ", is_metadata_imported=" << row.is_metadata_imported
       << ", pdb_import_key=" << row.pdb_import_key
       << ", streaming_source=" << row.streaming_source << ", uri=" << row.uri
       << ", is_beat_grid_locked=" << row.is_beat_grid_locked
       << ", origin_database_uuid=" << row.origin_database_uuid
       << ", origin_track_id=" << row.origin_track_id
       << ", track_data=" << row.track_data
       << ", overview_waveform_data=" << row.overview_waveform_data
       << ", beat_data=" << row.beat_data << ", quick_cues=" << row.quick_cues
       << ", loops=" << row.loops
       << ", third_party_source_id=" << row.third_party_source_id
       << ", streaming_flags=" << row.streaming_flags
       << ", explicit_lyrics=" << row.explicit_lyrics << "}";
    return os;
}
}  // namespace engine::v2
}  // namespace djinterop

/// Printable optional wrapper.
///
/// Boost does not know how to print std::optional, and we cannot add overloads
/// for operator<< as it lives in the std namespace, which is forbidden to be
/// extended.  This thin wrapper provides appropriate support, at the syntactic
/// expense of having to wrap optional types at the time of comparison.
// TODO (mr-smidge): Remove in favour of `printable`.
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


/// Printable optional wrapper.
///
/// Boost does not know how to print things such as std::optional, and we cannot
/// add overloads for operator<< as it lives in the std namespace, which is
/// forbidden to be extended.  This thin wrapper provides appropriate support,
/// at the syntactic expense of having to wrap optional types at the time of
/// comparison.
template <typename T>
struct printable
{
    T value;
};

template <typename T>
auto make_printable(const T& v)
{
    return printable<T>{v};
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const printable<T>& v)
{
    return o << v.value;
}

template <typename T>
std::ostream& operator<<(std::ostream& o, const printable<djinterop::stdx::optional<T> >& v)
{
    if (v.value)
        return o << make_printable(*v.value);
    else
        return o << "nullopt";
}

template <typename Clock, typename Duration>
std::ostream& operator<<(
    std::ostream& o,
    const printable<std::chrono::time_point<Clock, Duration> >& v)
{
    return o << v.value.time_since_epoch().count();
}

template <typename Rep, typename Period>
std::ostream& operator<<(
    std::ostream& o,
    const printable<std::chrono::duration<Rep, Period> >& v)
{
    return o << v.value.count();
}

template <class T, class U>
bool operator==(
    const printable<T>& lhs, const printable<U>& rhs)
{
    return lhs.value == rhs.value;
}

template <class T, class U>
bool operator!=(
    const printable<T>& lhs, const printable<U>& rhs)
{
    return !(lhs == rhs);
}
