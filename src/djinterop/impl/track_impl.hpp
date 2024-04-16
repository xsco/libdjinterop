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
#include <vector>

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

    virtual std::optional<std::string> album() = 0;
    virtual void set_album(std::optional<std::string> album) = 0;
    virtual std::optional<std::string> artist() = 0;
    virtual void set_artist(std::optional<std::string> artist) = 0;
    virtual std::optional<double> average_loudness() = 0;
    virtual void set_average_loudness(
        std::optional<double> average_loudness) = 0;
    virtual std::vector<beatgrid_marker> beatgrid() = 0;
    virtual void set_beatgrid(std::vector<beatgrid_marker> beatgrid) = 0;
    virtual std::optional<int> bitrate() = 0;
    virtual void set_bitrate(std::optional<int> bitrate) = 0;
    virtual std::optional<double> bpm() = 0;
    virtual void set_bpm(std::optional<double> bpm) = 0;
    virtual std::optional<std::string> comment() = 0;
    virtual void set_comment(std::optional<std::string> comment) = 0;
    virtual std::optional<std::string> composer() = 0;
    virtual void set_composer(std::optional<std::string> composer) = 0;
    virtual std::vector<crate> containing_crates() = 0;
    virtual database db() = 0;
    virtual std::optional<std::chrono::milliseconds> duration() = 0;
    virtual void set_duration(
        std::optional<std::chrono::milliseconds> duration) = 0;
    virtual std::string file_extension() = 0;
    virtual std::string filename() = 0;
    virtual std::optional<std::string> genre() = 0;
    virtual void set_genre(std::optional<std::string> genre) = 0;
    virtual std::optional<hot_cue> hot_cue_at(int index) = 0;
    virtual void set_hot_cue_at(int index, std::optional<hot_cue> cue) = 0;
    virtual std::vector<std::optional<hot_cue> > hot_cues() = 0;
    virtual void set_hot_cues(std::vector<std::optional<hot_cue> > cues) = 0;
    virtual bool is_valid() = 0;
    virtual std::optional<musical_key> key() = 0;
    virtual void set_key(std::optional<musical_key> key) = 0;
    virtual std::optional<std::chrono::system_clock::time_point>
    last_played_at() = 0;
    virtual void set_last_played_at(
        std::optional<std::chrono::system_clock::time_point> time) = 0;
    virtual std::optional<loop> loop_at(int index) = 0;
    virtual void set_loop_at(int index, std::optional<loop> l) = 0;
    virtual std::vector<std::optional<loop> > loops() = 0;
    virtual void set_loops(std::vector<std::optional<loop> > loops) = 0;
    virtual std::optional<double> main_cue() = 0;
    virtual void set_main_cue(std::optional<double> sample_offset) = 0;
    virtual std::optional<std::string> publisher() = 0;
    virtual void set_publisher(std::optional<std::string> publisher) = 0;
    virtual std::optional<int> rating() = 0;
    virtual void set_rating(std::optional<int> rating) = 0;
    virtual std::string relative_path() = 0;
    virtual void set_relative_path(std::string relative_path) = 0;
    virtual std::optional<unsigned long long> sample_count() = 0;
    virtual void set_sample_count(std::optional<unsigned long long> sample_count) = 0;
    virtual std::optional<double> sample_rate() = 0;
    virtual void set_sample_rate(std::optional<double> sample_rate) = 0;
    virtual std::optional<std::string> title() = 0;
    virtual void set_title(std::optional<std::string> title) = 0;
    virtual std::optional<int> track_number() = 0;
    virtual void set_track_number(std::optional<int> track_number) = 0;
    virtual std::vector<waveform_entry> waveform() = 0;
    virtual void set_waveform(std::vector<waveform_entry> waveform) = 0;
    virtual std::optional<int> year() = 0;
    virtual void set_year(std::optional<int> year) = 0;

private:
    int64_t id_;
};

}  // namespace djinterop
