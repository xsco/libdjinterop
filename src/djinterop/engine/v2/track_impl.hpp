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
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/engine/v2/track_table.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/optional.hpp>
#include <djinterop/track_snapshot.hpp>

#include "../../impl/track_impl.hpp"
#include "engine_library_context.hpp"

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

    stdx::optional<std::string> album() override;
    void set_album(stdx::optional<std::string> album) override;
    stdx::optional<std::string> artist() override;
    void set_artist(stdx::optional<std::string> artist) override;
    stdx::optional<double> average_loudness() override;
    void set_average_loudness(stdx::optional<double> average_loudness) override;
    std::vector<beatgrid_marker> beatgrid() override;
    void set_beatgrid(std::vector<beatgrid_marker> beatgrid) override;
    stdx::optional<int> bitrate() override;
    void set_bitrate(stdx::optional<int> bitrate) override;
    stdx::optional<double> bpm() override;
    void set_bpm(stdx::optional<double> bpm) override;
    stdx::optional<std::string> comment() override;
    void set_comment(stdx::optional<std::string> comment) override;
    stdx::optional<std::string> composer() override;
    void set_composer(stdx::optional<std::string> composer) override;
    std::vector<djinterop::crate> containing_crates() override;
    database db() override;
    stdx::optional<std::chrono::milliseconds> duration() override;
    void set_duration(
        stdx::optional<std::chrono::milliseconds> duration) override;
    std::string file_extension() override;
    std::string filename() override;
    stdx::optional<std::string> genre() override;
    void set_genre(stdx::optional<std::string> genre) override;
    stdx::optional<hot_cue> hot_cue_at(int index) override;
    void set_hot_cue_at(int index, stdx::optional<hot_cue> cue) override;
    std::vector<stdx::optional<hot_cue>> hot_cues() override;
    void set_hot_cues(std::vector<stdx::optional<hot_cue>> cues) override;
    bool is_valid() override;
    stdx::optional<musical_key> key() override;
    void set_key(stdx::optional<musical_key> key) override;
    stdx::optional<std::chrono::system_clock::time_point> last_played_at()
        override;
    void set_last_played_at(
        stdx::optional<std::chrono::system_clock::time_point> played_at)
        override;
    stdx::optional<loop> loop_at(int index) override;
    void set_loop_at(int index, stdx::optional<loop> l) override;
    std::vector<stdx::optional<loop>> loops() override;
    void set_loops(std::vector<stdx::optional<loop>> loops) override;
    stdx::optional<double> main_cue() override;
    void set_main_cue(stdx::optional<double> sample_offset) override;
    stdx::optional<std::string> publisher() override;
    void set_publisher(stdx::optional<std::string> publisher) override;
    stdx::optional<int32_t> rating() override;
    void set_rating(stdx::optional<int32_t> rating) override;
    std::string relative_path() override;
    void set_relative_path(std::string relative_path) override;
    stdx::optional<unsigned long long> sample_count() override;
    void set_sample_count(
        stdx::optional<unsigned long long> sample_count) override;
    stdx::optional<double> sample_rate() override;
    void set_sample_rate(stdx::optional<double> sample_rate) override;
    stdx::optional<std::string> title() override;
    void set_title(stdx::optional<std::string> title) override;
    stdx::optional<int> track_number() override;
    void set_track_number(stdx::optional<int> track_number) override;
    std::vector<waveform_entry> waveform() override;
    void set_waveform(std::vector<waveform_entry> waveform) override;
    stdx::optional<int> year() override;
    void set_year(stdx::optional<int> year) override;

private:
    std::shared_ptr<engine_library> library_;
    track_table track_;
};

track create_track(
    const std::shared_ptr<engine_library>& library,
    const track_snapshot& snapshot);

}  // namespace engine::v2
}  // namespace djinterop
