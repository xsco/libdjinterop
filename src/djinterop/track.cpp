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

#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <sqlite_modern_cpp.h>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include <djinterop/database.hpp>
#include <djinterop/impl/database_impl.hpp>
#include <djinterop/impl/track_impl.hpp>
#include <djinterop/impl/util.hpp>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace djinterop
{
track_import_info::track_import_info() noexcept = default;

track_import_info::track_import_info(
    std::string external_db_uuid, int64_t external_track_id) noexcept
    : external_db_uuid_{std::move(external_db_uuid)},
      external_track_id_{external_track_id}
{
}

std::string& track_import_info::external_db_uuid()
{
    return external_db_uuid_;
}

const std::string& track_import_info::external_db_uuid() const
{
    return external_db_uuid_;
}

int64_t& track_import_info::external_track_id()
{
    return external_track_id_;
}

const int64_t& track_import_info::external_track_id() const
{
    return external_track_id_;
}

track::track(const track& other) noexcept = default;

track::~track() = default;

track& track::operator=(const track& other) noexcept = default;

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

boost::optional<std::string> track::album() const
{
    return pimpl_->album();
}

void track::set_album(boost::optional<boost::string_view> album) const
{
    pimpl_->set_album(album);
}

void track::set_album(boost::string_view album) const
{
    set_album(boost::make_optional(album));
}

boost::optional<int64_t> track::album_art_id() const
{
    return pimpl_->album_art_id();
}

void track::set_album_art_id(boost::optional<int64_t> album_art_id) const
{
    pimpl_->set_album_art_id(album_art_id);
}

void track::set_album_art_id(int64_t album_art_id) const
{
    set_album_art_id(boost::make_optional(album_art_id));
}

boost::optional<std::string> track::artist() const
{
    return pimpl_->artist();
}

void track::set_artist(boost::optional<boost::string_view> artist) const
{
    pimpl_->set_artist(artist);
}

void track::set_artist(boost::string_view artist) const
{
    set_artist(boost::make_optional(artist));
}

boost::optional<double> track::average_loudness() const
{
    return pimpl_->average_loudness();
}

void track::set_average_loudness(boost::optional<double> average_loudness) const
{
    pimpl_->set_average_loudness(average_loudness);
}

void track::set_average_loudness(double average_loudness) const
{
    set_average_loudness(boost::make_optional(average_loudness));
}

boost::optional<int64_t> track::bitrate() const
{
    return pimpl_->bitrate();
}

void track::set_bitrate(boost::optional<int64_t> bitrate) const
{
    pimpl_->set_bitrate(bitrate);
}

void track::set_bitrate(int64_t bitrate) const
{
    set_bitrate(boost::make_optional(bitrate));
}

boost::optional<double> track::bpm() const
{
    return pimpl_->bpm();
}

void track::set_bpm(boost::optional<double> bpm) const
{
    pimpl_->set_bpm(bpm);
}

void track::set_bpm(double bpm) const
{
    set_bpm(boost::make_optional(bpm));
}

boost::optional<std::string> track::comment() const
{
    return pimpl_->comment();
}

void track::set_comment(boost::optional<boost::string_view> comment) const
{
    pimpl_->set_comment(comment);
}

void track::set_comment(boost::string_view comment) const
{
    set_comment(boost::make_optional(comment));
}

boost::optional<std::string> track::composer() const
{
    return pimpl_->composer();
}

void track::set_composer(boost::optional<boost::string_view> composer) const
{
    pimpl_->set_composer(composer);
}

void track::set_composer(boost::string_view composer) const
{
    set_composer(boost::make_optional(composer));
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

boost::optional<milliseconds> track::duration() const
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

boost::optional<std::string> track::genre() const
{
    return pimpl_->genre();
}

void track::set_genre(boost::optional<boost::string_view> genre) const
{
    pimpl_->set_genre(genre);
}

void track::set_genre(boost::string_view genre) const
{
    set_genre(boost::make_optional(genre));
}

boost::optional<hot_cue> track::hot_cue_at(int32_t index) const
{
    return pimpl_->hot_cue_at(index);
}

void track::set_hot_cue_at(int32_t index, boost::optional<hot_cue> cue) const
{
    pimpl_->set_hot_cue_at(index, std::move(cue));
}

void track::set_hot_cue_at(int32_t index, hot_cue cue) const
{
    set_hot_cue_at(index, boost::make_optional(std::move(cue)));
}

std::array<boost::optional<hot_cue>, 8> track::hot_cues() const
{
    return pimpl_->hot_cues();
}

void track::set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues) const
{
    pimpl_->set_hot_cues(std::move(cues));
}

int64_t track::id() const
{
    return pimpl_->id();
}

boost::optional<track_import_info> track::import_info() const
{
    return pimpl_->import_info();
}

void track::set_import_info(
    const boost::optional<track_import_info>& import_info) const
{
    pimpl_->set_import_info(import_info);
}

void track::set_import_info(const track_import_info& import_info) const
{
    set_import_info(boost::make_optional(import_info));
}

bool track::is_valid() const
{
    return pimpl_->is_valid();
}

boost::optional<musical_key> track::key() const
{
    return pimpl_->key();
}

void track::set_key(boost::optional<musical_key> key) const
{
    pimpl_->set_key(key);
}

void track::set_key(musical_key key) const
{
    set_key(boost::make_optional(key));
}

boost::optional<system_clock::time_point> track::last_accessed_at() const
{
    return pimpl_->last_accessed_at();
}

void track::set_last_accessed_at(
    boost::optional<system_clock::time_point> accessed_at) const
{
    pimpl_->set_last_accessed_at(accessed_at);
}

void track::set_last_accessed_at(system_clock::time_point accessed_at) const
{
    set_last_accessed_at(boost::make_optional(accessed_at));
}

boost::optional<system_clock::time_point> track::last_modified_at() const
{
    return pimpl_->last_modified_at();
}

void track::set_last_modified_at(
    boost::optional<system_clock::time_point> modified_at) const
{
    pimpl_->set_last_modified_at(modified_at);
}

void track::set_last_modified_at(system_clock::time_point modified_at) const
{
    set_last_modified_at(boost::make_optional(modified_at));
}

boost::optional<system_clock::time_point> track::last_played_at() const
{
    return pimpl_->last_played_at();
}

void track::set_last_played_at(
    boost::optional<system_clock::time_point> played_at) const
{
    pimpl_->set_last_played_at(played_at);
}

void track::set_last_played_at(system_clock::time_point played_at) const
{
    set_last_played_at(boost::make_optional(played_at));
}

boost::optional<loop> track::loop_at(int32_t index) const
{
    return pimpl_->loop_at(index);
}

void track::set_loop_at(int32_t index, boost::optional<loop> l) const
{
    pimpl_->set_loop_at(index, std::move(l));
}

void track::set_loop_at(int32_t index, loop l) const
{
    set_loop_at(index, boost::make_optional(l));
}

std::array<boost::optional<loop>, 8> track::loops() const
{
    return pimpl_->loops();
}

void track::set_loops(std::array<boost::optional<loop>, 8> loops) const
{
    pimpl_->set_loops(std::move(loops));
}

std::vector<waveform_entry> track::overview_waveform() const
{
    return pimpl_->overview_waveform();
}

boost::optional<std::string> track::publisher() const
{
    return pimpl_->publisher();
}

void track::set_publisher(boost::optional<boost::string_view> publisher) const
{
    pimpl_->set_publisher(publisher);
}

void track::set_publisher(boost::string_view publisher) const
{
    set_publisher(boost::make_optional(publisher));
}

int64_t track::recommended_waveform_size() const
{
    return pimpl_->recommended_waveform_size();
}

std::string track::relative_path() const
{
    return pimpl_->relative_path();
}

void track::set_relative_path(boost::string_view relative_path) const
{
    pimpl_->set_relative_path(relative_path);
}

boost::optional<sampling_info> track::sampling() const
{
    return pimpl_->sampling();
}

void track::set_sampling(boost::optional<sampling_info> sampling) const
{
    pimpl_->set_sampling(std::move(sampling));
}

void track::set_sampling(sampling_info sampling) const
{
    set_sampling(boost::make_optional(sampling));
}

boost::optional<std::string> track::title() const
{
    return pimpl_->title();
}

void track::set_title(boost::optional<boost::string_view> title) const
{
    pimpl_->set_title(std::move(title));
}

void track::set_title(boost::string_view title) const
{
    set_title(boost::make_optional(title));
}

boost::optional<int32_t> track::track_number() const
{
    return pimpl_->track_number();
}

void track::set_track_number(boost::optional<int32_t> track_number) const
{
    return pimpl_->set_track_number(track_number);
}

void track::set_track_number(int32_t track_number) const
{
    set_track_number(boost::make_optional(track_number));
}

std::vector<waveform_entry> track::waveform() const
{
    return pimpl_->waveform();
}

void track::set_waveform(std::vector<waveform_entry> waveform) const
{
    pimpl_->set_waveform(waveform);
}

boost::optional<int32_t> track::year() const
{
    return pimpl_->year();
}

void track::set_year(boost::optional<int32_t> year) const
{
    pimpl_->set_year(year);
}

void track::set_year(int32_t year) const
{
    set_year(boost::make_optional(year));
}

track::track(std::shared_ptr<track_impl> pimpl) : pimpl_{std::move(pimpl)} {}

}  // namespace djinterop
