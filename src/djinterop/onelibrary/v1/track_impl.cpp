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

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <utility>

#include <djinterop/database.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/onelibrary/v1/content_table.hpp>
#include <djinterop/onelibrary/v1/playlist_table.hpp>

#include "../../util/filesystem.hpp"
#include "crate_impl.hpp"
#include "database_impl.hpp"
#include "track_conversion.hpp"

namespace djinterop::onelibrary::v1
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
    const auto path = content_table{context_}.get_path(id());
    if (!path)
        return {};

    // Paths are absolute within the device, whereas djinterop wants them
    // relative to the directory of the database.
    return path->front() == '/' ? path->substr(1) : *path;
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
    return content_table{context_}.get_album(id());
}

std::optional<std::string> track_impl::artist()
{
    return content_table{context_}.get_artist(id());
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
    const auto bitrate = content_table{context_}.get_bitrate(id());
    if (!bitrate || *bitrate <= 0)
        return std::nullopt;

    return static_cast<int>(*bitrate);
}

std::optional<double> track_impl::bpm()
{
    const auto bpm_x100 = content_table{context_}.get_bpm_x100(id());
    if (!bpm_x100 || *bpm_x100 <= 0)
        return std::nullopt;

    return static_cast<double>(*bpm_x100) / 100;
}

std::optional<std::string> track_impl::comment()
{
    return content_table{context_}.get_comment(id());
}

std::optional<std::string> track_impl::composer()
{
    return content_table{context_}.get_composer(id());
}

std::optional<std::chrono::milliseconds> track_impl::duration()
{
    const auto length = content_table{context_}.get_length(id());
    if (!length || length->count() <= 0)
        return std::nullopt;

    return std::chrono::duration_cast<std::chrono::milliseconds>(*length);
}

std::optional<std::string> track_impl::genre()
{
    return content_table{context_}.get_genre(id());
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
    const auto name = content_table{context_}.get_key(id());
    if (!name)
        return std::nullopt;

    return parse_musical_key(*name);
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
    return content_table{context_}.get_label(id());
}

std::optional<int> track_impl::rating()
{
    const auto stars = content_table{context_}.get_rating_stars(id());
    if (!stars)
        return std::nullopt;

    // djinterop rates a track from zero to one hundred, where rekordbox uses
    // whole stars.
    return static_cast<int>(std::clamp<int64_t>(*stars, 0, 5) * 20);
}

std::optional<unsigned long long> track_impl::sample_count()
{
    // The database records a duration in whole seconds and no sample count,
    // so the count can only be recovered to that precision.
    const content_table content{context_};
    const auto length = content.get_length(id());
    const auto rate = content.get_sampling_rate(id());
    if (!length || length->count() <= 0 || !rate || *rate <= 0)
        return std::nullopt;

    return static_cast<unsigned long long>(length->count() * *rate);
}

std::optional<double> track_impl::sample_rate()
{
    const auto rate = content_table{context_}.get_sampling_rate(id());
    if (!rate || *rate <= 0)
        return std::nullopt;

    return static_cast<double>(*rate);
}

std::optional<std::string> track_impl::title()
{
    return content_table{context_}.get_title(id());
}

std::optional<int> track_impl::track_number()
{
    const auto number = content_table{context_}.get_track_number(id());
    if (!number || *number <= 0)
        return std::nullopt;

    return static_cast<int>(*number);
}

std::vector<waveform_entry> track_impl::waveform()
{
    return {};
}

std::optional<int> track_impl::year()
{
    const auto year = content_table{context_}.get_release_year(id());
    if (!year || *year <= 0)
        return std::nullopt;

    return static_cast<int>(*year);
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

}  // namespace djinterop::onelibrary::v1
