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
#include <djinterop/impl/database_impl.hpp>
#include <djinterop/impl/track_impl.hpp>
#include <djinterop/track.hpp>
#include <djinterop/track_snapshot.hpp>

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

std::vector<beatgrid_marker> track::adjusted_beatgrid() const
{
    return pimpl_->adjusted_beatgrid();
}

void track::set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid) const
{
    pimpl_->set_adjusted_beatgrid(std::move(beatgrid));
}

double track::adjusted_main_cue() const
{
    return pimpl_->adjusted_main_cue();
}

void track::set_adjusted_main_cue(double sample_offset) const
{
    pimpl_->set_adjusted_main_cue(sample_offset);
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

stdx::optional<int64_t> track::album_art_id() const
{
    return pimpl_->album_art_id();
}

void track::set_album_art_id(stdx::optional<int64_t> album_art_id) const
{
    pimpl_->set_album_art_id(album_art_id);
}

void track::set_album_art_id(int64_t album_art_id) const
{
    set_album_art_id(stdx::make_optional(album_art_id));
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

stdx::optional<int64_t> track::bitrate() const
{
    return pimpl_->bitrate();
}

void track::set_bitrate(stdx::optional<int64_t> bitrate) const
{
    pimpl_->set_bitrate(bitrate);
}

void track::set_bitrate(int64_t bitrate) const
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

std::vector<beatgrid_marker> track::default_beatgrid() const
{
    return pimpl_->default_beatgrid();
}

void track::set_default_beatgrid(std::vector<beatgrid_marker> beatgrid) const
{
    pimpl_->set_default_beatgrid(std::move(beatgrid));
}

double track::default_main_cue() const
{
    return pimpl_->default_main_cue();
}

void track::set_default_main_cue(double sample_offset) const
{
    pimpl_->set_default_main_cue(sample_offset);
}

stdx::optional<milliseconds> track::duration() const
{
    return pimpl_->duration();
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

stdx::optional<hot_cue> track::hot_cue_at(int32_t index) const
{
    return pimpl_->hot_cue_at(index);
}

void track::set_hot_cue_at(int32_t index, stdx::optional<hot_cue> cue) const
{
    pimpl_->set_hot_cue_at(index, cue);
}

void track::set_hot_cue_at(int32_t index, hot_cue cue) const
{
    set_hot_cue_at(index, stdx::make_optional(std::move(cue)));
}

std::array<stdx::optional<hot_cue>, 8> track::hot_cues() const
{
    return pimpl_->hot_cues();
}

void track::set_hot_cues(std::array<stdx::optional<hot_cue>, 8> cues) const
{
    pimpl_->set_hot_cues(std::move(cues));
}

int64_t track::id() const
{
    return pimpl_->id();
}

stdx::optional<track_import_info> track::import_info() const
{
    return pimpl_->import_info();
}

void track::set_import_info(
    const stdx::optional<track_import_info>& import_info) const
{
    pimpl_->set_import_info(import_info);
}

void track::set_import_info(const track_import_info& import_info) const
{
    set_import_info(stdx::make_optional(import_info));
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

stdx::optional<system_clock::time_point> track::last_accessed_at() const
{
    return pimpl_->last_accessed_at();
}

void track::set_last_accessed_at(
    stdx::optional<system_clock::time_point> accessed_at) const
{
    pimpl_->set_last_accessed_at(accessed_at);
}

void track::set_last_accessed_at(system_clock::time_point accessed_at) const
{
    set_last_accessed_at(stdx::make_optional(accessed_at));
}

stdx::optional<system_clock::time_point> track::last_modified_at() const
{
    return pimpl_->last_modified_at();
}

void track::set_last_modified_at(
    stdx::optional<system_clock::time_point> modified_at) const
{
    pimpl_->set_last_modified_at(modified_at);
}

void track::set_last_modified_at(system_clock::time_point modified_at) const
{
    set_last_modified_at(stdx::make_optional(modified_at));
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

stdx::optional<loop> track::loop_at(int32_t index) const
{
    return pimpl_->loop_at(index);
}

void track::set_loop_at(int32_t index, stdx::optional<loop> l) const
{
    pimpl_->set_loop_at(index, l);
}

void track::set_loop_at(int32_t index, loop l) const
{
    set_loop_at(index, stdx::make_optional(l));
}

std::array<stdx::optional<loop>, 8> track::loops() const
{
    return pimpl_->loops();
}

void track::set_loops(std::array<stdx::optional<loop>, 8> loops) const
{
    pimpl_->set_loops(std::move(loops));
}

std::vector<waveform_entry> track::overview_waveform() const
{
    return pimpl_->overview_waveform();
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

stdx::optional<int32_t> track::rating() const
{
    return pimpl_->rating();
}

void track::set_rating(stdx::optional<int32_t> rating)
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

stdx::optional<sampling_info> track::sampling() const
{
    return pimpl_->sampling();
}

void track::set_sampling(stdx::optional<sampling_info> sampling) const
{
    pimpl_->set_sampling(sampling);
}

void track::set_sampling(sampling_info sampling) const
{
    set_sampling(stdx::make_optional(sampling));
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

stdx::optional<int32_t> track::track_number() const
{
    return pimpl_->track_number();
}

void track::set_track_number(stdx::optional<int32_t> track_number) const
{
    pimpl_->set_track_number(track_number);
}

void track::set_track_number(int32_t track_number) const
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

stdx::optional<int32_t> track::year() const
{
    return pimpl_->year();
}

void track::set_year(stdx::optional<int32_t> year) const
{
    pimpl_->set_year(year);
}

void track::set_year(int32_t year) const
{
    set_year(stdx::make_optional(year));
}

track::track(std::shared_ptr<track_impl> pimpl) : pimpl_{std::move(pimpl)} {}

}  // namespace djinterop
