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

#include <djinterop/djinterop.hpp>

#include "../../util/chrono.hpp"
#include "../../util/convert.hpp"
#include "../../util/filesystem.hpp"
#include "../../util/sqlite_transaction.hpp"
#include "djinterop/engine/track_utils.hpp"
#include "engine_crate_impl.hpp"
#include "engine_database_impl.hpp"
#include "engine_track_impl.hpp"

namespace djinterop::engine::v1
{
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace
{
const int64_t default_track_type = 1;

const int64_t default_is_external_track = 0;

const stdx::optional<std::string> default_uuid_of_external_database;

const stdx::optional<int64_t> default_id_track_in_external_database;

const int64_t no_album_art_id = 1;

const int64_t default_pdb_import_key = 0;

const stdx::optional<std::string> default_uri;

const int64_t default_is_beatgrid_locked = 0;

const int64_t default_is_rendered = 0;

const int64_t default_has_serato_values = 0;

const int64_t default_has_rekordbox_values = 0;

const int64_t default_has_traktor_values = 0;

struct length_field_data
{
    stdx::optional<int64_t> length;
    stdx::optional<int64_t> length_calculated;
    stdx::optional<std::string> length_mm_ss;
};

length_field_data to_length_fields(
    stdx::optional<std::chrono::milliseconds> duration,
    stdx::optional<unsigned long long> sample_count,
    stdx::optional<double> sample_rate)
{
    stdx::optional<int64_t> length;
    stdx::optional<std::string> length_mm_ss;
    if (duration)
    {
        length = static_cast<int64_t>(duration->count() / 1000);

        // String metadata, type 10, is the duration encoded as "MM:SS".
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0');
        oss << (*length / 60);
        oss << ":";
        oss << std::setw(2) << std::setfill('0');
        oss << (*length % 60);
        length_mm_ss = oss.str();
    }

    // A zero sample rate is interpreted as no sample rate.
    stdx::optional<int64_t> length_calculated;
    if (sample_count && sample_rate && *sample_rate != 0)
    {
        length_calculated = static_cast<int64_t>(*sample_count) /
                            static_cast<int64_t>(*sample_rate);
    }

    return length_field_data{length, length_calculated, length_mm_ss};
}

struct bpm_field_data
{
    stdx::optional<int64_t> bpm;
    stdx::optional<double> bpm_analyzed;
};

bpm_field_data to_bpm_fields(
    stdx::optional<double> bpm, stdx::optional<double> sample_rate,
    const std::vector<beatgrid_marker>& beatgrid)
{
    stdx::optional<int64_t> rounded_bpm;
    if (bpm)
    {
        rounded_bpm = static_cast<int64_t>(*bpm);
    }

    stdx::optional<double> bpm_analyzed;
    if (sample_rate && beatgrid.size() >= 2)
    {
        auto marker_1 = beatgrid[0];
        auto marker_2 = beatgrid[1];
        if (marker_1.sample_offset != marker_2.sample_offset)
        {
            bpm_analyzed = *sample_rate * 60 *
                           (marker_2.index - marker_1.index) /
                           (marker_2.sample_offset - marker_1.sample_offset);
        }
    }

    return bpm_field_data{rounded_bpm, bpm_analyzed};
}

struct timestamp_field_data
{
    stdx::optional<int64_t> last_played_at_ts;
    stdx::optional<int64_t> last_modified_at_ts;
    stdx::optional<int64_t> last_accessed_at_ts;
    stdx::optional<std::string> ever_played;
};

timestamp_field_data to_timestamp_fields(
    stdx::optional<std::chrono::system_clock::time_point> last_played_at)
{
    auto last_played_at_ts = djinterop::util::to_timestamp(last_played_at);

    stdx::optional<std::string> ever_played =
        last_played_at ? stdx::optional<std::string>{"1"} : stdx::nullopt;

    return timestamp_field_data{
        last_played_at_ts, stdx::nullopt, stdx::nullopt, ever_played};
}

stdx::optional<int64_t> to_key_num(stdx::optional<musical_key> key)
{
    stdx::optional<int64_t> key_num;
    if (key)
    {
        key_num = static_cast<int64_t>(*key);
    }

    return key_num;
}

track_data to_track_data(
    stdx::optional<unsigned long long> sample_count,
    stdx::optional<double> sample_rate, stdx::optional<double> average_loudness,
    stdx::optional<musical_key> key)
{
    return track_data{
        sample_rate,
        djinterop::util::optional_static_cast<int64_t>(sample_count),
        average_loudness, key};
}

quick_cues_data to_cues_data(
    const std::vector<stdx::optional<hot_cue> >& hot_cues,
    stdx::optional<double> main_cue)
{
    auto data = quick_cues_data{
        hot_cues, main_cue.value_or(0), main_cue.value_or(0)};
    if (data.hot_cues.size() < 8)
        data.hot_cues.resize(8);

    return data;
}

beat_data to_beat_data(
    stdx::optional<unsigned long long> sample_count,
    stdx::optional<double> sample_rate,
    const std::vector<beatgrid_marker>& beatgrid)
{
    return beat_data{
        sample_rate,
        djinterop::util::optional_static_cast<double>(sample_count), beatgrid,
        beatgrid};
}

loops_data to_loops_data(const std::vector<stdx::optional<loop> >& loops)
{
    auto data = loops_data{loops};
    if (data.loops.size() < 8)
        data.loops.resize(8);

    return data;
}

overview_waveform_data to_overview_waveform_data(
    stdx::optional<unsigned long long> sample_count,
    stdx::optional<double> sample_rate,
    const std::vector<waveform_entry>& waveform)
{
    if (!sample_count || !sample_rate)
    {
        return overview_waveform_data{0};
    }

    auto extents =
        util::calculate_overview_waveform_extents(*sample_count, *sample_rate);
    std::vector<waveform_entry> overview_waveform;
    if (!waveform.empty())
    {
        // Calculate an overview waveform automatically.
        overview_waveform.reserve(extents.size);
        for (unsigned long long i = 0; i < extents.size; ++i)
        {
            auto entry = waveform
                [waveform.size() * (2 * i + 1) / (2 * extents.size)];
            overview_waveform.push_back(entry);
        }
    }

    return overview_waveform_data{extents.samples_per_entry, overview_waveform};
}

high_res_waveform_data to_high_res_waveform_data(
    stdx::optional<unsigned long long> sample_count,
    stdx::optional<double> sample_rate,
    const std::vector<waveform_entry>& waveform)
{
    // Make the assumption that the client has respected the required number
    // of samples per entry when constructing the waveform.
    auto extents = util::calculate_high_resolution_waveform_extents(
        *sample_count, *sample_rate);

    // TODO (mr-smidge) Rework to avoid copying the waveform here.
    return high_res_waveform_data{extents.samples_per_entry, waveform};
}

}  // namespace

engine_track_impl::engine_track_impl(
    std::shared_ptr<engine_storage> storage, int64_t id) :
    track_impl{id},
    storage_{std::move(storage)}
{
}

beat_data engine_track_impl::get_beat_data()
{
    return storage_->get_performance_data_column<beat_data>(id(), "beatData");
}

void engine_track_impl::set_beat_data(beat_data data)
{
    storage_->set_performance_data_column(id(), "beatData", data);
}

high_res_waveform_data engine_track_impl::get_high_res_waveform_data()
{
    return storage_->get_performance_data_column<high_res_waveform_data>(
        id(), "highResolutionWaveFormData");
}

void engine_track_impl::set_high_res_waveform_data(high_res_waveform_data data)
{
    storage_->set_performance_data_column(
        id(), "highResolutionWaveFormData", data);
}

loops_data engine_track_impl::get_loops_data()
{
    return storage_->get_performance_data_column<loops_data>(id(), "loops");
}

void engine_track_impl::set_loops_data(loops_data data)
{
    storage_->set_performance_data_column(id(), "loops", data);
}

overview_waveform_data engine_track_impl::get_overview_waveform_data()
{
    return storage_->get_performance_data_column<overview_waveform_data>(
        id(), "overviewWaveFormData");
}

void engine_track_impl::set_overview_waveform_data(overview_waveform_data data)
{
    // As the overview waveform does not store opacity, it is defaulted to 255
    // when read back.  If we also set it to 255 here, we can apply a check in
    // `set_perfdata` that a round-trip encode/decode gives the same data.
    for (auto&& entry : data.waveform)
    {
        entry.low.opacity = 255;
        entry.mid.opacity = 255;
        entry.high.opacity = 255;
    }
    storage_->set_performance_data_column(id(), "overviewWaveFormData", data);
}

quick_cues_data engine_track_impl::get_quick_cues_data()
{
    return storage_->get_performance_data_column<quick_cues_data>(
        id(), "quickCues");
}

void engine_track_impl::set_quick_cues_data(quick_cues_data data)
{
    storage_->set_performance_data_column(id(), "quickCues", data);
}

track_data engine_track_impl::get_track_data()
{
    return storage_->get_performance_data_column<track_data>(id(), "trackData");
}

void engine_track_impl::set_track_data(track_data data)
{
    storage_->set_performance_data_column(id(), "trackData", data);
}

track_snapshot engine_track_impl::snapshot() const
{
    track_snapshot snapshot{};

    auto track_data = storage_->get_track(id());
    auto meta_data = storage_->get_all_meta_data(id());
    auto meta_data_integer = storage_->get_all_meta_data_integer(id());
    auto perf_data = storage_->get_performance_data(id());

    if (perf_data.beats)
    {
        snapshot.beatgrid = std::move(perf_data.beats->adjusted_beatgrid);
    }
    snapshot.main_cue =
        (perf_data.quick_cues && perf_data.quick_cues->adjusted_main_cue != 0)
            ? stdx::make_optional(perf_data.quick_cues->adjusted_main_cue)
            : stdx::nullopt;
    snapshot.average_loudness =
        perf_data.track_performance_data
            ? perf_data.track_performance_data->average_loudness
            : stdx::nullopt;
    snapshot.bitrate =
        djinterop::util::optional_static_cast<int>(track_data.bitrate);
    snapshot.bpm =
        track_data.bpm_analyzed ? track_data.bpm_analyzed
        : track_data.bpm
            ? stdx::make_optional(static_cast<double>(*track_data.bpm))
            : stdx::nullopt;
    if (track_data.length)
    {
        auto ms = 1000 * *track_data.length;
        snapshot.duration = stdx::make_optional(milliseconds{ms});
    }
    snapshot.file_bytes =
        djinterop::util::optional_static_cast<unsigned long long>(
            track_data.file_bytes);
    if (perf_data.quick_cues)
    {
        snapshot.hot_cues = std::move(perf_data.quick_cues->hot_cues);
    }
    snapshot.key = perf_data.track_performance_data
                       ? perf_data.track_performance_data->key
                       : stdx::nullopt;
    if (perf_data.loops)
    {
        snapshot.loops = std::move(perf_data.loops->loops);
    }
    snapshot.relative_path = std::move(track_data.relative_path);
    if (perf_data.track_performance_data &&
        perf_data.track_performance_data->sample_count)
    {
        snapshot.sample_count = *perf_data.track_performance_data->sample_count;
    }
    if (perf_data.track_performance_data &&
        perf_data.track_performance_data->sample_rate)
    {
        snapshot.sample_rate = *perf_data.track_performance_data->sample_rate;
    }
    snapshot.track_number =
        track_data.play_order
            ? stdx::make_optional(static_cast<int32_t>(*track_data.play_order))
            : stdx::nullopt;
    if (perf_data.high_res_waveform)
    {
        snapshot.waveform = std::move(perf_data.high_res_waveform->waveform);
    }
    snapshot.year =
        track_data.year
            ? stdx::make_optional(static_cast<int32_t>(*track_data.year))
            : stdx::nullopt;

    for (auto&& row : meta_data)
    {
        switch (row.type)
        {
            case metadata_str_type::title: snapshot.title = row.value; break;
            case metadata_str_type::artist: snapshot.artist = row.value; break;
            case metadata_str_type::album: snapshot.album = row.value; break;
            case metadata_str_type::genre: snapshot.genre = row.value; break;
            case metadata_str_type::comment:
                snapshot.comment = row.value;
                break;
            case metadata_str_type::publisher:
                snapshot.publisher = row.value;
                break;
            case metadata_str_type::composer:
                snapshot.composer = row.value;
                break;
            default: break;
        }
    }

    for (auto&& row : meta_data_integer)
    {
        switch (row.type)
        {
            case metadata_int_type::last_played_ts:
                snapshot.last_played_at =
                    djinterop::util::to_time_point(row.value);
                break;
            case metadata_int_type::musical_key:
                if (!snapshot.key)
                {
                    snapshot.key = stdx::make_optional(
                        static_cast<musical_key>(row.value));
                }

                break;
            default: break;
        }
    }

    return snapshot;
}

void engine_track_impl::update(const track_snapshot& snapshot)
{
    if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required on any track"};
    }

    auto length_fields = to_length_fields(
        snapshot.duration, snapshot.sample_count, snapshot.sample_rate);
    auto bpm_fields =
        to_bpm_fields(snapshot.bpm, snapshot.sample_rate, snapshot.beatgrid);
    auto filename = djinterop::util::get_filename(*snapshot.relative_path);
    auto extension = djinterop::util::get_file_extension(filename);
    auto track_number =
        snapshot.track_number
            ? stdx::make_optional(static_cast<int64_t>(*snapshot.track_number))
            : stdx::nullopt;
    auto year = snapshot.year
                    ? stdx::make_optional(static_cast<int64_t>(*snapshot.year))
                    : stdx::nullopt;
    auto timestamp_fields = to_timestamp_fields(snapshot.last_played_at);
    auto key_num = to_key_num(snapshot.key);
    auto clamped_rating = snapshot.rating
                              ? stdx::make_optional(static_cast<int64_t>(
                                    std::clamp(*snapshot.rating, 0, 100)))
                              : stdx::nullopt;
    stdx::optional<int64_t> last_play_hash;
    auto track_data = to_track_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.average_loudness,
        snapshot.key);
    auto overview_waveform_data = to_overview_waveform_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.waveform);
    auto high_res_waveform_data = to_high_res_waveform_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.waveform);
    auto beat_data = to_beat_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.beatgrid);
    auto cues_data = to_cues_data(snapshot.hot_cues, snapshot.main_cue);
    auto loops_data = to_loops_data(snapshot.loops);

    djinterop::util::sqlite_transaction trans{storage_->db};

    // Firstly, update the `Track` table entry.
    storage_->update_track(
        id(), track_number, length_fields.length,
        length_fields.length_calculated, bpm_fields.bpm, year,
        snapshot.relative_path, filename,
        djinterop::util::optional_static_cast<int64_t>(snapshot.bitrate),
        bpm_fields.bpm_analyzed, default_track_type, default_is_external_track,
        default_uuid_of_external_database,
        default_id_track_in_external_database, no_album_art_id,
        djinterop::util::optional_static_cast<int64_t>(snapshot.file_bytes),
        default_pdb_import_key, default_uri, default_is_beatgrid_locked);

    // Set string-based metadata.
    storage_->set_meta_data(
        id(), snapshot.title, snapshot.artist, snapshot.album, snapshot.genre,
        snapshot.comment, snapshot.publisher, snapshot.composer,
        length_fields.length_mm_ss, timestamp_fields.ever_played, extension);

    // Set integer-based metadata.
    storage_->set_meta_data_integer(
        id(), key_num, clamped_rating, timestamp_fields.last_played_at_ts,
        timestamp_fields.last_modified_at_ts,
        timestamp_fields.last_accessed_at_ts, last_play_hash);

    // Set performance data, or remove, as appropriate.
    auto has_perf_data =
        snapshot.sample_count || snapshot.sample_rate ||
        snapshot.average_loudness || !snapshot.beatgrid.empty() ||
        !cues_data.hot_cues.empty() || !loops_data.loops.empty();
    auto is_analysed = 1;
    if (has_perf_data)
    {
        storage_->set_performance_data(
            id(), is_analysed, default_is_rendered, track_data,
            high_res_waveform_data, overview_waveform_data, beat_data,
            cues_data, loops_data, default_has_serato_values,
            default_has_rekordbox_values, default_has_traktor_values);
    }
    else
    {
        storage_->clear_performance_data(id());
    }

    trans.commit();
}

stdx::optional<std::string> engine_track_impl::album()
{
    return storage_->get_meta_data(id(), metadata_str_type::album);
}

void engine_track_impl::set_album(stdx::optional<std::string> album)
{
    storage_->set_meta_data(id(), metadata_str_type::album, album);
}

stdx::optional<std::string> engine_track_impl::artist()
{
    return storage_->get_meta_data(id(), metadata_str_type::artist);
}

void engine_track_impl::set_artist(stdx::optional<std::string> artist)
{
    return storage_->set_meta_data(id(), metadata_str_type::artist, artist);
}

stdx::optional<double> engine_track_impl::average_loudness()
{
    return get_track_data().average_loudness;
}

void engine_track_impl::set_average_loudness(
    stdx::optional<double> average_loudness)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    auto track_d = get_track_data();

    // Zero average loudness is interpreted as no average loudness.
    track_d.average_loudness =
        average_loudness.value_or(0) == 0 ? stdx::nullopt : average_loudness;

    set_track_data(track_d);
    trans.commit();
}

std::vector<beatgrid_marker> engine_track_impl::beatgrid()
{
    auto beat_d = get_beat_data();
    return std::move(beat_d.adjusted_beatgrid);
}

void engine_track_impl::set_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    auto beat_d = get_beat_data();
    beat_d.adjusted_beatgrid = std::move(beatgrid);
    beat_d.default_beatgrid = beat_d.adjusted_beatgrid;
    set_beat_data(std::move(beat_d));
    trans.commit();
}

stdx::optional<int> engine_track_impl::bitrate()
{
    return djinterop::util::optional_static_cast<int>(
        storage_->get_track_column<stdx::optional<int64_t> >(id(), "bitrate"));
}

void engine_track_impl::set_bitrate(stdx::optional<int> bitrate)
{
    storage_->set_track_column(id(), "bitrate", bitrate);
}

stdx::optional<double> engine_track_impl::bpm()
{
    return storage_->get_track_column<stdx::optional<double> >(
        id(), "bpmAnalyzed");
}

void engine_track_impl::set_bpm(stdx::optional<double> bpm)
{
    storage_->set_track_column(id(), "bpmAnalyzed", bpm);
    stdx::optional<int64_t> ceiled_bpm;
    if (bpm)
    {
        ceiled_bpm = static_cast<int64_t>(std::ceil(*bpm));
    }
    storage_->set_track_column(id(), "bpm", ceiled_bpm);
}

stdx::optional<std::string> engine_track_impl::comment()
{
    return storage_->get_meta_data(id(), metadata_str_type::comment);
}

void engine_track_impl::set_comment(stdx::optional<std::string> comment)
{
    storage_->set_meta_data(id(), metadata_str_type::comment, comment);
}

stdx::optional<std::string> engine_track_impl::composer()
{
    return storage_->get_meta_data(id(), metadata_str_type::composer);
}

void engine_track_impl::set_composer(stdx::optional<std::string> composer)
{
    storage_->set_meta_data(id(), metadata_str_type::composer, composer);
}

database engine_track_impl::db()
{
    return database{std::make_shared<engine_database_impl>(storage_)};
}

std::vector<crate> engine_track_impl::containing_crates()
{
    std::vector<crate> results;
    storage_->db << "SELECT crateId FROM CrateTrackList WHERE trackId = ?"
                 << id() >>
        [&](int64_t id)
    {
        results.push_back(
            crate{std::make_shared<engine_crate_impl>(storage_, id)});
    };
    return results;
}

stdx::optional<milliseconds> engine_track_impl::duration()
{
    auto secs =
        storage_->get_track_column<stdx::optional<int64_t> >(id(), "length");
    if (secs)
    {
        return milliseconds{*secs * 1000};
    }
    return stdx::nullopt;
}

void engine_track_impl::set_duration(
    stdx::optional<std::chrono::milliseconds> duration)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    stdx::optional<int64_t> secs =
        duration ? stdx::make_optional(duration->count() / 1000)
                 : stdx::nullopt;
    storage_->set_track_column<stdx::optional<int64_t> >(id(), "length", secs);

    if (secs)
    {
        // Set metadata duration_mm_ss as "MM:SS"
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0');
        oss << (*secs / 60);
        oss << ":";
        oss << (*secs % 60);
        auto str = oss.str();
        storage_->set_meta_data(
            id(), metadata_str_type::duration_mm_ss, std::string{str});
    }
    else
    {
        storage_->set_meta_data(
            id(), metadata_str_type::duration_mm_ss, stdx::nullopt);
    }

    trans.commit();
}

std::string engine_track_impl::file_extension()
{
    auto rel_path = relative_path();
    return djinterop::util::get_file_extension(rel_path).value_or(
        std::string{});
}

std::string engine_track_impl::filename()
{
    auto rel_path = relative_path();
    return djinterop::util::get_filename(rel_path);
}

stdx::optional<std::string> engine_track_impl::genre()
{
    return storage_->get_meta_data(id(), metadata_str_type::genre);
}

void engine_track_impl::set_genre(stdx::optional<std::string> genre)
{
    storage_->set_meta_data(id(), metadata_str_type::genre, genre);
}

stdx::optional<hot_cue> engine_track_impl::hot_cue_at(int index)
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues[index]);
}

void engine_track_impl::set_hot_cue_at(int index, stdx::optional<hot_cue> cue)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues[index] = std::move(cue);
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

std::vector<stdx::optional<hot_cue> > engine_track_impl::hot_cues()
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues);
}

void engine_track_impl::set_hot_cues(std::vector<stdx::optional<hot_cue> > cues)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    // TODO (haslersn): The following can be optimized because in this case we
    // overwrite all hot_cues
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues = std::move(cues);
    if (quick_cues_d.hot_cues.size() < 8)
        quick_cues_d.hot_cues.resize(8);

    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

bool engine_track_impl::is_valid()
{
    bool valid = false;
    storage_->db << "SELECT COUNT(*) FROM Track WHERE id = ?" << id() >>
        [&](int count)
    {
        if (count == 1)
        {
            valid = true;
        }
        else if (count > 1)
        {
            throw track_database_inconsistency{
                "More than one track with the same ID", id()};
        }
    };
    return valid;
}

stdx::optional<musical_key> engine_track_impl::key()
{
    stdx::optional<musical_key> result;
    auto key_num =
        storage_->get_meta_data_integer(id(), metadata_int_type::musical_key);
    if (key_num)
    {
        result = static_cast<musical_key>(*key_num);
    }
    return result;
}

void engine_track_impl::set_key(stdx::optional<musical_key> key)
{
    stdx::optional<int64_t> key_num;
    if (key)
    {
        key_num = static_cast<int64_t>(*key);
    }

    djinterop::util::sqlite_transaction trans{storage_->db};
    auto track_d = get_track_data();
    track_d.key = key;
    set_track_data(track_d);
    storage_->set_meta_data_integer(
        id(), metadata_int_type::musical_key, key_num);
    trans.commit();
}

stdx::optional<system_clock::time_point> engine_track_impl::last_played_at()
{
    return djinterop::util::to_time_point(storage_->get_meta_data_integer(
        id(), metadata_int_type::last_played_ts));
}

void engine_track_impl::set_last_played_at(
    stdx::optional<system_clock::time_point> played_at)
{
    static stdx::optional<std::string> zero{"0"};
    static stdx::optional<std::string> one{"1"};
    storage_->set_meta_data(
        id(), metadata_str_type::ever_played, played_at ? one : zero);
    storage_->set_meta_data_integer(
        id(), metadata_int_type::last_played_ts,
        djinterop::util::to_timestamp(played_at));
    if (played_at)
    {
        // TODO (haslersn): Add entry to HistorylistTrackList
    }
    else
    {
        // TODO (haslersn): Should HistorylistTrackList now be cleared of this
        // track?
    }
}

stdx::optional<loop> engine_track_impl::loop_at(int index)
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops[index]);
}

void engine_track_impl::set_loop_at(int index, stdx::optional<loop> l)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    auto loops_d = get_loops_data();
    loops_d.loops[index] = std::move(l);
    set_loops_data(std::move(loops_d));
    trans.commit();
}

std::vector<stdx::optional<loop> > engine_track_impl::loops()
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops);
}

void engine_track_impl::set_loops(std::vector<stdx::optional<loop> > loops)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    loops_data loops_d;
    loops_d.loops = std::move(loops);
    if (loops_d.loops.size() < 8)
        loops_d.loops.resize(8);

    set_loops_data(std::move(loops_d));
    trans.commit();
}

stdx::optional<double> engine_track_impl::main_cue()
{
    auto cue = get_quick_cues_data().adjusted_main_cue;
    if (cue == 0)
        return stdx::nullopt;

    return stdx::make_optional(cue);
}

void engine_track_impl::set_main_cue(stdx::optional<double> sample_offset)
{
    djinterop::util::sqlite_transaction trans{storage_->db};
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.adjusted_main_cue = sample_offset.value_or(0);
    quick_cues_d.default_main_cue = sample_offset.value_or(0);
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

stdx::optional<std::string> engine_track_impl::publisher()
{
    return storage_->get_meta_data(id(), metadata_str_type::publisher);
}

void engine_track_impl::set_publisher(stdx::optional<std::string> publisher)
{
    storage_->set_meta_data(id(), metadata_str_type::publisher, publisher);
}

stdx::optional<int32_t> engine_track_impl::rating()
{
    auto result =
        storage_->get_meta_data_integer(id(), metadata_int_type::rating);
    return result ? stdx::make_optional(static_cast<int32_t>(*result))
                  : stdx::nullopt;
}

void engine_track_impl::set_rating(stdx::optional<int32_t> rating)
{
    auto clamped_rating = rating ? stdx::make_optional(static_cast<int64_t>(
                                       std::clamp(*rating, 0, 100)))
                                 : stdx::nullopt;
    storage_->set_meta_data_integer(
        id(), metadata_int_type::rating, clamped_rating);
}

std::string engine_track_impl::relative_path()
{
    return storage_->get_track_column<std::string>(id(), "path");
}

void engine_track_impl::set_relative_path(std::string relative_path)
{
    // TODO (haslersn): Should we check the path?
    storage_->set_track_column(id(), "path", std::string{relative_path});
    auto filename = djinterop::util::get_filename(relative_path);
    storage_->set_track_column(id(), "filename", std::string{filename});
    auto extension = djinterop::util::get_file_extension(filename);
    storage_->set_meta_data(id(), metadata_str_type::file_extension, extension);
}

stdx::optional<unsigned long long> engine_track_impl::sample_count()
{
    return djinterop::util::optional_static_cast<unsigned long long>(
        get_track_data().sample_count);
}

void engine_track_impl::set_sample_count(
    stdx::optional<unsigned long long> sample_count)
{
    djinterop::util::sqlite_transaction trans{storage_->db};

    // read old data
    auto track_d = get_track_data();
    auto beat_d = get_beat_data();
    auto high_res_waveform_d = get_high_res_waveform_data();
    auto overview_waveform_d = get_overview_waveform_data();

    stdx::optional<int64_t> secs;
    if (sample_count && track_d.sample_rate && *track_d.sample_rate != 0)
    {
        secs = *sample_count / static_cast<int64_t>(*track_d.sample_rate);
    }
    storage_->set_track_column(id(), "lengthCalculated", secs);

    auto sample_count_or_zero = sample_count.value_or(0);
    auto sample_rate_or_zero = track_d.sample_rate.value_or(0);

    // write new data
    track_d.sample_count =
        djinterop::util::optional_static_cast<int64_t>(sample_count);
    beat_d.sample_count =
        djinterop::util::optional_static_cast<double>(sample_count);
    set_beat_data(std::move(beat_d));
    set_track_data(std::move(track_d));

    if (!overview_waveform_d.waveform.empty())
    {
        // The overview waveform has a varying number of samples per entry, as
        // the number of entries is always fixed.
        auto extents = util::calculate_overview_waveform_extents(
            sample_count_or_zero, sample_rate_or_zero);
        overview_waveform_d.samples_per_entry = extents.samples_per_entry;
        set_overview_waveform_data(std::move(overview_waveform_d));
    }

    trans.commit();
}

stdx::optional<double> engine_track_impl::sample_rate()
{
    return get_track_data().sample_rate;
}

void engine_track_impl::set_sample_rate(stdx::optional<double> sample_rate)
{
    djinterop::util::sqlite_transaction trans{storage_->db};

    // read old data
    auto track_d = get_track_data();
    auto beat_d = get_beat_data();
    auto high_res_waveform_d = get_high_res_waveform_data();
    auto overview_waveform_d = get_overview_waveform_data();

    stdx::optional<int64_t> secs;
    if (track_d.sample_count && sample_rate && *sample_rate != 0)
    {
        secs = *track_d.sample_count / static_cast<int64_t>(*sample_rate);
    }
    storage_->set_track_column(id(), "lengthCalculated", secs);

    auto sample_count_or_zero = track_d.sample_count.value_or(0);
    auto sample_rate_or_zero = sample_rate.value_or(0);

    // write new data
    track_d.sample_rate = sample_rate;
    beat_d.sample_rate = sample_rate;
    set_beat_data(std::move(beat_d));
    set_track_data(std::move(track_d));

    if (!high_res_waveform_d.waveform.empty())
    {
        // The high-resolution waveform has a required number of samples per
        // entry that is dependent on the sample rate.  If the sample rate is
        // genuinely changed using this method, note that the waveform is likely
        // to need to be updated as well.
        auto extents = util::calculate_high_resolution_waveform_extents(
            sample_count_or_zero, sample_rate_or_zero);
        high_res_waveform_d.samples_per_entry = extents.samples_per_entry;
        set_high_res_waveform_data(std::move(high_res_waveform_d));
    }

    if (!overview_waveform_d.waveform.empty())
    {
        // The overview waveform has a varying number of samples per entry, as
        // the number of entries is always fixed.
        auto extents = util::calculate_overview_waveform_extents(
            sample_count_or_zero, sample_rate_or_zero);
        overview_waveform_d.samples_per_entry = extents.samples_per_entry;
        set_overview_waveform_data(std::move(overview_waveform_d));
    }

    trans.commit();
}

stdx::optional<std::string> engine_track_impl::title()
{
    return storage_->get_meta_data(id(), metadata_str_type::title);
}

void engine_track_impl::set_title(stdx::optional<std::string> title)
{
    storage_->set_meta_data(id(), metadata_str_type::title, title);
}

stdx::optional<int32_t> engine_track_impl::track_number()
{
    return storage_->get_track_column<stdx::optional<int32_t> >(
        id(), "playOrder");
}

void engine_track_impl::set_track_number(stdx::optional<int32_t> track_number)
{
    storage_->set_track_column(id(), "playOrder", track_number);
}

std::vector<waveform_entry> engine_track_impl::waveform()
{
    auto high_res_waveform_d = get_high_res_waveform_data();
    return std::move(high_res_waveform_d.waveform);
}

void engine_track_impl::set_waveform(std::vector<waveform_entry> waveform)
{
    djinterop::util::sqlite_transaction trans{storage_->db};

    overview_waveform_data overview_waveform_d;
    high_res_waveform_data high_res_waveform_d;

    if (!waveform.empty())
    {
        auto track_data = get_track_data();
        auto sample_count = track_data.sample_count.value_or(0);
        auto sample_rate = track_data.sample_rate.value_or(0);

        // Calculate an overview waveform automatically.
        // Note that the overview waveform always has 1024 entries in it.
        auto overview_extents = util::calculate_overview_waveform_extents(
            sample_count, sample_rate);
        overview_waveform_d.samples_per_entry =
            overview_extents.samples_per_entry;
        overview_waveform_d.waveform.reserve(overview_extents.size);
        for (unsigned long long i = 0; i < overview_extents.size; ++i)
        {
            auto entry = waveform
                [waveform.size() * (2 * i + 1) / (2 * overview_extents.size)];
            overview_waveform_d.waveform.push_back(entry);
        }

        // Make the assumption that the client has respected the required number
        // of samples per entry when constructing the waveform.
        auto high_res_extents =
            util::calculate_high_resolution_waveform_extents(
                sample_count, sample_rate);
        high_res_waveform_d.samples_per_entry =
            high_res_extents.samples_per_entry;
        high_res_waveform_d.waveform = std::move(waveform);
    }

    set_overview_waveform_data(std::move(overview_waveform_d));
    set_high_res_waveform_data(std::move(high_res_waveform_d));

    trans.commit();
}

stdx::optional<int> engine_track_impl::year()
{
    return storage_->get_track_column<stdx::optional<int> >(id(), "year");
}

void engine_track_impl::set_year(stdx::optional<int> year)
{
    storage_->set_track_column(id(), "year", year);
}

track create_track(
    std::shared_ptr<engine_storage> storage, const track_snapshot& snapshot)
{
    if (!snapshot.relative_path)
    {
        throw invalid_track_snapshot{
            "Snapshot does not contain a populated `relative_path` field, "
            "which is required to create a track"};
    }

    auto length_fields = to_length_fields(
        snapshot.duration, snapshot.sample_count, snapshot.sample_rate);
    auto bpm_fields =
        to_bpm_fields(snapshot.bpm, snapshot.sample_rate, snapshot.beatgrid);
    auto filename = djinterop::util::get_filename(*snapshot.relative_path);
    auto extension = djinterop::util::get_file_extension(filename);
    auto track_number =
        snapshot.track_number
            ? stdx::make_optional(static_cast<int64_t>(*snapshot.track_number))
            : stdx::nullopt;
    auto year = snapshot.year
                    ? stdx::make_optional(static_cast<int64_t>(*snapshot.year))
                    : stdx::nullopt;
    auto timestamp_fields = to_timestamp_fields(snapshot.last_played_at);
    auto key_num = to_key_num(snapshot.key);
    auto clamped_rating = snapshot.rating
                              ? stdx::make_optional(static_cast<int64_t>(
                                    std::clamp(*snapshot.rating, 0, 100)))
                              : stdx::nullopt;
    stdx::optional<int64_t> last_play_hash;
    auto track_data = to_track_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.average_loudness,
        snapshot.key);
    auto overview_waveform_data = to_overview_waveform_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.waveform);
    auto high_res_waveform_data = to_high_res_waveform_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.waveform);
    auto beat_data = to_beat_data(
        snapshot.sample_count, snapshot.sample_rate, snapshot.beatgrid);
    auto cues_data = to_cues_data(snapshot.hot_cues, snapshot.main_cue);
    auto loops_data = to_loops_data(snapshot.loops);

    djinterop::util::sqlite_transaction trans{storage->db};

    // Firstly, create the `Track` table entry.
    auto id = storage->create_track(
        track_number, length_fields.length, length_fields.length_calculated,
        bpm_fields.bpm, year, snapshot.relative_path, filename,
        djinterop::util::optional_static_cast<int64_t>(snapshot.bitrate),
        bpm_fields.bpm_analyzed, default_track_type, default_is_external_track,
        default_uuid_of_external_database,
        default_id_track_in_external_database, no_album_art_id,
        djinterop::util::optional_static_cast<int64_t>(snapshot.file_bytes),
        default_pdb_import_key, default_uri, default_is_beatgrid_locked);

    // Set string-based metadata.
    storage->set_meta_data(
        id, snapshot.title, snapshot.artist, snapshot.album, snapshot.genre,
        snapshot.comment, snapshot.publisher, snapshot.composer,
        length_fields.length_mm_ss, timestamp_fields.ever_played, extension);

    // Set integer-based metadata.
    storage->set_meta_data_integer(
        id, key_num, clamped_rating, timestamp_fields.last_played_at_ts,
        timestamp_fields.last_modified_at_ts,
        timestamp_fields.last_accessed_at_ts, last_play_hash);

    // Set performance data, if any.
    // Note that empty sets of hot cues or loops are written as 8 empty entries,
    // so performance data is in fact always written.
    auto has_perf_data =
        snapshot.sample_count || snapshot.sample_rate ||
        snapshot.average_loudness || !snapshot.beatgrid.empty() ||
        !cues_data.hot_cues.empty() || !loops_data.loops.empty();
    auto is_analysed = 1;
    if (has_perf_data)
    {
        storage->set_performance_data(
            id, is_analysed, default_is_rendered, track_data,
            high_res_waveform_data, overview_waveform_data, beat_data,
            cues_data, loops_data, default_has_serato_values,
            default_has_rekordbox_values, default_has_traktor_values);
    }

    track tr{std::make_shared<engine_track_impl>(storage, id)};

    trans.commit();

    return tr;
}

}  // namespace djinterop::engine::v1
