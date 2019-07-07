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
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>

#include <djinterop/performance_data.hpp>

namespace djinterop
{
class database;
class track;
class track_import_info;
enum class musical_key;

class track_impl
{
public:
    track_impl(int64_t id) noexcept;
    virtual ~track_impl() noexcept;

    int64_t id() noexcept;

    virtual std::vector<beatgrid_marker> adjusted_beatgrid() = 0;
    virtual void set_adjusted_beatgrid(
        std::vector<beatgrid_marker> beatgrid) = 0;
    virtual double adjusted_main_cue() = 0;
    virtual void set_adjusted_main_cue(double sample_offset) = 0;
    virtual boost::optional<std::string> album() = 0;
    virtual void set_album(boost::optional<boost::string_view> album) = 0;
    virtual boost::optional<int64_t> album_art_id() = 0;
    virtual void set_album_art_id(boost::optional<int64_t> album_art_id) = 0;
    virtual boost::optional<std::string> artist() = 0;
    virtual void set_artist(boost::optional<boost::string_view> artist) = 0;
    virtual boost::optional<double> average_loudness() = 0;
    virtual void set_average_loudness(
        boost::optional<double> average_loudness) = 0;
    virtual boost::optional<int64_t> bitrate() = 0;
    virtual void set_bitrate(boost::optional<int64_t> bitrate) = 0;
    virtual boost::optional<double> bpm() = 0;
    virtual void set_bpm(boost::optional<double> bpm) = 0;
    virtual boost::optional<std::string> comment() = 0;
    virtual void set_comment(boost::optional<boost::string_view> comment) = 0;
    virtual boost::optional<std::string> composer() = 0;
    virtual void set_composer(boost::optional<boost::string_view> composer) = 0;
    virtual std::vector<crate> containing_crates() = 0;
    virtual database db() = 0;
    virtual std::vector<beatgrid_marker> default_beatgrid() = 0;
    virtual void set_default_beatgrid(
        std::vector<beatgrid_marker> beatgrid) = 0;
    virtual double default_main_cue() = 0;
    virtual void set_default_main_cue(double sample_offset) = 0;
    virtual boost::optional<std::chrono::milliseconds> duration() = 0;
    virtual std::string file_extension() = 0;
    virtual std::string filename() = 0;
    virtual boost::optional<std::string> genre() = 0;
    virtual void set_genre(boost::optional<boost::string_view> genre) = 0;
    virtual boost::optional<hot_cue> hot_cue_at(int32_t index) = 0;
    virtual void set_hot_cue_at(
        int32_t index, boost::optional<hot_cue> cue) = 0;
    virtual std::array<boost::optional<hot_cue>, 8> hot_cues() = 0;
    virtual void set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues) = 0;
    virtual boost::optional<track_import_info> import_info() = 0;
    virtual void set_import_info(
        const boost::optional<track_import_info>& import_info) = 0;
    virtual bool is_valid() = 0;
    virtual boost::optional<musical_key> key() = 0;
    virtual void set_key(boost::optional<musical_key> key) = 0;
    virtual boost::optional<std::chrono::system_clock::time_point>
    last_accessed_at() = 0;
    virtual void set_last_accessed_at(
        boost::optional<std::chrono::system_clock::time_point>
            last_accessed_at) = 0;
    virtual boost::optional<std::chrono::system_clock::time_point>
    last_modified_at() = 0;
    virtual void set_last_modified_at(
        boost::optional<std::chrono::system_clock::time_point>
            last_modified_at) = 0;
    virtual boost::optional<std::chrono::system_clock::time_point>
    last_played_at() = 0;
    virtual void set_last_played_at(
        boost::optional<std::chrono::system_clock::time_point> time) = 0;
    virtual boost::optional<loop> loop_at(int32_t index) = 0;
    virtual void set_loop_at(int32_t index, boost::optional<loop> l) = 0;
    virtual std::array<boost::optional<loop>, 8> loops() = 0;
    virtual void set_loops(std::array<boost::optional<loop>, 8> loops) = 0;
    virtual std::vector<waveform_entry> overview_waveform() = 0;
    virtual boost::optional<std::string> publisher() = 0;
    virtual void set_publisher(
        boost::optional<boost::string_view> publisher) = 0;
    virtual int64_t recommended_waveform_size() = 0;
    virtual std::string relative_path() = 0;
    virtual void set_relative_path(boost::string_view relative_path) = 0;
    virtual boost::optional<sampling_info> sampling() = 0;
    virtual void set_sampling(boost::optional<sampling_info> sample_rate) = 0;
    virtual boost::optional<std::string> title() = 0;
    virtual void set_title(boost::optional<boost::string_view> title) = 0;
    virtual boost::optional<int32_t> track_number() = 0;
    virtual void set_track_number(boost::optional<int32_t> track_number) = 0;
    virtual std::vector<waveform_entry> waveform() = 0;
    virtual void set_waveform(std::vector<waveform_entry> waveform) = 0;
    virtual boost::optional<int32_t> year() = 0;
    virtual void set_year(boost::optional<int32_t> year) = 0;

private:
    int64_t id_;
};

}  // namespace djinterop
