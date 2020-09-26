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

#include <cmath>
#include <iomanip>
#include <sstream>

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_crate_impl.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_track_impl.hpp>
#include <djinterop/enginelibrary/el_transaction_guard_impl.hpp>
#include <djinterop/util.hpp>

namespace djinterop
{
namespace enginelibrary
{
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace
{
stdx::optional<system_clock::time_point> to_time_point(
    stdx::optional<int64_t> timestamp)
{
    stdx::optional<system_clock::time_point> result;
    if (timestamp)
    {
        result = system_clock::time_point{seconds(*timestamp)};
    }
    return result;
}

stdx::optional<int64_t> to_timestamp(
    stdx::optional<system_clock::time_point> time)
{
    stdx::optional<int64_t> result;
    if (time)
    {
        result = duration_cast<seconds>(time->time_since_epoch()).count();
    }
    return result;
}

/// Calculate the quantisation number for waveforms, given a sample rate.
///
/// A few numbers written to the waveform performance data are rounded
/// to multiples of a particular "quantisation number", that is equal to
/// the sample rate divided by 105, and then rounded to the nearest
/// multiple of two.
int64_t quantisation_number(int64_t sample_rate)
{
    return (sample_rate / 210) * 2;
}

/// Calculate the samples-per-entry in an overview waveform.
///
/// An overview waveform always has 1024 entries, and the number of samples
/// that each one represents must be calculated from the true sample count by
/// rounding the number of samples to the quantisation number first.
int64_t calculate_overview_waveform_samples_per_entry(
    int64_t sample_rate, int64_t sample_count)
{
    auto qn = quantisation_number(sample_rate);
    return ((sample_count / qn) * qn) / 1024;
}

}  // namespace

el_track_impl::el_track_impl(std::shared_ptr<el_storage> storage, int64_t id) :
    track_impl{id}, storage_{std::move(storage)}
{
}

stdx::optional<std::string> el_track_impl::get_metadata_str(
    metadata_str_type type)
{
    stdx::optional<std::string> result;
    storage_->db << "SELECT text FROM MetaData WHERE id = ? AND "
                    "type = ? AND text IS NOT NULL"
                 << id() << static_cast<int64_t>(type) >>
        [&](std::string text) {
            if (!result)
            {
                result = std::move(text);
            }
            else
            {
                throw track_database_inconsistency{
                    "More than one MetaData entry of the same type for the "
                    "same track",
                    id()};
            }
        };
    return result;
}

void el_track_impl::set_metadata_str(
    metadata_str_type type, stdx::optional<std::string> content)
{
    if (content)
    {
        set_metadata_str(type, std::string{*content});
    }
    else
    {
        storage_->db
            << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)" << id()
            << static_cast<int64_t>(type) << nullptr;
    }
}

void el_track_impl::set_metadata_str(
    metadata_str_type type, const std::string& content)
{
    storage_->db << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)"
                 << id() << static_cast<int64_t>(type) << content;
}

stdx::optional<int64_t> el_track_impl::get_metadata_int(metadata_int_type type)
{
    stdx::optional<int64_t> result;
    storage_->db << "SELECT value FROM MetaDataInteger WHERE id = "
                    "? AND type = ? AND value IS NOT NULL"
                 << id() << static_cast<int64_t>(type) >>
        [&](int64_t value) {
            if (!result)
            {
                result = value;
            }
            else
            {
                throw track_database_inconsistency{
                    "More than one MetaDataInteger entry of the same type "
                    "for the same track",
                    id()};
            }
        };
    return result;
}

void el_track_impl::set_metadata_int(
    metadata_int_type type, stdx::optional<int64_t> content)
{
    storage_->db
        << "REPLACE INTO MetaDataInteger (id, type, value) VALUES (?, ?, ?)"
        << id() << static_cast<int64_t>(type) << content;
}

beat_data el_track_impl::get_beat_data()
{
    return get_perfdata<beat_data>("beatData");
}

void el_track_impl::set_beat_data(beat_data data)
{
    set_perfdata("beatData", data);
}

high_res_waveform_data el_track_impl::get_high_res_waveform_data()
{
    return get_perfdata<high_res_waveform_data>("highResolutionWaveFormData");
}

void el_track_impl::set_high_res_waveform_data(high_res_waveform_data data)
{
    set_perfdata("highResolutionWaveFormData", data);
}

loops_data el_track_impl::get_loops_data()
{
    return get_perfdata<loops_data>("loops");
}

void el_track_impl::set_loops_data(loops_data data)
{
    set_perfdata("loops", data);
}

overview_waveform_data el_track_impl::get_overview_waveform_data()
{
    return get_perfdata<overview_waveform_data>("overviewWaveFormData");
}

void el_track_impl::set_overview_waveform_data(overview_waveform_data data)
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
    set_perfdata("overviewWaveFormData", data);
}

quick_cues_data el_track_impl::get_quick_cues_data()
{
    return get_perfdata<quick_cues_data>("quickCues");
}

void el_track_impl::set_quick_cues_data(quick_cues_data data)
{
    set_perfdata("quickCues", data);
}

track_data el_track_impl::get_track_data()
{
    return get_perfdata<track_data>("trackData");
}

void el_track_impl::set_track_data(track_data data)
{
    set_perfdata("trackData", data);
}

std::vector<beatgrid_marker> el_track_impl::adjusted_beatgrid()
{
    auto beat_d = get_beat_data();
    return std::move(beat_d.adjusted_beatgrid);
}

void el_track_impl::set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    el_transaction_guard_impl trans{storage_};
    auto beat_d = get_beat_data();
    beat_d.adjusted_beatgrid = std::move(beatgrid);
    set_beat_data(std::move(beat_d));
    trans.commit();
}

double el_track_impl::adjusted_main_cue()
{
    return get_quick_cues_data().adjusted_main_cue;
}

void el_track_impl::set_adjusted_main_cue(double sample_offset)
{
    el_transaction_guard_impl trans{storage_};
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.adjusted_main_cue = sample_offset;
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

stdx::optional<std::string> el_track_impl::album()
{
    return get_metadata_str(metadata_str_type::album);
}

void el_track_impl::set_album(stdx::optional<std::string> album)
{
    set_metadata_str(metadata_str_type::album, album);
}

stdx::optional<int64_t> el_track_impl::album_art_id()
{
    int64_t cell = get_cell<int64_t>("idAlbumArt");
    stdx::optional<int64_t> album_art_id;
    if (cell < 1)
    {
        // TODO (haslersn): Throw something.
    }
    else if (cell > 1)
    {
        album_art_id = cell;
    }
    return album_art_id;
}

void el_track_impl::set_album_art_id(stdx::optional<int64_t> album_art_id)
{
    if (album_art_id && *album_art_id <= 1)
    {
        // TODO (haslersn): Throw something.
    }
    set_cell("idAlbumArt", album_art_id.value_or(1));
    // 1 is the magic number for "no album art"
}

stdx::optional<std::string> el_track_impl::artist()
{
    return get_metadata_str(metadata_str_type::artist);
}

void el_track_impl::set_artist(stdx::optional<std::string> artist)
{
    set_metadata_str(metadata_str_type::artist, artist);
}

stdx::optional<double> el_track_impl::average_loudness()
{
    return get_track_data().average_loudness;
}

void el_track_impl::set_average_loudness(
    stdx::optional<double> average_loudness)
{
    el_transaction_guard_impl trans{storage_};
    auto track_d = get_track_data();

    // Zero average loudness is interpreted as no average loudness.
    track_d.average_loudness =
        average_loudness == 0 ? stdx::nullopt : average_loudness;

    set_track_data(track_d);
    trans.commit();
}

stdx::optional<int64_t> el_track_impl::bitrate()
{
    return get_cell<stdx::optional<int64_t> >("bitrate");
}

void el_track_impl::set_bitrate(stdx::optional<int64_t> bitrate)
{
    set_cell("bitrate", bitrate);
}

stdx::optional<double> el_track_impl::bpm()
{
    return get_cell<stdx::optional<double> >("bpmAnalyzed");
}

void el_track_impl::set_bpm(stdx::optional<double> bpm)
{
    set_cell("bpmAnalyzed", bpm);
    stdx::optional<int64_t> ceiled_bpm;
    if (bpm)
    {
        ceiled_bpm = static_cast<int64_t>(std::ceil(*bpm));
    }
    set_cell("bpm", ceiled_bpm);
}

stdx::optional<std::string> el_track_impl::comment()
{
    return get_metadata_str(metadata_str_type::comment);
}

void el_track_impl::set_comment(stdx::optional<std::string> comment)
{
    set_metadata_str(metadata_str_type::comment, comment);
}

stdx::optional<std::string> el_track_impl::composer()
{
    return get_metadata_str(metadata_str_type::composer);
}

void el_track_impl::set_composer(stdx::optional<std::string> composer)
{
    set_metadata_str(metadata_str_type::composer, composer);
}

database el_track_impl::db()
{
    return database{std::make_shared<el_database_impl>(storage_)};
}

std::vector<crate> el_track_impl::containing_crates()
{
    std::vector<crate> results;
    storage_->db << "SELECT crateId FROM CrateTrackList WHERE trackId = ?"
                 << id() >>
        [&](int64_t id) {
            results.push_back(
                crate{std::make_shared<el_crate_impl>(storage_, id)});
        };
    return results;
}

std::vector<beatgrid_marker> el_track_impl::default_beatgrid()
{
    auto beat_d = get_beat_data();
    return std::move(beat_d.default_beatgrid);
}

void el_track_impl::set_default_beatgrid(std::vector<beatgrid_marker> beatgrid)
{
    el_transaction_guard_impl trans{storage_};
    auto beat_d = get_beat_data();
    beat_d.default_beatgrid = std::move(beatgrid);
    set_beat_data(std::move(beat_d));
    trans.commit();
}

double el_track_impl::default_main_cue()
{
    return get_quick_cues_data().default_main_cue;
}

void el_track_impl::set_default_main_cue(double sample_offset)
{
    el_transaction_guard_impl trans{storage_};
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.default_main_cue = sample_offset;
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

stdx::optional<milliseconds> el_track_impl::duration()
{
    auto smp = sampling();
    if (smp)
    {
        double secs = smp->sample_count / smp->sample_rate;
        return milliseconds{static_cast<int64_t>(1000 * secs)};
    }
    auto secs = get_cell<stdx::optional<int64_t> >("length");
    if (secs)
    {
        return milliseconds{*secs * 1000};
    }
    return stdx::nullopt;
}

std::string el_track_impl::file_extension()
{
    auto rel_path = relative_path();
    return get_file_extension(rel_path).value_or(std::string{});
}

std::string el_track_impl::filename()
{
    auto rel_path = relative_path();
    return get_filename(rel_path);
}

stdx::optional<std::string> el_track_impl::genre()
{
    return get_metadata_str(metadata_str_type::genre);
}

void el_track_impl::set_genre(stdx::optional<std::string> genre)
{
    set_metadata_str(metadata_str_type::genre, genre);
}

stdx::optional<hot_cue> el_track_impl::hot_cue_at(int32_t index)
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues[index]);
}

void el_track_impl::set_hot_cue_at(int32_t index, stdx::optional<hot_cue> cue)
{
    el_transaction_guard_impl trans{storage_};
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues[index] = std::move(cue);
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

std::array<stdx::optional<hot_cue>, 8> el_track_impl::hot_cues()
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues);
}

void el_track_impl::set_hot_cues(std::array<stdx::optional<hot_cue>, 8> cues)
{
    el_transaction_guard_impl trans{storage_};
    // TODO (haslersn): The following can be optimized because in this case we
    // overwrite all hot_cues
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues = std::move(cues);
    set_quick_cues_data(std::move(quick_cues_d));
    trans.commit();
}

stdx::optional<track_import_info> el_track_impl::import_info()
{
    if (get_cell<int64_t>("isExternalTrack") == 0)
    {
        return stdx::nullopt;
    }
    return track_import_info{
        get_cell<std::string>("uuidOfExternalDatabase"),
        get_cell<int64_t>("idTrackInExternalDatabase")};
    // TODO (haslersn): How should we handle cells that unexpectedly don't
    // contain integral values?
}

void el_track_impl::set_import_info(
    const stdx::optional<track_import_info>& import_info)
{
    if (import_info)
    {
        set_cell("isExternalTrack", 1);
        set_cell("uuidOfExternalDatabase", import_info->external_db_uuid);
        set_cell("idTrackInExternalDatabase", import_info->external_track_id);
    }
    else
    {
        set_cell("isExternalTrack", 0);
        set_cell("uuidOfExternalDatabase", nullptr);
        set_cell("idTrackInExternalDatabase", nullptr);
    }
}

bool el_track_impl::is_valid()
{
    bool valid = false;
    storage_->db << "SELECT COUNT(*) FROM Track WHERE id = ?" << id() >>
        [&](int count) {
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

stdx::optional<musical_key> el_track_impl::key()
{
    stdx::optional<musical_key> result;
    auto key_num = get_metadata_int(metadata_int_type::musical_key);
    if (key_num)
    {
        result = static_cast<musical_key>(*key_num);
    }
    return result;
}

void el_track_impl::set_key(stdx::optional<musical_key> key)
{
    stdx::optional<int64_t> key_num;
    if (key)
    {
        key_num = static_cast<int64_t>(*key);
    }

    el_transaction_guard_impl trans{storage_};
    auto track_d = get_track_data();
    track_d.key = key;
    set_track_data(track_d);
    set_metadata_int(metadata_int_type::musical_key, key_num);
    trans.commit();
}

stdx::optional<system_clock::time_point> el_track_impl::last_accessed_at()

{
    // TODO (haslersn): Is there a difference between
    // `el_track_impl::last_accessed_at()` and
    // `el_track_impl::last_played_at()`, except for the ceiling of the
    // timestamp?
    return to_time_point(get_metadata_int(metadata_int_type::last_accessed_ts));
}

void el_track_impl::set_last_accessed_at(
    stdx::optional<system_clock::time_point> accessed_at)
{
    if (accessed_at)
    {
        // Field is always ceiled to the midnight at the end of the day the
        // track is played, it seems.
        // TODO (haslersn): ^ Why "played" and not "accessed"?
        // TODO (haslersn): Shouldn't we just set the unceiled time? This would
        // leave the decision whether to ceil it to the library user. Also, it
        // would make `el_track_impl::last_accessed_at()` consistent with the
        // value that has been set using this method.
        auto timestamp = *to_timestamp(accessed_at);
        auto secs_per_day = 86400;
        timestamp += secs_per_day - 1;
        timestamp -= timestamp % secs_per_day;
        set_metadata_int(metadata_int_type::last_accessed_ts, timestamp);
    }
    else
    {
        set_metadata_int(metadata_int_type::last_accessed_ts, stdx::nullopt);
    }
}

stdx::optional<system_clock::time_point> el_track_impl::last_modified_at()

{
    return to_time_point(get_metadata_int(metadata_int_type::last_modified_ts));
}

void el_track_impl::set_last_modified_at(
    stdx::optional<system_clock::time_point> modified_at)
{
    set_metadata_int(
        metadata_int_type::last_modified_ts, to_timestamp(modified_at));
}

stdx::optional<system_clock::time_point> el_track_impl::last_played_at()

{
    return to_time_point(get_metadata_int(metadata_int_type::last_played_ts));
}

void el_track_impl::set_last_played_at(
    stdx::optional<system_clock::time_point> played_at)
{
    static stdx::optional<std::string> zero{"0"};
    static stdx::optional<std::string> one{"1"};
    set_metadata_str(metadata_str_type::ever_played, played_at ? one : zero);
    set_metadata_int(
        metadata_int_type::last_played_ts, to_timestamp(played_at));
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

stdx::optional<loop> el_track_impl::loop_at(int32_t index)
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops[index]);
}

void el_track_impl::set_loop_at(int32_t index, stdx::optional<loop> l)
{
    el_transaction_guard_impl trans{storage_};
    auto loops_d = get_loops_data();
    loops_d.loops[index] = std::move(l);
    set_loops_data(std::move(loops_d));
    trans.commit();
}

std::array<stdx::optional<loop>, 8> el_track_impl::loops()
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops);
}

void el_track_impl::set_loops(std::array<stdx::optional<loop>, 8> cues)
{
    el_transaction_guard_impl trans{storage_};
    loops_data loops_d;
    loops_d.loops = std::move(cues);
    set_loops_data(std::move(loops_d));
    trans.commit();
}

std::vector<waveform_entry> el_track_impl::overview_waveform()
{
    auto overview_waveform_d = get_overview_waveform_data();
    return std::move(overview_waveform_d.waveform);
}

stdx::optional<std::string> el_track_impl::publisher()
{
    return get_metadata_str(metadata_str_type::publisher);
}

void el_track_impl::set_publisher(stdx::optional<std::string> publisher)
{
    set_metadata_str(metadata_str_type::publisher, publisher);
}

int64_t el_track_impl::required_waveform_samples_per_entry()
{
    auto smp = sampling();
    if (!smp)
    {
        return 0;
    }
    if (smp->sample_rate <= 0)
    {
        throw track_database_inconsistency{
            "Track has non-positive sample rate", id()};
    }

    // In high-resolution waveforms, the samples-per-entry is the same as
    // the quantisation number.
    return quantisation_number(smp->sample_rate);
}

std::string el_track_impl::relative_path()
{
    return get_cell<std::string>("path");
}

void el_track_impl::set_relative_path(std::string relative_path)
{
    // TODO (haslersn): Should we check the path?
    set_cell("path", std::string{relative_path});
    auto filename = get_filename(relative_path);
    set_cell("filename", std::string{filename});
    auto extension = get_file_extension(filename);
    set_metadata_str(metadata_str_type::file_extension, extension);
}

stdx::optional<sampling_info> el_track_impl::sampling()
{
    return get_track_data().sampling;
}

void el_track_impl::set_sampling(stdx::optional<sampling_info> sampling)
{
    el_transaction_guard_impl trans{storage_};

    stdx::optional<int64_t> secs;
    if (sampling)
    {
        secs = static_cast<int64_t>(
            sampling->sample_count / sampling->sample_rate);

        // Set metadata duration_mm_ss as "MM:SS"
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0');
        oss << (*secs / 60);
        oss << ":";
        oss << (*secs % 60);
        auto str = oss.str();
        set_metadata_str(metadata_str_type::duration_mm_ss, std::string{str});
    }
    else
    {
        set_metadata_str(metadata_str_type::duration_mm_ss, stdx::nullopt);
    }
    set_cell("length", secs);
    set_cell("lengthCalculated", secs);

    // read old data
    auto track_d = get_track_data();
    auto beat_d = get_beat_data();
    auto high_res_waveform_d = get_high_res_waveform_data();
    auto overview_waveform_d = get_overview_waveform_data();

    // write new data
    track_d.sampling = sampling;
    beat_d.sampling = sampling;
    set_beat_data(std::move(beat_d));
    set_track_data(std::move(track_d));

    int64_t sample_rate = sampling ? sampling->sample_rate : 0;
    int64_t sample_count = sampling ? sampling->sample_count : 0;

    if (!high_res_waveform_d.waveform.empty())
    {
        // The high-resolution waveform has a required number of samples per
        // entry that is dependent on the sample rate.  If the sample rate is
        // genuinely changed using this method, note that the waveform is likely
        // to need to be updated as well afterwards.
        high_res_waveform_d.samples_per_entry =
            quantisation_number(sample_rate);
        set_high_res_waveform_data(std::move(high_res_waveform_d));
    }

    if (!overview_waveform_d.waveform.empty())
    {
        // The overview waveform has a varying number of samples per entry, as
        // the number of entries is always fixed.
        overview_waveform_d.samples_per_entry =
            calculate_overview_waveform_samples_per_entry(
                sample_rate, sample_count);
        set_overview_waveform_data(std::move(overview_waveform_d));
    }

    trans.commit();
}

stdx::optional<std::string> el_track_impl::title()
{
    return get_metadata_str(metadata_str_type::title);
}

void el_track_impl::set_title(stdx::optional<std::string> title)
{
    set_metadata_str(metadata_str_type::title, title);
}

stdx::optional<int32_t> el_track_impl::track_number()
{
    return get_cell<stdx::optional<int32_t> >("playOrder");
}

void el_track_impl::set_track_number(stdx::optional<int32_t> track_number)
{
    set_cell("playOrder", track_number);
}

std::vector<waveform_entry> el_track_impl::waveform()
{
    auto high_res_waveform_d = get_high_res_waveform_data();
    return std::move(high_res_waveform_d.waveform);
}

void el_track_impl::set_waveform(std::vector<waveform_entry> waveform)
{
    el_transaction_guard_impl trans{storage_};

    overview_waveform_data overview_waveform_d;
    high_res_waveform_data high_res_waveform_d;

    if (!waveform.empty())
    {
        auto smp = sampling();
        int64_t sample_count = smp ? smp->sample_count : 0;
        int64_t sample_rate = smp ? smp->sample_rate : 0;

        // Calculate an overview waveform automatically.
        // Note that the overview waveform always has 1024 entries in it.
        overview_waveform_d.samples_per_entry =
            calculate_overview_waveform_samples_per_entry(
                sample_rate, sample_count);
        overview_waveform_d.waveform.reserve(1024);
        for (int32_t i = 0; i < 1024; ++i)
        {
            auto entry = waveform[waveform.size() * (2 * i + 1) / 2048];
            overview_waveform_d.waveform.push_back(entry);
        }

        // Make the assumption that the client has respected the required number
        // of samples per entry when constructing the waveform.
        high_res_waveform_d.samples_per_entry =
            quantisation_number(sample_rate);
        high_res_waveform_d.waveform = std::move(waveform);
    }

    set_overview_waveform_data(std::move(overview_waveform_d));
    set_high_res_waveform_data(std::move(high_res_waveform_d));

    trans.commit();
}

stdx::optional<int32_t> el_track_impl::year()
{
    return get_cell<stdx::optional<int32_t> >("year");
}

void el_track_impl::set_year(stdx::optional<int32_t> year)
{
    set_cell("year", year);
}

}  // namespace enginelibrary
}  // namespace djinterop
