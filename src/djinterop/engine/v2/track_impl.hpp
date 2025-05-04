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
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/track_snapshot.hpp>

#include "../../impl/track_impl.hpp"

namespace djinterop
{
class crate;
class database;
enum class musical_key;

namespace engine::v2
{
class track_impl : public djinterop::track_impl
{
public:
    track_impl(std::shared_ptr<engine_library> library, int64_t id);

    track_snapshot snapshot() const override;

    void update(const track_snapshot& snapshot) override;

    std::optional<std::string> album() override;
    void set_album(std::optional<std::string> album) override;
    std::optional<std::string> artist() override;
    void set_artist(std::optional<std::string> artist) override;
    std::optional<double> average_loudness() override;
    void set_average_loudness(std::optional<double> average_loudness) override;
    std::vector<beatgrid_marker> beatgrid() override;
    void set_beatgrid(std::vector<beatgrid_marker> beatgrid) override;
    std::optional<int> bitrate() override;
    void set_bitrate(std::optional<int> bitrate) override;
    std::optional<double> bpm() override;
    void set_bpm(std::optional<double> bpm) override;
    std::optional<std::string> comment() override;
    void set_comment(std::optional<std::string> comment) override;
    std::optional<std::string> composer() override;
    void set_composer(std::optional<std::string> composer) override;
    std::vector<djinterop::crate> containing_crates() override;
    database db() override;
    std::optional<std::chrono::milliseconds> duration() override;
    void set_duration(
        std::optional<std::chrono::milliseconds> duration) override;
    std::string file_extension() override;
    std::string filename() override;
    std::optional<std::string> genre() override;
    void set_genre(std::optional<std::string> genre) override;
    std::optional<hot_cue> hot_cue_at(int index) override;
    void set_hot_cue_at(int index, std::optional<hot_cue> cue) override;
    std::vector<std::optional<hot_cue>> hot_cues() override;
    void set_hot_cues(std::vector<std::optional<hot_cue>> cues) override;
    bool is_valid() override;
    std::optional<musical_key> key() override;
    void set_key(std::optional<musical_key> key) override;
    std::optional<std::chrono::system_clock::time_point> last_played_at()
        override;
    void set_last_played_at(
        std::optional<std::chrono::system_clock::time_point> played_at)
        override;
    std::optional<loop> loop_at(int index) override;
    void set_loop_at(int index, std::optional<loop> l) override;
    std::vector<std::optional<loop>> loops() override;
    void set_loops(std::vector<std::optional<loop>> loops) override;
    std::optional<double> main_cue() override;
    void set_main_cue(std::optional<double> sample_offset) override;
    std::optional<std::string> publisher() override;
    void set_publisher(std::optional<std::string> publisher) override;
    std::optional<int32_t> rating() override;
    void set_rating(std::optional<int32_t> rating) override;
    std::string relative_path() override;
    void set_relative_path(std::string relative_path) override;
    std::optional<unsigned long long> sample_count() override;
    void set_sample_count(
        std::optional<unsigned long long> sample_count) override;
    std::optional<double> sample_rate() override;
    void set_sample_rate(std::optional<double> sample_rate) override;
    std::optional<std::string> title() override;
    void set_title(std::optional<std::string> title) override;
    std::optional<int> track_number() override;
    void set_track_number(std::optional<int> track_number) override;
    std::vector<waveform_entry> waveform() override;
    void set_waveform(std::vector<waveform_entry> waveform) override;
    std::optional<int> year() override;
    void set_year(std::optional<int> year) override;

private:
    std::shared_ptr<engine_library> library_;
};

track create_track(
    const std::shared_ptr<engine_library>& library,
    const track_snapshot& snapshot);

}  // namespace engine::v2
}  // namespace djinterop
