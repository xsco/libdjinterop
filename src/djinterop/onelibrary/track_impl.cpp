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

#include "track_impl.hpp"

#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/exceptions.hpp>

#include "../util/filesystem.hpp"
#include "content_table.hpp"
#include "crate_impl.hpp"
#include "database_impl.hpp"
#include "playlist_table.hpp"

namespace djinterop::onelibrary
{
// Loudness, beatgrids, waveforms, hot cues and loops are absent throughout:
// rekordbox leaves them in the ANLZ files that `analysisDataFilePath` points
// at, and exports an empty `cue` table.  Their accessors read as no value
// rather than raising.

track_impl::track_impl(
    std::shared_ptr<onelibrary_context> context, int64_t id) :
    djinterop::track_impl{id}, context_{std::move(context)}
{
}

track_snapshot track_impl::snapshot() const
{
    const auto row = content_table{context_}.get(id());
    if (!row)
        throw track_deleted{id()};

    return to_snapshot(*row);
}

bool track_impl::is_valid()
{
    return content_table{context_}.exists(id());
}

database track_impl::db()
{
    return database{std::make_shared<database_impl>(context_)};
}

std::vector<djinterop::crate> track_impl::containing_crates()
{
    // A crate and a playlist are the same row in this format, so the crates
    // containing a track are the playlists that hold it.
    std::vector<djinterop::crate> results;
    for (const auto& playlist_id :
         playlist_table{context_}.playlists_containing(id()))
        results.push_back(make_crate(context_, playlist_id));

    return results;
}

std::string track_impl::relative_path()
{
    const auto path = snapshot().relative_path;
    return path.value_or(std::string{});
}

std::string track_impl::filename()
{
    return util::get_filename(relative_path());
}

std::string track_impl::file_extension()
{
    return util::get_file_extension(relative_path()).value_or(std::string{});
}

std::optional<std::string> track_impl::album()
{
    return snapshot().album;
}

std::optional<std::string> track_impl::artist()
{
    return snapshot().artist;
}

std::optional<double> track_impl::average_loudness()
{
    return std::nullopt;
}

std::vector<beatgrid_marker> track_impl::beatgrid()
{
    return {};
}

std::optional<int> track_impl::bitrate()
{
    return snapshot().bitrate;
}

std::optional<double> track_impl::bpm()
{
    return snapshot().bpm;
}

std::optional<std::string> track_impl::comment()
{
    return snapshot().comment;
}

std::optional<std::string> track_impl::composer()
{
    return snapshot().composer;
}

std::optional<std::chrono::milliseconds> track_impl::duration()
{
    return snapshot().duration;
}

std::optional<std::string> track_impl::genre()
{
    return snapshot().genre;
}

std::optional<hot_cue> track_impl::hot_cue_at(int)
{
    return std::nullopt;
}

std::vector<std::optional<hot_cue>> track_impl::hot_cues()
{
    return {};
}

std::optional<musical_key> track_impl::key()
{
    return snapshot().key;
}

std::optional<std::chrono::system_clock::time_point>
track_impl::last_played_at()
{
    // The database counts plays but does not record when the last one was.
    return std::nullopt;
}

std::optional<loop> track_impl::loop_at(int)
{
    return std::nullopt;
}

std::vector<std::optional<loop>> track_impl::loops()
{
    return {};
}

std::optional<double> track_impl::main_cue()
{
    return std::nullopt;
}

std::optional<std::string> track_impl::publisher()
{
    return snapshot().publisher;
}

std::optional<int> track_impl::rating()
{
    return snapshot().rating;
}

std::optional<unsigned long long> track_impl::sample_count()
{
    return snapshot().sample_count;
}

std::optional<double> track_impl::sample_rate()
{
    return snapshot().sample_rate;
}

std::optional<std::string> track_impl::title()
{
    return snapshot().title;
}

std::optional<int> track_impl::track_number()
{
    return snapshot().track_number;
}

std::vector<waveform_entry> track_impl::waveform()
{
    return {};
}

std::optional<int> track_impl::year()
{
    return snapshot().year;
}

void track_impl::update(const track_snapshot&)
{
    read_only();
}

void track_impl::set_album(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_artist(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_average_loudness(std::optional<double>)
{
    read_only();
}

void track_impl::set_beatgrid(std::vector<beatgrid_marker>)
{
    read_only();
}

void track_impl::set_bitrate(std::optional<int>)
{
    read_only();
}

void track_impl::set_bpm(std::optional<double>)
{
    read_only();
}

void track_impl::set_comment(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_composer(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_duration(std::optional<std::chrono::milliseconds>)
{
    read_only();
}

void track_impl::set_genre(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_hot_cue_at(int, std::optional<hot_cue>)
{
    read_only();
}

void track_impl::set_hot_cues(std::vector<std::optional<hot_cue>>)
{
    read_only();
}

void track_impl::set_key(std::optional<musical_key>)
{
    read_only();
}

void track_impl::set_last_played_at(
    std::optional<std::chrono::system_clock::time_point>)
{
    read_only();
}

void track_impl::set_loop_at(int, std::optional<loop>)
{
    read_only();
}

void track_impl::set_loops(std::vector<std::optional<loop>>)
{
    read_only();
}

void track_impl::set_main_cue(std::optional<double>)
{
    read_only();
}

void track_impl::set_publisher(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_rating(std::optional<int>)
{
    read_only();
}

void track_impl::set_relative_path(std::string)
{
    read_only();
}

void track_impl::set_sample_count(std::optional<unsigned long long>)
{
    read_only();
}

void track_impl::set_sample_rate(std::optional<double>)
{
    read_only();
}

void track_impl::set_title(std::optional<std::string>)
{
    read_only();
}

void track_impl::set_track_number(std::optional<int>)
{
    read_only();
}

void track_impl::set_waveform(std::vector<waveform_entry>)
{
    read_only();
}

void track_impl::set_year(std::optional<int>)
{
    read_only();
}

}  // namespace djinterop::onelibrary
