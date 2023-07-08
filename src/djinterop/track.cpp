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

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>

#include <djinterop/crate.hpp>
#include <djinterop/database.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

#include "impl/database_impl.hpp"
#include "impl/track_impl.hpp"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace djinterop
{
track::track(const track& other) noexcept = default;

track::~track() = default;

track& track::operator=(const track& other) noexcept = default;

track_snapshot track::snapshot() const
{
    return pimpl_->snapshot();
}

void track::update(const track_snapshot& snapshot)
{
    pimpl_->update(snapshot);
}

stdx::optional<std::string> track::album() const
{
    return pimpl_->album();
}

void track::set_album(stdx::optional<std::string> album) const
{
    pimpl_->set_album(album);
}

void track::set_album(std::string album) const
{
    set_album(stdx::make_optional(album));
}

stdx::optional<std::string> track::artist() const
{
    return pimpl_->artist();
}

void track::set_artist(stdx::optional<std::string> artist) const
{
    pimpl_->set_artist(artist);
}

void track::set_artist(std::string artist) const
{
    set_artist(stdx::make_optional(artist));
}

stdx::optional<double> track::average_loudness() const
{
    return pimpl_->average_loudness();
}

void track::set_average_loudness(stdx::optional<double> average_loudness) const
{
    pimpl_->set_average_loudness(average_loudness);
}

void track::set_average_loudness(double average_loudness) const
{
    set_average_loudness(stdx::make_optional(average_loudness));
}

std::vector<beatgrid_marker> track::beatgrid() const
{
    return pimpl_->beatgrid();
}

void track::set_beatgrid(std::vector<beatgrid_marker> beatgrid) const
{
    pimpl_->set_beatgrid(std::move(beatgrid));
}

stdx::optional<int> track::bitrate() const
{
    return pimpl_->bitrate();
}

void track::set_bitrate(stdx::optional<int> bitrate) const
{
    pimpl_->set_bitrate(bitrate);
}

void track::set_bitrate(int bitrate) const
{
    set_bitrate(stdx::make_optional(bitrate));
}

stdx::optional<double> track::bpm() const
{
    return pimpl_->bpm();
}

void track::set_bpm(stdx::optional<double> bpm) const
{
    pimpl_->set_bpm(bpm);
}

void track::set_bpm(double bpm) const
{
    set_bpm(stdx::make_optional(bpm));
}

stdx::optional<std::string> track::comment() const
{
    return pimpl_->comment();
}

void track::set_comment(stdx::optional<std::string> comment) const
{
    pimpl_->set_comment(comment);
}

void track::set_comment(std::string comment) const
{
    set_comment(stdx::make_optional(comment));
}

stdx::optional<std::string> track::composer() const
{
    return pimpl_->composer();
}

void track::set_composer(stdx::optional<std::string> composer) const
{
    pimpl_->set_composer(composer);
}

void track::set_composer(std::string composer) const
{
    set_composer(stdx::make_optional(composer));
}

std::vector<crate> track::containing_crates() const
{
    return pimpl_->containing_crates();
}

database track::db() const
{
    return pimpl_->db();
}

stdx::optional<milliseconds> track::duration() const
{
    return pimpl_->duration();
}

void track::set_duration(stdx::optional<milliseconds> duration)
{
    pimpl_->set_duration(duration);
}

void track::set_duration(milliseconds duration)
{
    set_duration(stdx::make_optional(duration));
}

std::string track::file_extension() const
{
    return pimpl_->file_extension();
}

std::string track::filename() const
{
    return pimpl_->filename();
}

stdx::optional<std::string> track::genre() const
{
    return pimpl_->genre();
}

void track::set_genre(stdx::optional<std::string> genre) const
{
    pimpl_->set_genre(genre);
}

void track::set_genre(std::string genre) const
{
    set_genre(stdx::make_optional(genre));
}

stdx::optional<hot_cue> track::hot_cue_at(int index) const
{
    return pimpl_->hot_cue_at(index);
}

void track::set_hot_cue_at(int index, stdx::optional<hot_cue> cue) const
{
    pimpl_->set_hot_cue_at(index, cue);
}

void track::set_hot_cue_at(int index, hot_cue cue) const
{
    set_hot_cue_at(index, stdx::make_optional(std::move(cue)));
}

std::vector<stdx::optional<hot_cue> > track::hot_cues() const
{
    return pimpl_->hot_cues();
}

void track::set_hot_cues(std::vector<stdx::optional<hot_cue> > cues) const
{
    pimpl_->set_hot_cues(std::move(cues));
}

int64_t track::id() const
{
    return pimpl_->id();
}

bool track::is_valid() const
{
    return pimpl_->is_valid();
}

stdx::optional<musical_key> track::key() const
{
    return pimpl_->key();
}

void track::set_key(stdx::optional<musical_key> key) const
{
    pimpl_->set_key(key);
}

void track::set_key(musical_key key) const
{
    set_key(stdx::make_optional(key));
}

stdx::optional<system_clock::time_point> track::last_played_at() const
{
    return pimpl_->last_played_at();
}

void track::set_last_played_at(
    stdx::optional<system_clock::time_point> played_at) const
{
    pimpl_->set_last_played_at(played_at);
}

void track::set_last_played_at(system_clock::time_point played_at) const
{
    set_last_played_at(stdx::make_optional(played_at));
}

stdx::optional<loop> track::loop_at(int index) const
{
    return pimpl_->loop_at(index);
}

void track::set_loop_at(int index, stdx::optional<loop> l) const
{
    pimpl_->set_loop_at(index, l);
}

void track::set_loop_at(int index, loop l) const
{
    set_loop_at(index, stdx::make_optional(l));
}

std::vector<stdx::optional<loop> > track::loops() const
{
    return pimpl_->loops();
}

void track::set_loops(std::vector<stdx::optional<loop> > loops) const
{
    pimpl_->set_loops(std::move(loops));
}

stdx::optional<double> track::main_cue() const
{
    return pimpl_->main_cue();
}

void track::set_main_cue(stdx::optional<double> sample_offset) const
{
    pimpl_->set_main_cue(sample_offset);
}

stdx::optional<std::string> track::publisher() const
{
    return pimpl_->publisher();
}

void track::set_publisher(stdx::optional<std::string> publisher) const
{
    pimpl_->set_publisher(publisher);
}

void track::set_publisher(std::string publisher) const
{
    set_publisher(stdx::make_optional(publisher));
}

stdx::optional<int> track::rating() const
{
    return pimpl_->rating();
}

void track::set_rating(stdx::optional<int> rating)
{
    pimpl_->set_rating(rating);
}

void track::set_rating(int32_t rating)
{
    pimpl_->set_rating(stdx::make_optional(rating));
}

std::string track::relative_path() const
{
    return pimpl_->relative_path();
}

void track::set_relative_path(std::string relative_path) const
{
    pimpl_->set_relative_path(relative_path);
}

stdx::optional<unsigned long long> track::sample_count() const
{
    return pimpl_->sample_count();
}

void track::set_sample_count(stdx::optional<unsigned long long> sample_count)
{
    pimpl_->set_sample_count(sample_count);
}

void track::set_sample_count(unsigned long long sample_count)
{
    set_sample_count(stdx::make_optional(sample_count));
}

stdx::optional<double> track::sample_rate() const
{
    return pimpl_->sample_rate();
}

void track::set_sample_rate(stdx::optional<double> sample_rate)
{
    pimpl_->set_sample_rate(sample_rate);
}

void track::set_sample_rate(double sample_rate)
{
    set_sample_rate(stdx::make_optional(sample_rate));
}

stdx::optional<std::string> track::title() const
{
    return pimpl_->title();
}

void track::set_title(stdx::optional<std::string> title) const
{
    pimpl_->set_title(title);
}

void track::set_title(std::string title) const
{
    set_title(stdx::make_optional(title));
}

stdx::optional<int> track::track_number() const
{
    return pimpl_->track_number();
}

void track::set_track_number(stdx::optional<int> track_number) const
{
    pimpl_->set_track_number(track_number);
}

void track::set_track_number(int track_number) const
{
    set_track_number(stdx::make_optional(track_number));
}

std::vector<waveform_entry> track::waveform() const
{
    return pimpl_->waveform();
}

void track::set_waveform(std::vector<waveform_entry> waveform) const
{
    pimpl_->set_waveform(waveform);
}

stdx::optional<int> track::year() const
{
    return pimpl_->year();
}

void track::set_year(stdx::optional<int> year) const
{
    pimpl_->set_year(year);
}

void track::set_year(int year) const
{
    set_year(stdx::make_optional(year));
}

track::track(std::shared_ptr<track_impl> pimpl) : pimpl_{std::move(pimpl)} {}

}  // namespace djinterop
