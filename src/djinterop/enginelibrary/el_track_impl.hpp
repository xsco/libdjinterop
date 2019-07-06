#pragma once

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>

#include <djinterop/enginelibrary.hpp>
#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/enginelibrary/performance_data_format.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/impl/track_impl.hpp>

namespace djinterop
{
class crate;
class database;
enum class musical_key;

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

class el_track_impl : public djinterop::track_impl
{
public:
    el_track_impl(std::shared_ptr<el_storage> storage, int64_t id);

    boost::optional<std::string> get_metadata_str(metadata_str_type type);
    void set_metadata_str(
        metadata_str_type type, boost::optional<boost::string_view> content);
    void set_metadata_str(metadata_str_type type, const std::string& content);
    boost::optional<int64_t> get_metadata_int(metadata_int_type type);
    void set_metadata_int(
        metadata_int_type type, boost::optional<int64_t> content);

    template <typename T>
    T get_cell(const char* column_name)
    {
        boost::optional<T> result;
        storage_->db << (std::string{"SELECT "} + column_name +
                         " FROM Track WHERE id = ?")
                     << id() >>
            [&](T cell) {
                if (!result)
                {
                    result = cell;
                }
                else
                {
                    throw track_database_inconsistency{
                        "More than one track with the same ID", id()};
                }
            };
        if (!result)
        {
            throw track_deleted{id()};
        }
        return *result;
    }

    template <typename T>
    void set_cell(const char* column_name, const T& content)
    {
        storage_->db << (std::string{"UPDATE Track SET "} + column_name +
                         " = ? WHERE id = ?")
                     << content << id();
    }

    template <typename T>
    T get_perfdata(const char* column_name)
    {
        boost::optional<T> result;
        storage_->db << (std::string{"SELECT "} + column_name +
                         " From PerformanceData WHERE id = ?")
                     << id() >>
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
                        id()};
                }
            };
        return result.value_or(T{});
    }

    // NOTE: No transaction
    template <typename T>
    void set_perfdata(const char* column_name, const T& content)
    {
        bool found = false;
        storage_->db << "SELECT COUNT(*) FROM PerformanceData WHERE id = ?"
                     << id() >>
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
                        id()};
                }
            };

        if (!found)
        {
            storage_->db
                << "INSERT INTO PerformanceData (id, isAnalyzed, isRendered, "
                   "trackData, highResolutionWaveFormData, "
                   "overviewWaveFormData, beatData, quickCues, loops, "
                   "hasSeratoValues) VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"
                << id()                               //
                << 0.0                                // isAnalyzed
                << 0.0                                // isRendered
                << track_data{}.encode()              //
                << high_res_waveform_data{}.encode()  //
                << overview_waveform_data{}.encode()  //
                << beat_data{}.encode()               //
                << quick_cues_data{}.encode()         //
                << loops_data{}.encode()              //
                << 0.0;                               // hasSeratoValues

            // TODO (haslersn): Don't allocate during the version() call
            if (db().version() >= version_1_7_1)
            {
                storage_->db
                    << "UPDATE PerformanceData SET hasRekordboxValues = 0 "
                       "WHERE id = ?"
                    << id();
            }
        }

        storage_->db << (std::string{"UPDATE PerformanceData SET "} +
                         column_name + " = ? WHERE id = ?")
                     << content.encode() << id();
    }

    beat_data get_beat_data();
    void set_beat_data(beat_data data);
    high_res_waveform_data get_high_res_waveform_data();
    void set_high_res_waveform_data(high_res_waveform_data data);
    loops_data get_loops_data();
    void set_loops_data(loops_data data);
    overview_waveform_data get_overview_waveform_data();
    void set_overview_waveform_data(overview_waveform_data data);
    quick_cues_data get_quick_cues_data();
    void set_quick_cues_data(quick_cues_data data);
    track_data get_track_data();
    void set_track_data(track_data data);

    std::vector<beatgrid_marker> adjusted_beatgrid() override;
    void set_adjusted_beatgrid(std::vector<beatgrid_marker> beatgrid) override;
    double adjusted_main_cue() override;
    void set_adjusted_main_cue(double sample_offset) override;
    boost::optional<std::string> album() override;
    void set_album(boost::optional<boost::string_view> album) override;
    boost::optional<int64_t> album_art_id() override;
    void set_album_art_id(boost::optional<int64_t> album_art_id) override;
    boost::optional<std::string> artist() override;
    void set_artist(boost::optional<boost::string_view> artist) override;
    boost::optional<double> average_loudness() override;
    void set_average_loudness(
        boost::optional<double> average_loudness) override;
    boost::optional<int64_t> bitrate() override;
    void set_bitrate(boost::optional<int64_t> bitrate) override;
    boost::optional<double> bpm() override;
    void set_bpm(boost::optional<double> bpm) override;
    boost::optional<std::string> comment() override;
    void set_comment(boost::optional<boost::string_view> comment) override;
    boost::optional<std::string> composer() override;
    void set_composer(boost::optional<boost::string_view> composer) override;
    std::vector<djinterop::crate> containing_crates() override;
    database db() override;
    std::vector<beatgrid_marker> default_beatgrid() override;
    void set_default_beatgrid(std::vector<beatgrid_marker> beatgrid) override;
    double default_main_cue() override;
    void set_default_main_cue(double sample_offset) override;
    boost::optional<std::chrono::milliseconds> duration() override;
    std::string file_extension() override;
    std::string filename() override;
    boost::optional<std::string> genre() override;
    void set_genre(boost::optional<boost::string_view> genre) override;
    boost::optional<hot_cue> hot_cue_at(int32_t index) override;
    void set_hot_cue_at(int32_t index, boost::optional<hot_cue> cue) override;
    std::array<boost::optional<hot_cue>, 8> hot_cues() override;
    void set_hot_cues(std::array<boost::optional<hot_cue>, 8> cues) override;
    boost::optional<track_import_info> import_info() override;
    void set_import_info(
        const boost::optional<track_import_info>& import_info) override;
    bool is_valid() override;
    boost::optional<musical_key> key() override;
    void set_key(boost::optional<musical_key> key) override;
    boost::optional<std::chrono::system_clock::time_point> last_accessed_at()
        override;
    void set_last_accessed_at(
        boost::optional<std::chrono::system_clock::time_point> accessed_at)
        override;
    boost::optional<std::chrono::system_clock::time_point> last_modified_at()
        override;
    void set_last_modified_at(
        boost::optional<std::chrono::system_clock::time_point> modified_at)
        override;
    boost::optional<std::chrono::system_clock::time_point> last_played_at()
        override;
    void set_last_played_at(
        boost::optional<std::chrono::system_clock::time_point> played_at)
        override;
    boost::optional<loop> loop_at(int32_t index) override;
    void set_loop_at(int32_t index, boost::optional<loop> l) override;
    std::array<boost::optional<loop>, 8> loops() override;
    void set_loops(std::array<boost::optional<loop>, 8> cues) override;
    std::vector<waveform_entry> overview_waveform() override;
    boost::optional<std::string> publisher() override;
    void set_publisher(boost::optional<boost::string_view> publisher) override;
    int64_t recommended_waveform_size() override;
    std::string relative_path() override;
    void set_relative_path(boost::string_view relative_path) override;
    boost::optional<sampling_info> sampling() override;
    void set_sampling(boost::optional<sampling_info> sampling) override;
    boost::optional<std::string> title() override;
    void set_title(boost::optional<boost::string_view> title) override;
    boost::optional<int32_t> track_number() override;
    void set_track_number(boost::optional<int32_t> track_number) override;
    std::vector<waveform_entry> waveform() override;
    void set_waveform(std::vector<waveform_entry> waveform) override;
    boost::optional<int32_t> year() override;
    void set_year(boost::optional<int32_t> year) override;

private:
    std::shared_ptr<el_storage> storage_;
};

}  // namespace enginelibrary
}  // namespace djinterop
