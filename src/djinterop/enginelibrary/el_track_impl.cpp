#include <cmath>

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_crate_impl.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_track_impl.hpp>
#include <djinterop/impl/util.hpp>

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

el_track_impl::el_track_impl(std::shared_ptr<el_storage> storage, int64_t id)
    : track_impl{id}, storage_{std::move(storage)}
{
}

boost::optional<std::string> el_track_impl::get_metadata_str(
    metadata_str_type type)
{
    boost::optional<std::string> result;
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
    metadata_str_type type, boost::optional<boost::string_view> content)
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

boost::optional<int64_t> el_track_impl::get_metadata_int(metadata_int_type type)
{
    boost::optional<int64_t> result;
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
    metadata_int_type type, boost::optional<int64_t> content)
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
    storage_->db << "BEGIN";
    auto beat_d = get_beat_data();
    beat_d.adjusted_beatgrid = std::move(beatgrid);
    set_beat_data(std::move(beat_d));
    storage_->db << "COMMIT";
}

double el_track_impl::adjusted_main_cue()
{
    return get_quick_cues_data().adjusted_main_cue;
}

void el_track_impl::set_adjusted_main_cue(double sample_offset)
{
    storage_->db << "BEGIN";
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.adjusted_main_cue = sample_offset;
    set_quick_cues_data(std::move(quick_cues_d));
    storage_->db << "COMMIT";
}

boost::optional<std::string> el_track_impl::album()
{
    return get_metadata_str(metadata_str_type::album);
}

void el_track_impl::set_album(boost::optional<boost::string_view> album)
{
    set_metadata_str(metadata_str_type::album, album);
}

boost::optional<int64_t> el_track_impl::album_art_id()
{
    int64_t cell = get_cell<int64_t>("idAlbumArt");
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

void el_track_impl::set_album_art_id(boost::optional<int64_t> album_art_id)
{
    if (album_art_id && *album_art_id <= 1)
    {
        // TODO (haslersn): Throw something.
    }
    set_cell("idAlbumArt", album_art_id.value_or(1));
    // 1 is the magic number for "no album art"
}

boost::optional<std::string> el_track_impl::artist()
{
    return get_metadata_str(metadata_str_type::artist);
}

void el_track_impl::set_artist(boost::optional<boost::string_view> artist)
{
    set_metadata_str(metadata_str_type::artist, artist);
}

boost::optional<double> el_track_impl::average_loudness()
{
    return get_track_data().average_loudness;
}

void el_track_impl::set_average_loudness(
    boost::optional<double> average_loudness)
{
    storage_->db << "BEGIN";
    auto track_d = get_track_data();
    track_d.average_loudness = average_loudness;
    set_track_data(track_d);
    storage_->db << "COMMIT";
}

boost::optional<int64_t> el_track_impl::bitrate()
{
    return get_cell<boost::optional<int64_t>>("bitrate");
}

void el_track_impl::set_bitrate(boost::optional<int64_t> bitrate)
{
    set_cell("bitrate", bitrate);
}

boost::optional<double> el_track_impl::bpm()
{
    return get_cell<boost::optional<double>>("bpmAnalyzed");
}

void el_track_impl::set_bpm(boost::optional<double> bpm)
{
    set_cell("bpmAnalyzed", bpm);
    boost::optional<int64_t> ceiled_bpm;
    if (bpm)
    {
        ceiled_bpm = static_cast<int64_t>(std::ceil(*bpm));
    }
    set_cell("bpm", ceiled_bpm);
}

boost::optional<std::string> el_track_impl::comment()
{
    return get_metadata_str(metadata_str_type::comment);
}

void el_track_impl::set_comment(boost::optional<boost::string_view> comment)
{
    set_metadata_str(metadata_str_type::comment, comment);
}

boost::optional<std::string> el_track_impl::composer()
{
    return get_metadata_str(metadata_str_type::composer);
}

void el_track_impl::set_composer(boost::optional<boost::string_view> composer)
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
    storage_->db << "BEGIN";
    auto beat_d = get_beat_data();
    beat_d.default_beatgrid = std::move(beatgrid);
    set_beat_data(std::move(beat_d));
    storage_->db << "COMMIT";
}

double el_track_impl::default_main_cue()
{
    return get_quick_cues_data().default_main_cue;
}

void el_track_impl::set_default_main_cue(double sample_offset)
{
    storage_->db << "BEGIN";
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.default_main_cue = sample_offset;
    set_quick_cues_data(std::move(quick_cues_d));
    storage_->db << "COMMIT";
}

boost::optional<milliseconds> el_track_impl::duration()
{
    boost::optional<milliseconds> result;
    auto smp = sampling();
    if (smp)
    {
        double secs = smp->sample_count / smp->sample_rate;
        return milliseconds{static_cast<int64_t>(1000 * secs)};
    }
    auto secs = get_cell<boost::optional<int64_t>>("length");
    if (secs)
    {
        return milliseconds{*secs * 1000};
    }
    return boost::none;
}

std::string el_track_impl::file_extension()
{
    auto rel_path = relative_path();
    return get_file_extension(rel_path)
        .value_or(boost::string_view{})
        .to_string();
}

std::string el_track_impl::filename()
{
    auto rel_path = relative_path();
    return get_filename(rel_path).to_string();
}

boost::optional<std::string> el_track_impl::genre()
{
    return get_metadata_str(metadata_str_type::genre);
}

void el_track_impl::set_genre(boost::optional<boost::string_view> genre)
{
    set_metadata_str(metadata_str_type::genre, genre);
}

boost::optional<hot_cue> el_track_impl::hot_cue_at(int32_t index)
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues[index]);
}

void el_track_impl::set_hot_cue_at(int32_t index, boost::optional<hot_cue> cue)
{
    storage_->db << "BEGIN";
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues[index] = std::move(cue);
    set_quick_cues_data(std::move(quick_cues_d));
    storage_->db << "END";
}

std::array<boost::optional<hot_cue>, 8> el_track_impl::hot_cues()
{
    auto quick_cues_d = get_quick_cues_data();
    return std::move(quick_cues_d.hot_cues);
}

void el_track_impl::set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues)
{
    storage_->db << "BEGIN";
    // TODO (haslersn): The following can be optimized because in this case we
    // overwrite all hot_cues
    auto quick_cues_d = get_quick_cues_data();
    quick_cues_d.hot_cues = std::move(cues);
    set_quick_cues_data(std::move(quick_cues_d));
    storage_->db << "END";
}

boost::optional<track_import_info> el_track_impl::import_info()
{
    if (get_cell<int64_t>("isExternalTrack") == 0)
    {
        return boost::none;
    }
    return track_import_info{get_cell<std::string>("uuidOfExternalDatabase"),
                             get_cell<int64_t>("idTrackInExternalDatabase")};
    // TODO (haslersn): How should we handle cells that unexpectedly don't
    // contain integral values?
}

void el_track_impl::set_import_info(
    const boost::optional<track_import_info>& import_info)
{
    if (import_info)
    {
        set_cell("isExternalTrack", 1);
        set_cell("uuidOfExternalDatabase", import_info->external_db_uuid());
        set_cell("idTrackInExternalDatabase", import_info->external_track_id());
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

boost::optional<musical_key> el_track_impl::key()
{
    boost::optional<musical_key> result;
    auto key_num = get_metadata_int(metadata_int_type::musical_key);
    if (key_num)
    {
        result = static_cast<musical_key>(*key_num);
    }
    return result;
}

void el_track_impl::set_key(boost::optional<musical_key> key)
{
    boost::optional<int64_t> key_num;
    if (key)
    {
        key_num = static_cast<int64_t>(*key);
    }

    storage_->db << "BEGIN";
    auto track_d = get_track_data();
    track_d.key = key;
    set_track_data(track_d);
    storage_->db << "COMMIT";

    // TODO (haslersn): atomic?
    set_metadata_int(metadata_int_type::musical_key, key_num);
}

boost::optional<system_clock::time_point> el_track_impl::last_accessed_at()

{
    // TODO (haslersn): Is there a difference between
    // `el_track_impl::last_accessed_at()` and
    // `el_track_impl::last_played_at()`, except for the ceiling of the
    // timestamp?
    return to_time_point(get_metadata_int(metadata_int_type::last_accessed_ts));
}

void el_track_impl::set_last_accessed_at(
    boost::optional<system_clock::time_point> accessed_at)
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
        set_metadata_int(metadata_int_type::last_accessed_ts, boost::none);
    }
}

boost::optional<system_clock::time_point> el_track_impl::last_modified_at()

{
    return to_time_point(get_metadata_int(metadata_int_type::last_modified_ts));
}

void el_track_impl::set_last_modified_at(
    boost::optional<system_clock::time_point> modified_at)
{
    set_metadata_int(
        metadata_int_type::last_modified_ts, to_timestamp(modified_at));
}

boost::optional<system_clock::time_point> el_track_impl::last_played_at()

{
    return to_time_point(get_metadata_int(metadata_int_type::last_played_ts));
}

void el_track_impl::set_last_played_at(
    boost::optional<system_clock::time_point> played_at)
{
    static boost::optional<boost::string_view> zero{"0"};
    static boost::optional<boost::string_view> one{"1"};
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

boost::optional<loop> el_track_impl::loop_at(int32_t index)
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops[index]);
}

void el_track_impl::set_loop_at(int32_t index, boost::optional<loop> l)
{
    storage_->db << "BEGIN";
    auto loops_d = get_loops_data();
    loops_d.loops[index] = std::move(l);
    set_loops_data(std::move(loops_d));
    storage_->db << "END";
}

std::array<boost::optional<loop>, 8> el_track_impl::loops()
{
    auto loops_d = get_loops_data();
    return std::move(loops_d.loops);
}

void el_track_impl::set_loops(std::array<boost::optional<loop>, 8> cues)
{
    storage_->db << "BEGIN";
    loops_data loops_d;
    loops_d.loops = std::move(cues);
    set_loops_data(std::move(loops_d));
    storage_->db << "END";
}

std::vector<waveform_entry> el_track_impl::overview_waveform()
{
    auto overview_waveform_d = get_overview_waveform_data();
    return std::move(overview_waveform_d.waveform);
}

boost::optional<std::string> el_track_impl::publisher()
{
    return get_metadata_str(metadata_str_type::publisher);
}

void el_track_impl::set_publisher(boost::optional<boost::string_view> publisher)
{
    set_metadata_str(metadata_str_type::publisher, publisher);
}

int64_t el_track_impl::recommended_waveform_size()
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

std::string el_track_impl::relative_path()
{
    return get_cell<std::string>("path");
}

void el_track_impl::set_relative_path(boost::string_view relative_path)
{
    // TODO (haslersn): Should we check the path?
    set_cell("path", std::string{relative_path});
    auto filename = get_filename(relative_path);
    set_cell("filename", std::string{filename});
    auto extension = get_file_extension(filename);
    set_metadata_str(metadata_str_type::file_extension, extension);
}

boost::optional<sampling_info> el_track_impl::sampling()
{
    return get_track_data().sampling;
}

void el_track_impl::set_sampling(boost::optional<sampling_info> sampling)
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
        set_metadata_str(
            metadata_str_type::duration_mm_ss, boost::string_view{str});
    }
    else
    {
        set_metadata_str(metadata_str_type::duration_mm_ss, boost::none);
    }
    set_cell("length", secs);
    set_cell("lengthCalculated", secs);

    storage_->db << "BEGIN";

    // read old data
    auto track_d = get_track_data();
    auto beat_d = get_beat_data();
    auto high_res_waveform_d = get_high_res_waveform_data();
    auto overview_waveform_d = get_overview_waveform_data();

    track_d.sampling = sampling;
    beat_d.sampling = sampling;
    set_beat_data(std::move(beat_d));
    set_track_data(std::move(track_d));

    int64_t sample_count = sampling ? sampling->sample_count : 0;

    if (!high_res_waveform_d.waveform.empty())
    {
        high_res_waveform_d.samples_per_entry =
            sample_count / high_res_waveform_d.waveform.size();
        set_high_res_waveform_data(std::move(high_res_waveform_d));
    }

    if (!overview_waveform_d.waveform.empty())
    {
        overview_waveform_d.samples_per_entry =
            sample_count / overview_waveform_d.waveform.size();
        set_overview_waveform_data(std::move(overview_waveform_d));
    }

    storage_->db << "COMMIT";
}

boost::optional<std::string> el_track_impl::title()
{
    return get_metadata_str(metadata_str_type::title);
}

void el_track_impl::set_title(boost::optional<boost::string_view> title)
{
    set_metadata_str(metadata_str_type::title, title);
}

boost::optional<int32_t> el_track_impl::track_number()
{
    return get_cell<boost::optional<int32_t>>("playOrder");
}

void el_track_impl::set_track_number(boost::optional<int32_t> track_number)
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

    storage_->db << "BEGIN";
    set_overview_waveform_data(std::move(overview_waveform_d));
    set_high_res_waveform_data(std::move(high_res_waveform_d));
    storage_->db << "END";
}

boost::optional<int32_t> el_track_impl::year()
{
    return get_cell<boost::optional<int32_t>>("year");
}

void el_track_impl::set_year(boost::optional<int32_t> year)
{
    set_cell("year", year);
}

}  // namespace enginelibrary
}  // namespace djinterop
