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
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <djinterop/track.hpp>

#include "../../util.hpp"
#include "engine_database_impl.hpp"
#include "engine_track_impl.hpp"

namespace djinterop::engine::v2
{
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

engine_track_impl::engine_track_impl(
    std::shared_ptr<engine_storage> storage, int64_t id) :
    track_impl{id},
    storage_{std::move(storage)}
{
}

track_snapshot engine_track_impl::snapshot() const
{
    track_snapshot snapshot{id()};

    //TODO
    throw std::runtime_error{"snapshot() - Not yet implemented"};

    return snapshot;
}

void engine_track_impl::update(const track_snapshot& snapshot)
{
    if (snapshot.id && *snapshot.id != id())
    {
        throw invalid_track_snapshot{
            "Snapshot pertains to a different track, and so it cannot be used "
            "to update this track"};
    }
    else if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required on any track"};
    }

    //TODO
    throw std::runtime_error{"update() - Not yet implemented"};
}

std::vector<beatgrid_marker> engine_track_impl::adjusted_beatgrid()
{
    //TODO
    throw std::runtime_error{"adjusted_beatgrid() - Not yet implemented"};
}

void engine_track_impl::set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    //TODO
    throw std::runtime_error{"set_adjusted_beatgrid() - Not yet implemented"};
}

double engine_track_impl::adjusted_main_cue()
{
    //TODO
    throw std::runtime_error{"adjusted_main_cue() - Not yet implemented"};
}

void engine_track_impl::set_adjusted_main_cue(double sample_offset)
{
    //TODO
    throw std::runtime_error{"set_adjusted_main_cue() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::album()
{
    //TODO
    throw std::runtime_error{"album() - Not yet implemented"};
}

void engine_track_impl::set_album(stdx::optional<std::string> album)
{
    //TODO
    throw std::runtime_error{"set_album() - Not yet implemented"};
}

stdx::optional<int64_t> engine_track_impl::album_art_id()
{
    //TODO
    throw std::runtime_error{"album_art_id() - Not yet implemented"};
}

void engine_track_impl::set_album_art_id(stdx::optional<int64_t> album_art_id)
{
    //TODO
    throw std::runtime_error{"set_album_art_id() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::artist()
{
    //TODO
    throw std::runtime_error{"artist() - Not yet implemented"};
}

void engine_track_impl::set_artist(stdx::optional<std::string> artist)
{
    //TODO
    throw std::runtime_error{"set_artist() - Not yet implemented"};
}

stdx::optional<double> engine_track_impl::average_loudness()
{
    //TODO
    throw std::runtime_error{"average_loudness() - Not yet implemented"};
}

void engine_track_impl::set_average_loudness(
    stdx::optional<double> average_loudness)
{
    //TODO
    throw std::runtime_error{"set_average_loudness() - Not yet implemented"};
}

stdx::optional<int64_t> engine_track_impl::bitrate()
{
    //TODO
    throw std::runtime_error{"bitrate() - Not yet implemented"};
}

void engine_track_impl::set_bitrate(stdx::optional<int64_t> bitrate)
{
    //TODO
    throw std::runtime_error{"set_bitrate() - Not yet implemented"};
}

stdx::optional<double> engine_track_impl::bpm()
{
    //TODO
    throw std::runtime_error{"bpm() - Not yet implemented"};
}

void engine_track_impl::set_bpm(stdx::optional<double> bpm)
{
    //TODO
    throw std::runtime_error{"set_bpm() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::comment()
{
    //TODO
    throw std::runtime_error{"comment() - Not yet implemented"};
}

void engine_track_impl::set_comment(stdx::optional<std::string> comment)
{
    //TODO
    throw std::runtime_error{"set_comment() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::composer()
{
    //TODO
    throw std::runtime_error{"composer() - Not yet implemented"};
}

void engine_track_impl::set_composer(stdx::optional<std::string> composer)
{
    //TODO
    throw std::runtime_error{"set_composer() - Not yet implemented"};
}

database engine_track_impl::db()
{
    return database{std::make_shared<engine_database_impl>(storage_)};
}

std::vector<crate> engine_track_impl::containing_crates()
{
    //TODO
    throw std::runtime_error{"containing_crates() - Not yet implemented"};
}

std::vector<beatgrid_marker> engine_track_impl::default_beatgrid()
{
    //TODO
    throw std::runtime_error{"default_beatgrid() - Not yet implemented"};
}

void engine_track_impl::set_default_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    //TODO
    throw std::runtime_error{"set_default_beatgrid() - Not yet implemented"};
}

double engine_track_impl::default_main_cue()
{
    //TODO
    throw std::runtime_error{"default_main_cue() - Not yet implemented"};
}

void engine_track_impl::set_default_main_cue(double sample_offset)
{
    //TODO
    throw std::runtime_error{"set_default_main_cue() - Not yet implemented"};
}

stdx::optional<milliseconds> engine_track_impl::duration()
{
    //TODO
    throw std::runtime_error{"duration() - Not yet implemented"};
}

std::string engine_track_impl::file_extension()
{
    auto rel_path = relative_path();
    return get_file_extension(rel_path).value_or(std::string{});
}

std::string engine_track_impl::filename()
{
    auto rel_path = relative_path();
    return get_filename(rel_path);
}

stdx::optional<std::string> engine_track_impl::genre()
{
    //TODO
    throw std::runtime_error{"genre() - Not yet implemented"};
}

void engine_track_impl::set_genre(stdx::optional<std::string> genre)
{
    //TODO
    throw std::runtime_error{"set_genre() - Not yet implemented"};
}

stdx::optional<hot_cue> engine_track_impl::hot_cue_at(int32_t index)
{
    //TODO
    throw std::runtime_error{"hot_cue_at() - Not yet implemented"};
}

void engine_track_impl::set_hot_cue_at(int32_t index, stdx::optional<hot_cue> cue)
{
    //TODO
    throw std::runtime_error{"set_hot_cue_at() - Not yet implemented"};
}

std::array<stdx::optional<hot_cue>, 8> engine_track_impl::hot_cues()
{
    //TODO
    throw std::runtime_error{"hot_cues() - Not yet implemented"};
}

void engine_track_impl::set_hot_cues(std::array<stdx::optional<hot_cue>, 8> cues)
{
    //TODO
    throw std::runtime_error{"set_hot_cues() - Not yet implemented"};
}

stdx::optional<track_import_info> engine_track_impl::import_info()
{
    //TODO
    throw std::runtime_error{"import_info() - Not yet implemented"};
}

void engine_track_impl::set_import_info(
    const stdx::optional<track_import_info>& import_info)
{
    //TODO
    throw std::runtime_error{"set_import_info() - Not yet implemented"};
}

bool engine_track_impl::is_valid()
{
    //TODO
    throw std::runtime_error{"is_valid() - Not yet implemented"};
}

stdx::optional<musical_key> engine_track_impl::key()
{
    //TODO
    throw std::runtime_error{"key() - Not yet implemented"};
}

void engine_track_impl::set_key(stdx::optional<musical_key> key)
{
    //TODO
    throw std::runtime_error{"set_key() - Not yet implemented"};
}

stdx::optional<system_clock::time_point> engine_track_impl::last_accessed_at()
{
    //TODO
    throw std::runtime_error{"last_accessed_at() - Not yet implemented"};
}

void engine_track_impl::set_last_accessed_at(
    stdx::optional<system_clock::time_point> accessed_at)
{
    //TODO
    throw std::runtime_error{"set_last_accessed_at() - Not yet implemented"};
}

stdx::optional<system_clock::time_point> engine_track_impl::last_modified_at()
{
    //TODO
    throw std::runtime_error{"last_modified_at() - Not yet implemented"};
}

void engine_track_impl::set_last_modified_at(
    stdx::optional<system_clock::time_point> modified_at)
{
    //TODO
    throw std::runtime_error{"set_last_modified_at() - Not yet implemented"};
}

stdx::optional<system_clock::time_point> engine_track_impl::last_played_at()
{
    //TODO
    throw std::runtime_error{"last_played_at() - Not yet implemented"};
}

void engine_track_impl::set_last_played_at(
    stdx::optional<system_clock::time_point> played_at)
{
    //TODO
    throw std::runtime_error{"set_last_played_at() - Not yet implemented"};
}

stdx::optional<loop> engine_track_impl::loop_at(int32_t index)
{
    //TODO
    throw std::runtime_error{"loop_at() - Not yet implemented"};
}

void engine_track_impl::set_loop_at(int32_t index, stdx::optional<loop> l)
{
    //TODO
    throw std::runtime_error{"set_loop_at() - Not yet implemented"};
}

std::array<stdx::optional<loop>, 8> engine_track_impl::loops()
{
    //TODO
    throw std::runtime_error{"loops() - Not yet implemented"};
}

void engine_track_impl::set_loops(std::array<stdx::optional<loop>, 8> cues)
{
    //TODO
    throw std::runtime_error{"set_loops() - Not yet implemented"};
}

std::vector<waveform_entry> engine_track_impl::overview_waveform()
{
    //TODO
    throw std::runtime_error{"overview_waveform() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::publisher()
{
    //TODO
    throw std::runtime_error{"publisher() - Not yet implemented"};
}

void engine_track_impl::set_publisher(stdx::optional<std::string> publisher)
{
    //TODO
    throw std::runtime_error{"set_publisher() - Not yet implemented"};
}

stdx::optional<int32_t> engine_track_impl::rating()
{
    //TODO
    throw std::runtime_error{"rating() - Not yet implemented"};
}

void engine_track_impl::set_rating(stdx::optional<int32_t> rating)
{
    //TODO
    throw std::runtime_error{"set_rating() - Not yet implemented"};
}

std::string engine_track_impl::relative_path()
{
    //TODO
    throw std::runtime_error{"relative_path() - Not yet implemented"};
}

void engine_track_impl::set_relative_path(std::string relative_path)
{
    //TODO
    throw std::runtime_error{"set_relative_path() - Not yet implemented"};
}

stdx::optional<sampling_info> engine_track_impl::sampling()
{
    //TODO
    throw std::runtime_error{"sampling() - Not yet implemented"};
}

void engine_track_impl::set_sampling(stdx::optional<sampling_info> sampling)
{
    //TODO
    throw std::runtime_error{"set_sampling() - Not yet implemented"};
}

stdx::optional<std::string> engine_track_impl::title()
{
    //TODO
    throw std::runtime_error{"title() - Not yet implemented"};
}

void engine_track_impl::set_title(stdx::optional<std::string> title)
{
    //TODO
    throw std::runtime_error{"set_title() - Not yet implemented"};
}

stdx::optional<int32_t> engine_track_impl::track_number()
{
    //TODO
    throw std::runtime_error{"track_number() - Not yet implemented"};
}

void engine_track_impl::set_track_number(stdx::optional<int32_t> track_number)
{
    //TODO
    throw std::runtime_error{"set_track_number() - Not yet implemented"};
}

std::vector<waveform_entry> engine_track_impl::waveform()
{
    //TODO
    throw std::runtime_error{"waveform() - Not yet implemented"};
}

void engine_track_impl::set_waveform(std::vector<waveform_entry> waveform)
{
    //TODO
    throw std::runtime_error{"set_waveform() - Not yet implemented"};
}

stdx::optional<int32_t> engine_track_impl::year()
{
    //TODO
    throw std::runtime_error{"year() - Not yet implemented"};
}

void engine_track_impl::set_year(stdx::optional<int32_t> year)
{
    //TODO
    throw std::runtime_error{"set_year() - Not yet implemented"};
}

}  // namespace djinterop::engine::v2
