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

#include <djinterop/enginelibrary.hpp>

#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>

#include "enginelibrary/database_impl.hpp"
#include "enginelibrary/performance_data_format.hpp"
#include "enginelibrary/util.hpp"
#include "sqlite_modern_cpp.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

namespace djinterop
{
namespace enginelibrary
{
enum class metadata_str_type
{
    title = 1,
    artist = 2,
    album = 3,
    genre = 4,
    comment = 5,
    publisher = 6,
    composer = 7,
    duration_mm_ss = 10,
    ever_played = 12,
    file_extension = 13
};

enum class metadata_int_type
{
    last_played_ts = 1,
    last_modified_ts = 2,
    last_accessed_ts =
        3,  // NOTE: truncated to date on VFAT (see FAT "ACCDATE")
    musical_key = 4,
    hash = 10
};

namespace
{
boost::optional<system_clock::time_point> to_time_point(
    boost::optional<int64_t> timestamp)
{
    boost::optional<system_clock::time_point> result;
    if (timestamp)
    {
        result = system_clock::time_point{seconds(*timestamp)};
    }
    return result;
}

boost::optional<int64_t> to_timestamp(
    boost::optional<system_clock::time_point> time)
{
    boost::optional<int64_t> result;
    if (time)
    {
        result = duration_cast<seconds>(time->time_since_epoch()).count();
    }
    return result;
}
}  // namespace

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

struct track::impl
{
    impl(database db, int64_t id) : db_{std::move(db)}, id_{id} {}

    boost::optional<std::string> get_metadata_str(metadata_str_type type) const
    {
        boost::optional<std::string> result;
        db_.pimpl_->music_db_ << "SELECT text FROM MetaData WHERE id = ? AND "
                                 "type = ? AND text IS NOT NULL"
                              << id_ << static_cast<int64_t>(type) >>
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
                        id_};
                }
            };
        return result;
    }

    void set_metadata_str(
        metadata_str_type type,
        boost::optional<boost::string_view> content) const
    {
        if (content)
        {
            set_metadata_str(type, std::string{*content});
        }
        else
        {
            db_.pimpl_->music_db_
                << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)"
                << id_ << static_cast<int64_t>(type) << nullptr;
        }
    }

    void set_metadata_str(
        metadata_str_type type, const std::string& content) const
    {
        db_.pimpl_->music_db_
            << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)" << id_
            << static_cast<int64_t>(type) << content;
    }

    boost::optional<int64_t> get_metadata_int(metadata_int_type type) const
    {
        boost::optional<int64_t> result;
        db_.pimpl_->music_db_ << "SELECT value FROM MetaDataInteger WHERE id = "
                                 "? AND type = ? AND value IS NOT NULL"
                              << id_ << static_cast<int64_t>(type) >>
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
                        id_};
                }
            };
        return result;
    }

    void set_metadata_int(
        metadata_int_type type, boost::optional<int64_t> content) const
    {
        db_.pimpl_->music_db_
            << "REPLACE INTO MetaDataInteger (id, type, value) VALUES (?, ?, ?)"
            << id_ << static_cast<int64_t>(type) << content;
    }

    template <typename T>
    T get_cell(const char* column_name) const
    {
        boost::optional<T> result;
        db_.pimpl_->music_db_ << (std::string{"SELECT "} + column_name +
                                  " FROM Track WHERE id = ?")
                              << id_ >>
            [&](T cell) {
                if (!result)
                {
                    result = cell;
                }
                else
                {
                    throw track_database_inconsistency{
                        "More than one track with the same ID", id_};
                }
            };
        if (!result)
        {
            throw track_deleted{id_};
        }
        return *result;
    }

    template <typename T>
    void set_cell(const char* column_name, const T& content) const
    {
        db_.pimpl_->music_db_ << (std::string{"UPDATE Track SET "} +
                                  column_name + " = ? WHERE id = ?")
                              << content << id_;
    }

    template <typename T>
    T get_perfdata(const char* column_name) const
    {
        boost::optional<T> result;
        db_.pimpl_->perfdata_db_ << (std::string{"SELECT "} + column_name +
                                     " From PerformanceData WHERE id = ?")
                                 << id_ >>
            [&](const std::vector<char>& encoded_data) {
                if (!result)
                {
                    result = T::decode(encoded_data);
                }
                else
                {
                    throw track_database_inconsistency{
                        "More than one PerformanceData entry for the same "
                        "track",
                        id_};
                }
            };
        return result.value_or(T{});
    }

    // NOTE: No transaction
    template <typename T>
    void set_perfdata(const char* column_name, const T& content) const
    {
        bool found = false;
        db_.pimpl_->perfdata_db_
                << "SELECT COUNT(*) FROM PerformanceData WHERE id = ?" << id_ >>
            [&](int32_t count) {
                if (count == 1)
                {
                    found = true;
                }
                else if (count > 1)
                {
                    throw track_database_inconsistency{
                        "More than one PerformanceData entry for the same "
                        "track",
                        id_};
                }
            };

        if (!found)
        {
            db_.pimpl_->perfdata_db_
                << "INSERT INTO PerformanceData (id, isAnalyzed, isRendered, "
                   "trackData, highResolutionWaveFormData, "
                   "overviewWaveFormData, beatData, quickCues, loops, "
                   "hasSeratoValues) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                << id_                                //
                << 0.0                                // isAnalyzed
                << 0.0                                // isRendered
                << track_data{}.encode()              //
                << high_res_waveform_data{}.encode()  //
                << overview_waveform_data{}.encode()  //
                << beat_data{}.encode()               //
                << quick_cues_data{}.encode()         //
                << loops_data{}.encode()              //
                << 0.0;                               // hasSeratoValues

            if (db_.version() >= version_1_7_1)
            {
                db_.pimpl_->perfdata_db_
                    << "UPDATE PerformanceData SET hasRekordboxValues = 0 "
                       "WHERE id = ?"
                    << id_;
            }
        }

        db_.pimpl_->perfdata_db_
            << (std::string{"UPDATE PerformanceData SET "} + column_name +
                " = ? WHERE id = ?")
            << content.encode() << id_;
    }

    beat_data get_beat_data() const
    {
        return get_perfdata<beat_data>("beatData");
    }

    void set_beat_data(beat_data data) const { set_perfdata("beatData", data); }

    high_res_waveform_data get_high_res_waveform_data() const
    {
        return get_perfdata<high_res_waveform_data>(
            "highResolutionWaveFormData");
    }

    void set_high_res_waveform_data(high_res_waveform_data data) const
    {
        set_perfdata("highResolutionWaveFormData", data);
    }

    loops_data get_loops_data() const
    {
        return get_perfdata<loops_data>("loops");
    }

    void set_loops_data(loops_data data) const { set_perfdata("loops", data); }

    overview_waveform_data get_overview_waveform_data() const
    {
        return get_perfdata<overview_waveform_data>("overviewWaveFormData");
    }

    void set_overview_waveform_data(overview_waveform_data data) const
    {
        set_perfdata("overviewWaveFormData", data);
    }

    quick_cues_data get_quick_cues_data() const
    {
        return get_perfdata<quick_cues_data>("quickCues");
    }

    void set_quick_cues_data(quick_cues_data data) const
    {
        set_perfdata("quickCues", data);
    }

    track_data get_track_data() const
    {
        return get_perfdata<track_data>("trackData");
    }

    void set_track_data(track_data data) const
    {
        set_perfdata("trackData", data);
    }

    database db_;
    int64_t id_;
};

track::track(const track& other) noexcept = default;

track::~track() = default;

track& track::operator=(const track& other) noexcept = default;

std::vector<beatgrid_marker> track::adjusted_beatgrid() const
{
    auto beat_d = pimpl_->get_beat_data();
    return std::move(beat_d.adjusted_beatgrid);
}

void track::set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto beat_d = pimpl_->get_beat_data();
    beat_d.adjusted_beatgrid = std::move(beatgrid);
    pimpl_->set_beat_data(std::move(beat_d));
    db().pimpl_->perfdata_db_ << "COMMIT";
}

double track::adjusted_main_cue() const
{
    return pimpl_->get_quick_cues_data().adjusted_main_cue;
}

void track::set_adjusted_main_cue(double sample_offset) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    quick_cues_d.adjusted_main_cue = sample_offset;
    pimpl_->set_quick_cues_data(std::move(quick_cues_d));
    db().pimpl_->perfdata_db_ << "COMMIT";
}

boost::optional<std::string> track::album() const
{
    return pimpl_->get_metadata_str(metadata_str_type::album);
}

void track::set_album(boost::optional<boost::string_view> album) const
{
    pimpl_->set_metadata_str(metadata_str_type::album, album);
}

void track::set_album(boost::string_view album) const
{
    set_album(boost::make_optional(album));
}

boost::optional<int64_t> track::album_art_id() const
{
    int64_t cell = pimpl_->get_cell<int64_t>("idAlbumArt");
    boost::optional<int64_t> album_art_id;
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

void track::set_album_art_id(boost::optional<int64_t> album_art_id) const
{
    if (album_art_id && *album_art_id <= 1)
    {
        // TODO (haslersn): Throw something.
    }
    pimpl_->set_cell("idAlbumArt", album_art_id.value_or(1));
    // 1 is the magic number for "no album art"
}

void track::set_album_art_id(int64_t album_art_id) const
{
    set_album_art_id(boost::make_optional(album_art_id));
}

boost::optional<std::string> track::artist() const
{
    return pimpl_->get_metadata_str(metadata_str_type::artist);
}

void track::set_artist(boost::optional<boost::string_view> artist) const
{
    pimpl_->set_metadata_str(metadata_str_type::artist, artist);
}

void track::set_artist(boost::string_view artist) const
{
    set_artist(boost::make_optional(artist));
}

boost::optional<double> track::average_loudness() const
{
    return pimpl_->get_track_data().average_loudness;
}

void track::set_average_loudness(boost::optional<double> average_loudness) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto track_d = pimpl_->get_track_data();
    track_d.average_loudness = average_loudness;
    pimpl_->set_track_data(track_d);
    db().pimpl_->perfdata_db_ << "COMMIT";
}

void track::set_average_loudness(double average_loudness) const
{
    set_average_loudness(boost::make_optional(average_loudness));
}

boost::optional<int64_t> track::bitrate() const
{
    return pimpl_->get_cell<boost::optional<int64_t>>("bitrate");
}

void track::set_bitrate(boost::optional<int64_t> bitrate) const
{
    pimpl_->set_cell("bitrate", bitrate);
}

void track::set_bitrate(int64_t bitrate) const
{
    set_bitrate(boost::make_optional(bitrate));
}

boost::optional<double> track::bpm() const
{
    return pimpl_->get_cell<boost::optional<double>>("bpmAnalyzed");
}

void track::set_bpm(boost::optional<double> bpm) const
{
    pimpl_->set_cell("bpmAnalyzed", bpm);
    boost::optional<int64_t> ceiled_bpm;
    if (bpm)
    {
        ceiled_bpm = static_cast<int64_t>(std::ceil(*bpm));
    }
    pimpl_->set_cell("bpm", ceiled_bpm);
}

void track::set_bpm(double bpm) const
{
    set_bpm(boost::make_optional(bpm));
}

boost::optional<std::string> track::comment() const
{
    return pimpl_->get_metadata_str(metadata_str_type::comment);
}

void track::set_comment(boost::optional<boost::string_view> comment) const
{
    pimpl_->set_metadata_str(metadata_str_type::comment, comment);
}

void track::set_comment(boost::string_view comment) const
{
    set_comment(boost::make_optional(comment));
}

boost::optional<std::string> track::composer() const
{
    return pimpl_->get_metadata_str(metadata_str_type::composer);
}

void track::set_composer(boost::optional<boost::string_view> composer) const
{
    pimpl_->set_metadata_str(metadata_str_type::composer, composer);
}

void track::set_composer(boost::string_view composer) const
{
    set_composer(boost::make_optional(composer));
}

std::vector<crate> track::containing_crates() const
{
    std::vector<crate> results;
    db().pimpl_->music_db_
            << "SELECT crateId FROM CrateTrackList WHERE trackId = ?" << id() >>
        [&](int64_t id) {
            results.push_back(crate{db(), id});
        };
    return results;
}

database track::db() const
{
    return pimpl_->db_;
}

std::vector<beatgrid_marker> track::default_beatgrid() const
{
    auto beat_d = pimpl_->get_beat_data();
    return std::move(beat_d.default_beatgrid);
}

void track::set_default_beatgrid(std::vector<beatgrid_marker> beatgrid) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto beat_d = pimpl_->get_beat_data();
    beat_d.default_beatgrid = std::move(beatgrid);
    pimpl_->set_beat_data(std::move(beat_d));
    db().pimpl_->perfdata_db_ << "COMMIT";
}

double track::default_main_cue() const
{
    return pimpl_->get_quick_cues_data().default_main_cue;
}

void track::set_default_main_cue(double sample_offset) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    quick_cues_d.default_main_cue = sample_offset;
    pimpl_->set_quick_cues_data(std::move(quick_cues_d));
    db().pimpl_->perfdata_db_ << "COMMIT";
}

boost::optional<milliseconds> track::duration() const
{
    boost::optional<milliseconds> result;
    auto smp = sampling();
    if (smp)
    {
        double secs = smp->sample_count / smp->sample_rate;
        return milliseconds{static_cast<int64_t>(1000 * secs)};
    }
    auto secs = pimpl_->get_cell<boost::optional<int64_t>>("length");
    if (secs)
    {
        return milliseconds{*secs * 1000};
    }
    return boost::none;
}

std::string track::file_extension() const
{
    auto rel_path = relative_path();
    return get_file_extension(rel_path)
        .value_or(boost::string_view{})
        .to_string();
}

std::string track::filename() const
{
    auto rel_path = relative_path();
    return get_filename(rel_path).to_string();
}

boost::optional<std::string> track::genre() const
{
    return pimpl_->get_metadata_str(metadata_str_type::genre);
}

void track::set_genre(boost::optional<boost::string_view> genre) const
{
    pimpl_->set_metadata_str(metadata_str_type::genre, genre);
}

void track::set_genre(boost::string_view genre) const
{
    set_genre(boost::make_optional(genre));
}

boost::optional<hot_cue> track::hot_cue_at(int32_t index) const
{
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues[index]);
}

void track::set_hot_cue_at(int32_t index, boost::optional<hot_cue> cue) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    quick_cues_d.hot_cues[index] = std::move(cue);
    pimpl_->set_quick_cues_data(std::move(quick_cues_d));
    db().pimpl_->perfdata_db_ << "END";
}

void track::set_hot_cue_at(int32_t index, hot_cue cue) const
{
    set_hot_cue_at(index, boost::make_optional(cue));
}

std::array<boost::optional<hot_cue>, 8> track::hot_cues() const
{
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues);
}

void track::set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    // TODO (haslersn): The following can be optimized because in this case we
    // overwrite all hot_cues
    auto quick_cues_d = pimpl_->get_quick_cues_data();
    quick_cues_d.hot_cues = std::move(cues);
    pimpl_->set_quick_cues_data(std::move(quick_cues_d));
    db().pimpl_->perfdata_db_ << "END";
}

int64_t track::id() const
{
    return pimpl_->id_;
}

boost::optional<track_import_info> track::import_info() const
{
    if (pimpl_->get_cell<int64_t>("isExternalTrack") == 0)
    {
        return boost::none;
    }
    return track_import_info{
        pimpl_->get_cell<std::string>("uuidOfExternalDatabase"),
        pimpl_->get_cell<int64_t>("idTrackInExternalDatabase")};
    // TODO (haslersn): How should we handle cells that unexpectedly don't
    // contain integral values?
}

void track::set_import_info(
    const boost::optional<track_import_info>& import_info) const
{
    if (import_info)
    {
        set_import_info(*import_info);
    }
    else
    {
        pimpl_->set_cell("isExternalTrack", 0);
        pimpl_->set_cell("uuidOfExternalDatabase", nullptr);
        pimpl_->set_cell("idTrackInExternalDatabase", nullptr);
    }
}

void track::set_import_info(const track_import_info& import_info) const
{
    pimpl_->set_cell("isExternalTrack", 1);
    pimpl_->set_cell("uuidOfExternalDatabase", import_info.external_db_uuid());
    pimpl_->set_cell(
        "idTrackInExternalDatabase", import_info.external_track_id());
}

bool track::is_valid() const
{
    bool valid = false;
    db().pimpl_->music_db_ << "SELECT COUNT(*) FROM Track WHERE id = ?"
                           << id() >>
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

boost::optional<musical_key> track::key() const
{
    boost::optional<musical_key> result;
    auto key_num = pimpl_->get_metadata_int(metadata_int_type::musical_key);
    if (key_num)
    {
        result = static_cast<musical_key>(*key_num);
    }
    return result;
}

void track::set_key(boost::optional<musical_key> key) const
{
    boost::optional<int64_t> key_num;
    if (key)
    {
        key_num = static_cast<int64_t>(*key);
    }

    db().pimpl_->perfdata_db_ << "BEGIN";
    auto track_d = pimpl_->get_track_data();
    track_d.key = key;
    pimpl_->set_track_data(track_d);
    db().pimpl_->perfdata_db_ << "COMMIT";

    // TODO (haslersn): atomic?
    pimpl_->set_metadata_int(metadata_int_type::musical_key, key_num);
}

void track::set_key(musical_key key) const
{
    set_key(boost::make_optional(key));
}

boost::optional<system_clock::time_point> track::last_accessed_at() const
{
    // TODO (haslersn): Is there a difference between
    // `track::last_accessed_at()` and `track::last_played_at()`, except for the
    // ceiling of the timestamp?
    return to_time_point(
        pimpl_->get_metadata_int(metadata_int_type::last_accessed_ts));
}

void track::set_last_accessed_at(
    boost::optional<system_clock::time_point> accessed_at) const
{
    if (accessed_at)
    {
        // Field is always ceiled to the midnight at the end of the day the
        // track is played, it seems.
        // TODO (haslersn): ^ Why "played" and not "accessed"?
        // TODO (haslersn): Shouldn't we just set the unceiled time? This would
        // leave the decision whether to ceil it to the library user. Also, it
        // would make `track::last_accessed_at()` consistent with the value that
        // has been set using this method.
        auto timestamp = *to_timestamp(accessed_at);
        auto secs_per_day = 86400;
        timestamp += secs_per_day - 1;
        timestamp -= timestamp % secs_per_day;
        pimpl_->set_metadata_int(
            metadata_int_type::last_accessed_ts, timestamp);
    }
    else
    {
        pimpl_->set_metadata_int(
            metadata_int_type::last_accessed_ts, boost::none);
    }
}

void track::set_last_accessed_at(system_clock::time_point accessed_at) const
{
    set_last_accessed_at(boost::make_optional(accessed_at));
}

boost::optional<system_clock::time_point> track::last_modified_at() const
{
    return to_time_point(
        pimpl_->get_metadata_int(metadata_int_type::last_modified_ts));
}

void track::set_last_modified_at(
    boost::optional<system_clock::time_point> modified_at) const
{
    pimpl_->set_metadata_int(
        metadata_int_type::last_modified_ts, to_timestamp(modified_at));
}

void track::set_last_modified_at(system_clock::time_point modified_at) const
{
    set_last_modified_at(boost::make_optional(modified_at));
}

boost::optional<system_clock::time_point> track::last_played_at() const
{
    return to_time_point(
        pimpl_->get_metadata_int(metadata_int_type::last_played_ts));
}

void track::set_last_played_at(
    boost::optional<system_clock::time_point> played_at) const
{
    static boost::optional<boost::string_view> zero{"0"};
    static boost::optional<boost::string_view> one{"1"};
    pimpl_->set_metadata_str(
        metadata_str_type::ever_played, played_at ? one : zero);
    pimpl_->set_metadata_int(
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

void track::set_last_played_at(system_clock::time_point played_at) const
{
    set_last_played_at(boost::make_optional(played_at));
}

boost::optional<loop> track::loop_at(int32_t index) const
{
    auto loops_d = pimpl_->get_loops_data();
    return std::move(loops_d.loops[index]);
}

void track::set_loop_at(int32_t index, boost::optional<loop> l) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    auto loops_d = pimpl_->get_loops_data();
    loops_d.loops[index] = std::move(l);
    pimpl_->set_loops_data(std::move(loops_d));
    db().pimpl_->perfdata_db_ << "END";
}

void track::set_loop_at(int32_t index, loop l) const
{
    set_loop_at(index, boost::make_optional(l));
}

std::array<boost::optional<loop>, 8> track::loops() const
{
    auto loops_d = pimpl_->get_loops_data();
    return std::move(loops_d.loops);
}

void track::set_loops(std::array<boost::optional<loop>, 8> cues) const
{
    db().pimpl_->perfdata_db_ << "BEGIN";
    loops_data loops_d;
    loops_d.loops = std::move(cues);
    pimpl_->set_loops_data(std::move(loops_d));
    db().pimpl_->perfdata_db_ << "END";
}

std::vector<waveform_entry> track::overview_waveform() const
{
    auto overview_waveform_d = pimpl_->get_overview_waveform_data();
    return std::move(overview_waveform_d.waveform);
}

boost::optional<std::string> track::publisher() const
{
    return pimpl_->get_metadata_str(metadata_str_type::publisher);
}

void track::set_publisher(boost::optional<boost::string_view> publisher) const
{
    pimpl_->set_metadata_str(metadata_str_type::publisher, publisher);
}

void track::set_publisher(boost::string_view publisher) const
{
    set_publisher(boost::make_optional(publisher));
}

int64_t track::recommended_waveform_size() const
{
    auto smp = sampling();
    if (!smp)
    {
        return 0;
    }
    if (smp->sample_count <= 0)
    {
        throw track_database_inconsistency{
            "Track has non-positive sample count", id()};
    }
    if (smp->sample_rate <= 0)
    {
        throw track_database_inconsistency{"Track has non-positive sample rate",
                                           id()};
    }
    return static_cast<int64_t>(smp->sample_count * 105 / smp->sample_rate);
}

std::string track::relative_path() const
{
    return pimpl_->get_cell<std::string>("path");
}

void track::set_relative_path(boost::string_view relative_path) const
{
    // TODO (haslersn): Should we check the path?
    pimpl_->set_cell("path", std::string{relative_path});
    auto filename = get_filename(relative_path);
    pimpl_->set_cell("filename", std::string{filename});
    auto extension = get_file_extension(filename);
    pimpl_->set_metadata_str(metadata_str_type::file_extension, extension);
}

boost::optional<sampling_info> track::sampling() const
{
    return pimpl_->get_track_data().sampling;
}

void track::set_sampling(boost::optional<sampling_info> sampling) const
{
    // TODO (haslersn): atomic?

    boost::optional<int64_t> secs;
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
        pimpl_->set_metadata_str(
            metadata_str_type::duration_mm_ss, boost::string_view{str});
    }
    else
    {
        pimpl_->set_metadata_str(
            metadata_str_type::duration_mm_ss, boost::none);
    }
    pimpl_->set_cell("length", secs);
    pimpl_->set_cell("lengthCalculated", secs);

    db().pimpl_->perfdata_db_ << "BEGIN";

    // read old data
    auto track_d = pimpl_->get_track_data();
    auto beat_d = pimpl_->get_beat_data();
    auto high_res_waveform_d = pimpl_->get_high_res_waveform_data();
    auto overview_waveform_d = pimpl_->get_overview_waveform_data();

    track_d.sampling = sampling;
    beat_d.sampling = sampling;
    pimpl_->set_beat_data(std::move(beat_d));
    pimpl_->set_track_data(std::move(track_d));

    int64_t sample_count = sampling ? sampling->sample_count : 0;

    if (!high_res_waveform_d.waveform.empty())
    {
        high_res_waveform_d.samples_per_entry =
            sample_count / high_res_waveform_d.waveform.size();
        pimpl_->set_high_res_waveform_data(std::move(high_res_waveform_d));
    }

    if (!overview_waveform_d.waveform.empty())
    {
        overview_waveform_d.samples_per_entry =
            sample_count / overview_waveform_d.waveform.size();
        pimpl_->set_overview_waveform_data(std::move(overview_waveform_d));
    }

    db().pimpl_->perfdata_db_ << "COMMIT";
}

void track::set_sampling(sampling_info sampling) const
{
    set_sampling(boost::make_optional(sampling));
}

boost::optional<std::string> track::title() const
{
    return pimpl_->get_metadata_str(metadata_str_type::title);
}

void track::set_title(boost::optional<boost::string_view> title) const
{
    pimpl_->set_metadata_str(metadata_str_type::title, title);
}

void track::set_title(boost::string_view title) const
{
    set_title(boost::make_optional(title));
}

boost::optional<int32_t> track::track_number() const
{
    return pimpl_->get_cell<boost::optional<int32_t>>("playOrder");
}

void track::set_track_number(boost::optional<int32_t> track_number) const
{
    pimpl_->set_cell("playOrder", track_number);
}

void track::set_track_number(int32_t track_number) const
{
    set_track_number(boost::make_optional(track_number));
}

std::vector<waveform_entry> track::waveform() const
{
    auto high_res_waveform_d = pimpl_->get_high_res_waveform_data();
    return std::move(high_res_waveform_d.waveform);
}

void track::set_waveform(std::vector<waveform_entry> waveform) const
{
    overview_waveform_data overview_waveform_d;
    high_res_waveform_data high_res_waveform_d;

    if (!waveform.empty())
    {
        // TODO (haslersn): atomic?
        auto smp = sampling();
        int64_t sample_count = smp ? smp->sample_count : 0;
        overview_waveform_d.samples_per_entry = sample_count / 1024;
        overview_waveform_d.waveform.reserve(1024);
        for (int32_t i = 0; i < 1024; ++i)
        {
            auto entry = waveform[waveform.size() * (2 * i + 1) / 2048];
            overview_waveform_d.waveform.push_back(entry);
        }
        high_res_waveform_d.samples_per_entry = sample_count / waveform.size();
        high_res_waveform_d.waveform = std::move(waveform);
    }

    db().pimpl_->perfdata_db_ << "BEGIN";
    pimpl_->set_overview_waveform_data(std::move(overview_waveform_d));
    pimpl_->set_high_res_waveform_data(std::move(high_res_waveform_d));
    db().pimpl_->perfdata_db_ << "END";
}

boost::optional<int32_t> track::year() const
{
    return pimpl_->get_cell<boost::optional<int32_t>>("year");
}

void track::set_year(boost::optional<int32_t> year) const
{
    pimpl_->set_cell("year", year);
}

void track::set_year(int32_t year) const
{
    set_year(boost::make_optional(year));
}

track::track(database database, int64_t id)
    : pimpl_{std::make_shared<impl>(database, id)}
{
}

}  // namespace enginelibrary
}  // namespace djinterop
