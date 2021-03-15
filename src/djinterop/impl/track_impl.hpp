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

#include <djinterop/optional.hpp>
#include <djinterop/performance_data.hpp>

namespace djinterop
{
class database;
class track;
struct track_import_info;
struct track_snapshot;
enum class musical_key;

class track_impl
{
public:
    track_impl(int64_t id) noexcept;
    virtual ~track_impl() noexcept;

    int64_t id() const noexcept;

    virtual track_snapshot snapshot() const = 0;

    virtual void update(const track_snapshot& snapshot) = 0;

    virtual std::vector<beatgrid_marker> adjusted_beatgrid() = 0;
    virtual void set_adjusted_beatgrid(
        std::vector<beatgrid_marker> beatgrid) = 0;
    virtual double adjusted_main_cue() = 0;
    virtual void set_adjusted_main_cue(double sample_offset) = 0;
    virtual stdx::optional<std::string> album() = 0;
    virtual void set_album(stdx::optional<std::string> album) = 0;
    virtual stdx::optional<int64_t> album_art_id() = 0;
    virtual void set_album_art_id(stdx::optional<int64_t> album_art_id) = 0;
    virtual stdx::optional<std::string> artist() = 0;
    virtual void set_artist(stdx::optional<std::string> artist) = 0;
    virtual stdx::optional<double> average_loudness() = 0;
    virtual void set_average_loudness(
        stdx::optional<double> average_loudness) = 0;
    virtual stdx::optional<int64_t> bitrate() = 0;
    virtual void set_bitrate(stdx::optional<int64_t> bitrate) = 0;
    virtual stdx::optional<double> bpm() = 0;
    virtual void set_bpm(stdx::optional<double> bpm) = 0;
    virtual stdx::optional<std::string> comment() = 0;
    virtual void set_comment(stdx::optional<std::string> comment) = 0;
    virtual stdx::optional<std::string> composer() = 0;
    virtual void set_composer(stdx::optional<std::string> composer) = 0;
    virtual std::vector<crate> containing_crates() = 0;
    virtual database db() = 0;
    virtual std::vector<beatgrid_marker> default_beatgrid() = 0;
    virtual void set_default_beatgrid(
        std::vector<beatgrid_marker> beatgrid) = 0;
    virtual double default_main_cue() = 0;
    virtual void set_default_main_cue(double sample_offset) = 0;
    virtual stdx::optional<std::chrono::milliseconds> duration() = 0;
    virtual std::string file_extension() = 0;
    virtual std::string filename() = 0;
    virtual stdx::optional<std::string> genre() = 0;
    virtual void set_genre(stdx::optional<std::string> genre) = 0;
    virtual stdx::optional<hot_cue> hot_cue_at(int32_t index) = 0;
    virtual void set_hot_cue_at(int32_t index, stdx::optional<hot_cue> cue) = 0;
    virtual std::array<stdx::optional<hot_cue>, 8> hot_cues() = 0;
    virtual void set_hot_cues(std::array<stdx::optional<hot_cue>, 8> cues) = 0;
    virtual stdx::optional<track_import_info> import_info() = 0;
    virtual void set_import_info(
        const stdx::optional<track_import_info>& import_info) = 0;
    virtual bool is_valid() = 0;
    virtual stdx::optional<musical_key> key() = 0;
    virtual void set_key(stdx::optional<musical_key> key) = 0;
    virtual stdx::optional<std::chrono::system_clock::time_point>
    last_accessed_at() = 0;
    virtual void set_last_accessed_at(
        stdx::optional<std::chrono::system_clock::time_point>
            last_accessed_at) = 0;
    virtual stdx::optional<std::chrono::system_clock::time_point>
    last_modified_at() = 0;
    virtual void set_last_modified_at(
        stdx::optional<std::chrono::system_clock::time_point>
            last_modified_at) = 0;
    virtual stdx::optional<std::chrono::system_clock::time_point>
    last_played_at() = 0;
    virtual void set_last_played_at(
        stdx::optional<std::chrono::system_clock::time_point> time) = 0;
    virtual stdx::optional<loop> loop_at(int32_t index) = 0;
    virtual void set_loop_at(int32_t index, stdx::optional<loop> l) = 0;
    virtual std::array<stdx::optional<loop>, 8> loops() = 0;
    virtual void set_loops(std::array<stdx::optional<loop>, 8> loops) = 0;
    virtual std::vector<waveform_entry> overview_waveform() = 0;
    virtual stdx::optional<std::string> publisher() = 0;
    virtual void set_publisher(stdx::optional<std::string> publisher) = 0;
    virtual stdx::optional<int32_t> rating() = 0;
    virtual void set_rating(stdx::optional<int32_t> rating) = 0;
    virtual std::string relative_path() = 0;
    virtual void set_relative_path(std::string relative_path) = 0;
    virtual stdx::optional<sampling_info> sampling() = 0;
    virtual void set_sampling(stdx::optional<sampling_info> sample_rate) = 0;
    virtual stdx::optional<std::string> title() = 0;
    virtual void set_title(stdx::optional<std::string> title) = 0;
    virtual stdx::optional<int32_t> track_number() = 0;
    virtual void set_track_number(stdx::optional<int32_t> track_number) = 0;
    virtual std::vector<waveform_entry> waveform() = 0;
    virtual void set_waveform(std::vector<waveform_entry> waveform) = 0;
    virtual stdx::optional<int32_t> year() = 0;
    virtual void set_year(stdx::optional<int32_t> year) = 0;

private:
    int64_t id_;
};

}  // namespace djinterop
