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
#include <stdexcept>

#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>

#include "../../util/convert.hpp"
#include "../../util/filesystem.hpp"
#include "convert_beatgrid.hpp"
#include "convert_hot_cues.hpp"
#include "convert_loops.hpp"
#include "convert_track.hpp"
#include "convert_waveform.hpp"
#include "database_impl.hpp"
#include "track_impl.hpp"

namespace djinterop::engine::v3
{
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace
{
track_row snapshot_to_track_row(
    const track_snapshot& snapshot, const information_row& information)
{
    if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required to write a track"};
    }

    auto filename = djinterop::util::get_filename(*snapshot.relative_path);
    auto rating = convert::write::rating(snapshot.rating);

    auto file_type = djinterop::util::get_file_extension(filename);
    if (!file_type)
    {
        throw invalid_track_snapshot{
            "Snapshot refers to a file with no file extension, and so cannot "
            "auto-determine file type based on extension"};
    }

    std::chrono::system_clock::time_point date_created;
    std::chrono::system_clock::time_point date_added =
        std::chrono::system_clock::now();

    auto converted_bpm = convert::write::bpm(snapshot.bpm);
    auto converted_key = convert::write::key(snapshot.key);
    auto converted_duration = convert::write::duration(snapshot.duration);

    return track_row{
        .id = TRACK_ROW_ID_NONE,
        .play_order = djinterop::util::optional_static_cast<int64_t>(snapshot.track_number),
        .length = converted_duration,
        .bpm = converted_bpm.bpm,
        .year = djinterop::util::optional_static_cast<int64_t>(snapshot.year),
        .path = *snapshot.relative_path,
        .filename = filename,
        .bitrate = djinterop::util::optional_static_cast<int64_t>(snapshot.bitrate),
        .bpm_analyzed = converted_bpm.bpm_analyzed,
        .album_art_id = ALBUM_ART_ID_NONE,
        .file_bytes = djinterop::util::optional_static_cast<int64_t>(snapshot.file_bytes),
        .title = snapshot.title,
        .artist = snapshot.artist,
        .album = snapshot.album,
        .genre = snapshot.genre,
        .comment = snapshot.comment,
        .label = snapshot.publisher,
        .composer = snapshot.composer,
        .remixer = {},
        .key = converted_key.key,
        .rating = rating,
        .album_art = {},
        .time_last_played = snapshot.last_played_at,
        .is_played = false,
        .file_type = *file_type,
        .is_analyzed = true,
        .date_created = date_created,
        .date_added = date_added,
        .is_available = true,
        .is_metadata_of_packed_track_changed = false,
        .is_performance_data_of_packed_track_changed = false,
        .played_indicator = {},
        .is_metadata_imported = true,
        .pdb_import_key = 0,
        .streaming_source = {},
        .uri = {},
        .is_beat_grid_locked = false,
        .origin_database_uuid = information.uuid,
        .origin_track_id = 0,
        .streaming_flags = 0,
        .explicit_lyrics = false};
}
performance_data_row snapshot_to_performance_data_row(
    int64_t id, const track_snapshot& snapshot)
{
    auto converted_average_loudness =
        convert::write::average_loudness(snapshot.average_loudness);
    auto converted_key = convert::write::key(snapshot.key);
    auto converted_sample_count =
        convert::write::sample_count(snapshot.sample_count);
    auto converted_sample_rate =
        convert::write::sample_rate(snapshot.sample_rate);

    track_data_blob track_data{
        converted_sample_rate,        converted_sample_count.track_data_samples,
        converted_key.track_data_key, converted_average_loudness,
        converted_average_loudness,   converted_average_loudness};

    auto overview_waveform_data = convert::write::waveform(
        snapshot.waveform, snapshot.sample_count, snapshot.sample_rate);

    auto converted_beatgrid = convert::write::beatgrid(snapshot.beatgrid);

    // Real beat data seems to have 9 additional zero bytes at the end!
    std::vector<std::byte> beat_data_extra(9, std::byte{0});

    beat_data_blob beat_data{
        converted_sample_rate,
        converted_sample_count.beat_data_samples,
        converted_beatgrid.is_beatgrid_set,
        converted_beatgrid.default_beat_grid,
        converted_beatgrid.adjusted_beat_grid,
        beat_data_extra};

    quick_cues_blob quick_cues;
    quick_cues.default_main_cue = convert::write::main_cue(snapshot.main_cue);
    quick_cues.adjusted_main_cue = convert::write::main_cue(snapshot.main_cue);
    quick_cues.is_main_cue_adjusted = true;
    quick_cues.quick_cues = convert::write::hot_cues(snapshot.hot_cues);

    loops_blob loops = convert::write::loops(snapshot.loops);

    std::optional<int64_t> third_party_source_id;
    int64_t active_on_load_loops = 0;

    return performance_data_row{
        .track_id = id,
        .track_data = track_data,
        .overview_waveform_data = overview_waveform_data,
        .beat_data = beat_data,
        .quick_cues = quick_cues,
        .loops = loops,
        .third_party_source_id = third_party_source_id,
        .active_on_load_loops = active_on_load_loops,
    };
}

}  // namespace

track_impl::track_impl(std::shared_ptr<engine_library> library, int64_t id) :
    djinterop::track_impl{id}, library_{std::move(library)},
    track_{library_->track()}, performance_data_{library_->performance_data()}
{
}

track_snapshot track_impl::snapshot() const
{
    const auto information = library_->information().get();
    const auto track_row_maybe = track_.get(id());
    if (!track_row_maybe)
        throw djinterop::track_deleted{id()};

    const auto performance_data_row_maybe = performance_data_.get(id());
    if (!performance_data_row_maybe)
        throw djinterop::track_database_inconsistency{
            "No performance data row is present for track", id()};

    track_snapshot snapshot{};
    const auto& track_row = *track_row_maybe;
    const auto& performance_data_row = *performance_data_row_maybe;

    snapshot.album = track_row.album;
    snapshot.artist = track_row.artist;
    snapshot.average_loudness =
        convert::read::average_loudness(performance_data_row.track_data);
    snapshot.beatgrid = convert::read::beatgrid_markers(
        performance_data_row.beat_data.adjusted_beat_grid);
    snapshot.bitrate =
        djinterop::util::optional_static_cast<int>(track_row.bitrate);
    snapshot.bpm = convert::read::bpm(track_row.bpm_analyzed, track_row.bpm);
    snapshot.comment = track_row.comment;
    snapshot.composer = track_row.composer;
    snapshot.duration = convert::read::duration(track_row.length);
    snapshot.file_bytes =
        djinterop::util::optional_static_cast<unsigned long long>(
            track_row.file_bytes);
    snapshot.genre = track_row.genre;
    snapshot.hot_cues =
        convert::read::hot_cues(performance_data_row.quick_cues);
    snapshot.key = convert::read::key(track_row.key);
    snapshot.last_played_at = track_row.time_last_played;
    snapshot.loops = convert::read::loops(performance_data_row.loops);
    snapshot.main_cue = convert::read::main_cue(
        performance_data_row.quick_cues.adjusted_main_cue);
    snapshot.publisher = track_row.label;
    snapshot.rating = convert::read::rating(track_row.rating);
    snapshot.relative_path = track_row.path;
    snapshot.sample_count =
        convert::read::sample_count(performance_data_row.track_data);
    snapshot.sample_rate =
        convert::read::sample_rate(performance_data_row.track_data);
    snapshot.title = track_row.title;
    snapshot.track_number =
        djinterop::util::optional_static_cast<int>(track_row.play_order);
    snapshot.waveform =
        convert::read::waveform(performance_data_row.overview_waveform_data);
    snapshot.year = djinterop::util::optional_static_cast<int>(track_row.year);

    return snapshot;
}

void track_impl::update(const track_snapshot& snapshot)
{
    if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required to update a track"};
    }

    const auto information = library_->information().get();
    auto track_row = snapshot_to_track_row(snapshot, information);
    track_row.id = id();

    track_.update(track_row);

    const auto performance_data_row = snapshot_to_performance_data_row(track_row.id, snapshot);
    performance_data_.update(performance_data_row);
}

std::optional<std::string> track_impl::album()
{
    return track_.get_album(id());
}

void track_impl::set_album(std::optional<std::string> album)
{
    track_.set_album(id(), album);
}

std::optional<std::string> track_impl::artist()
{
    return track_.get_artist(id());
}

void track_impl::set_artist(std::optional<std::string> artist)
{
    track_.set_artist(id(), artist);
}

std::optional<double> track_impl::average_loudness()
{
    auto track_data = performance_data_.get_track_data(id());
    return convert::read::average_loudness(track_data);
}

void track_impl::set_average_loudness(std::optional<double> average_loudness)
{
    auto track_data = performance_data_.get_track_data(id());
    auto converted = convert::write::average_loudness(average_loudness);
    track_data.average_loudness_low = converted;
    track_data.average_loudness_mid = converted;
    track_data.average_loudness_high = converted;
    performance_data_.set_track_data(id(), track_data);
}

std::vector<beatgrid_marker> track_impl::beatgrid()
{
    auto beat_data = performance_data_.get_beat_data(id());
    return convert::read::beatgrid_markers(beat_data.adjusted_beat_grid);
}

void track_impl::set_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    auto beat_data = performance_data_.get_beat_data(id());

    auto converted_beatgrid = convert::write::beatgrid(beatgrid);
    beat_data.adjusted_beat_grid = converted_beatgrid.adjusted_beat_grid;
    beat_data.default_beat_grid = converted_beatgrid.default_beat_grid;
    beat_data.is_beatgrid_set = converted_beatgrid.is_beatgrid_set;

    performance_data_.set_beat_data(id(), beat_data);
}

std::optional<int> track_impl::bitrate()
{
    return djinterop::util::optional_static_cast<int>(track_.get_bitrate(id()));
}

void track_impl::set_bitrate(std::optional<int> bitrate)
{
    track_.set_bitrate(
        id(), djinterop::util::optional_static_cast<int64_t>(bitrate));
}

std::optional<double> track_impl::bpm()
{
    return convert::read::bpm(
        track_.get_bpm_analyzed(id()), track_.get_bpm(id()));
}

void track_impl::set_bpm(std::optional<double> bpm)
{
    auto fields = convert::write::bpm(bpm);
    track_.set_bpm_analyzed(id(), fields.bpm_analyzed);
    track_.set_bpm(id(), fields.bpm);
}

std::optional<std::string> track_impl::comment()
{
    return track_.get_comment(id());
}

void track_impl::set_comment(std::optional<std::string> comment)
{
    track_.set_comment(id(), comment);
}

std::optional<std::string> track_impl::composer()
{
    return track_.get_composer(id());
}

void track_impl::set_composer(std::optional<std::string> composer)
{
    track_.set_composer(id(), composer);
}

database track_impl::db()
{
    return database{std::make_shared<database_impl>(library_)};
}

std::vector<crate> track_impl::containing_crates()
{
    // TODO (mr-smidge): track::containing_crates() not yet implemented.
    throw std::runtime_error{"containing_crates() - Not yet implemented"};
}

std::optional<milliseconds> track_impl::duration()
{
    auto length = track_.get_length(id());
    return convert::read::duration(length);
}

void track_impl::set_duration(std::optional<std::chrono::milliseconds> duration)
{
    track_.set_length(id(), convert::write::duration(duration));
}

std::string track_impl::file_extension()
{
    auto rel_path = relative_path();
    return djinterop::util::get_file_extension(rel_path).value_or(
        std::string{});
}

std::string track_impl::filename()
{
    auto rel_path = relative_path();
    return djinterop::util::get_filename(rel_path);
}

std::optional<std::string> track_impl::genre()
{
    return track_.get_genre(id());
}

void track_impl::set_genre(std::optional<std::string> genre)
{
    track_.set_genre(id(), genre);
}

std::optional<hot_cue> track_impl::hot_cue_at(int index)
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    if (index < 0 || (unsigned)index > quick_cues.quick_cues.size())
    {
        throw std::out_of_range{
            "Request for hot cue at given index exceeds maximum number of cues "
            "on track"};
    }

    return convert::read::hot_cue(quick_cues.quick_cues[index]);
}

void track_impl::set_hot_cue_at(int index, std::optional<hot_cue> cue)
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    if (index < 0 || (unsigned)index > quick_cues.quick_cues.size())
    {
        throw std::out_of_range{
            "Request to set hot cue at given index exceeds maximum number of "
            "cues on track"};
    }

    quick_cues.quick_cues[index] = convert::write::hot_cue(cue);
    performance_data_.set_quick_cues(id(), quick_cues);
}

std::vector<std::optional<hot_cue>> track_impl::hot_cues()
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    return convert::read::hot_cues(quick_cues);
}

void track_impl::set_hot_cues(std::vector<std::optional<hot_cue>> cues)
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    quick_cues.quick_cues = convert::write::hot_cues(cues);
    performance_data_.set_quick_cues(id(), quick_cues);
}

bool track_impl::is_valid()
{
    return track_.exists(id());
}

std::optional<musical_key> track_impl::key()
{
    return convert::read::key(track_.get_key(id()));
}

void track_impl::set_key(std::optional<musical_key> key)
{
    auto converted = convert::write::key(key);
    track_.set_key(id(), converted.key);

    auto track_data = performance_data_.get_track_data(id());
    track_data.key = converted.track_data_key;
    performance_data_.set_track_data(id(), track_data);
}

std::optional<system_clock::time_point> track_impl::last_played_at()
{
    return track_.get_time_last_played(id());
}

void track_impl::set_last_played_at(
    std::optional<system_clock::time_point> played_at)
{
    track_.set_time_last_played(id(), played_at);
}

std::optional<loop> track_impl::loop_at(int index)
{
    auto loops = performance_data_.get_loops(id());
    if (index < 0 || (unsigned)index > loops.loops.size())
    {
        throw std::out_of_range{
            "Request for loop at given index exceeds maximum number of loops "
            "on track"};
    }

    return convert::read::loop(loops.loops[index]);
}

void track_impl::set_loop_at(int index, std::optional<loop> l)
{
    auto loops = performance_data_.get_loops(id());
    if (index < 0 || (unsigned)index > loops.loops.size())
    {
        throw std::out_of_range{
            "Request to set loop at given index exceeds maximum number of "
            "loops on track"};
    }

    loops.loops[index] = convert::write::loop(l);
    performance_data_.set_loops(id(), loops);
}

std::vector<std::optional<loop>> track_impl::loops()
{
    return convert::read::loops(performance_data_.get_loops(id()));
}

void track_impl::set_loops(std::vector<std::optional<loop>> loops)
{
    auto converted = convert::write::loops(loops);
    performance_data_.set_loops(id(), converted);
}

std::optional<double> track_impl::main_cue()
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    auto cue = quick_cues.adjusted_main_cue;
    if (cue == 0)
        return std::nullopt;

    return std::make_optional(cue);
}

void track_impl::set_main_cue(std::optional<double> sample_offset)
{
    auto quick_cues = performance_data_.get_quick_cues(id());
    quick_cues.adjusted_main_cue = sample_offset.value_or(0);
    quick_cues.default_main_cue = sample_offset.value_or(0);
    quick_cues.is_main_cue_adjusted = true;
    performance_data_.set_quick_cues(id(), quick_cues);
}

std::optional<std::string> track_impl::publisher()
{
    return track_.get_label(id());
}

void track_impl::set_publisher(std::optional<std::string> publisher)
{
    track_.set_label(id(), publisher);
}

std::optional<int> track_impl::rating()
{
    auto rating = track_.get_rating(id());
    return convert::read::rating(rating);
}

void track_impl::set_rating(std::optional<int> rating)
{
    track_.set_rating(id(), convert::write::rating(rating));
}

std::string track_impl::relative_path()
{
    return track_.get_path(id());
}

void track_impl::set_relative_path(std::string relative_path)
{
    track_.set_path(id(), relative_path);
}

std::optional<unsigned long long> track_impl::sample_count()
{
    auto track_data = performance_data_.get_track_data(id());
    return convert::read::sample_count(track_data);
}

void track_impl::set_sample_count(
    std::optional<unsigned long long> sample_count)
{
    auto converted = convert::write::sample_count(sample_count);

    auto track_data = performance_data_.get_track_data(id());
    track_data.samples = converted.track_data_samples;

    auto beat_data = performance_data_.get_beat_data(id());
    beat_data.samples = converted.beat_data_samples;

    performance_data_.set_track_data(id(), track_data);
    performance_data_.set_beat_data(id(), beat_data);
}

std::optional<double> track_impl::sample_rate()
{
    auto track_data = performance_data_.get_track_data(id());
    return convert::read::sample_rate(track_data);
}

void track_impl::set_sample_rate(std::optional<double> sample_rate)
{
    auto converted = convert::write::sample_rate(sample_rate);

    auto track_data = performance_data_.get_track_data(id());
    track_data.sample_rate = converted;

    auto beat_data = performance_data_.get_beat_data(id());
    beat_data.sample_rate = converted;

    performance_data_.set_track_data(id(), track_data);
    performance_data_.set_beat_data(id(), beat_data);
}

std::optional<std::string> track_impl::title()
{
    return track_.get_title(id());
}

void track_impl::set_title(std::optional<std::string> title)
{
    track_.set_title(id(), title);
}

std::optional<int> track_impl::track_number()
{
    return djinterop::util::optional_static_cast<int>(
        track_.get_play_order(id()));
}

void track_impl::set_track_number(std::optional<int> track_number)
{
    track_.set_play_order(
        id(), djinterop::util::optional_static_cast<int64_t>(track_number));
}

std::vector<waveform_entry> track_impl::waveform()
{
    // Engine 2.x only has an overview waveform.
    auto overview_waveform_data =
        performance_data_.get_overview_waveform_data(id());
    return convert::read::waveform(overview_waveform_data);
}

void track_impl::set_waveform(std::vector<waveform_entry> waveform)
{
    // Engine 2.x only has an overview waveform.
    auto overview_waveform_data =
        convert::write::waveform(waveform, sample_count(), sample_rate());
    performance_data_.set_overview_waveform_data(id(), overview_waveform_data);
}

std::optional<int> track_impl::year()
{
    return djinterop::util::optional_static_cast<int>(track_.get_year(id()));
}

void track_impl::set_year(std::optional<int> year)
{
    track_.set_year(id(), djinterop::util::optional_static_cast<int64_t>(year));
}

track create_track(
    const std::shared_ptr<engine_library>& library,
    const track_snapshot& snapshot)
{
    if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required to create a track"};
    }

    auto information = library->information().get();

    auto track_row = snapshot_to_track_row(snapshot, information);
    auto id = library->track().add(track_row);

    auto performance_data_row =
        snapshot_to_performance_data_row(id, snapshot);
    library->performance_data().update(performance_data_row);

    return track{std::make_shared<track_impl>(library, id)};
}

}  // namespace djinterop::engine::v3
