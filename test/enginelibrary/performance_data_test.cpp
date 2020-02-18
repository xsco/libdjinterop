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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE performance_data_test

#include <chrono>
#include <cstdio>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>

#include <djinterop/database.hpp>
#include <djinterop/enginelibrary.hpp>
#include <djinterop/track.hpp>

#include "sqlite_modern_cpp.h"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace el = djinterop::enginelibrary;
namespace fs = boost::filesystem;
namespace c = std::chrono;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el3"};

static fs::path create_temp_dir()
{
    fs::path temp_dir{fs::temp_directory_path()};
    temp_dir /= fs::unique_path();
    if (!fs::create_directory(temp_dir))
    {
        throw std::runtime_error{"Failed to create tmp_dir"};
    }
    std::cout << "Created temp dir at " << temp_dir.string() << std::endl;
    return temp_dir;
}

static void remove_temp_dir(const fs::path& temp_dir)
{
    fs::remove_all(temp_dir);
    std::cout << "Removed temp dir at " << temp_dir.string() << std::endl;
}

static void populate_track_1(djinterop::track& t)
{
    // Usual meta-data (not under test but for completeness).
    t.set_track_number(1);
    t.set_bpm(123);
    t.set_year(2017);
    t.set_title("Mad (Original Mix)");
    t.set_artist("Dennis Cruz");
    t.set_album("Mad EP");
    t.set_genre("Tech House");
    t.set_comment("Purchased at Beatport.com");
    t.set_publisher("Stereo Productions");
    t.set_composer(djinterop::stdx::nullopt);
    t.set_key(djinterop::musical_key::a_minor);
    t.set_relative_path("../01 - Dennis Cruz - Mad (Original Mix).mp3");
    t.set_last_modified_at(c::system_clock::time_point{c::seconds{1509371790}});
    t.set_bitrate(320);
    t.set_last_played_at(djinterop::stdx::nullopt);
    t.set_last_accessed_at(c::system_clock::time_point{c::seconds{1509321600}});
    t.set_import_info(djinterop::stdx::nullopt);

    // Track data fields
    t.set_sampling(djinterop::sampling_info{44100, 17452800});
    t.set_key(djinterop::musical_key::a_minor);
    t.set_average_loudness(0.520831584930419921875);

    // Beat data fields
    t.set_default_beatgrid({{-4, -83316.78}, {812, 17470734.439}});
    t.set_adjusted_beatgrid({{-4, -84904.768}, {812, 17469046.451}});

    // Quick cue fields
    t.set_hot_cues({});
    t.set_hot_cue_at(0, {"Cue 1", 1377924.5, el::standard_pad_colors::pad_1});
    t.set_hot_cue_at(2, {"Cue 3", 5508265.964, el::standard_pad_colors::pad_3});
    t.set_hot_cue_at(4, {"Cue 5", 8261826.939, el::standard_pad_colors::pad_5});
    t.set_hot_cue_at(5, {"Cue 6", 9638607.427, el::standard_pad_colors::pad_6});
    t.set_adjusted_main_cue(1377924.5);
    t.set_default_main_cue(1144.012);

    // Loop fields
    std::array<djinterop::stdx::optional<djinterop::loop>, 8> loops;
    loops[0] = djinterop::loop{"Loop 1", 1144.012, 345339.134,
                               el::standard_pad_colors::pad_1};
    loops[1] = djinterop::loop{"Loop 2", 2582607.427, 2754704.988,
                               el::standard_pad_colors::pad_2};
    loops[3] = djinterop::loop{"Loop 4", 4131485.476, 4303583.037,
                               el::standard_pad_colors::pad_4};
    t.set_loops(std::move(loops));

    // High-resolution waveform data
    std::vector<djinterop::waveform_entry> waveform;
    int64_t waveform_size = t.recommended_waveform_size();
    waveform.reserve(waveform_size);
    for (int64_t i = 0; i < waveform_size; ++i)
    {
        waveform.push_back({
                {(uint8_t)(i * 255 / waveform_size),
                 (uint8_t)(i * 255 / waveform_size)},
                {(uint8_t)(i * 127 / waveform_size),
                 (uint8_t)(i * 127 / waveform_size)},
                {(uint8_t)(i * 63 / waveform_size),
                 (uint8_t)(i * 63 / waveform_size)}});
    }
    t.set_waveform(std::move(waveform));
}

static void check_track_1(const djinterop::track& t)
{
    // Track data fields
    BOOST_CHECK_CLOSE(t.sampling()->sample_rate, 44100.0, 0.001);
    BOOST_CHECK(t.key() == djinterop::musical_key::a_minor);
    BOOST_CHECK_CLOSE(*t.average_loudness(), 0.520831584930419921875, 0.001);
    BOOST_CHECK(*t.duration() == c::milliseconds{395755});

    // Beat data fields
    auto default_beatgrid = t.default_beatgrid();
    BOOST_CHECK_EQUAL(default_beatgrid.size(), 2);
    BOOST_CHECK_EQUAL(default_beatgrid[0].index, -4);
    BOOST_CHECK_CLOSE(default_beatgrid[0].sample_offset, -83316.78, 0.001);
    BOOST_CHECK_EQUAL(default_beatgrid[1].index, 812);
    BOOST_CHECK_CLOSE(default_beatgrid[1].sample_offset, 17470734.439, 0.001);
    auto adjusted_beatgrid = t.adjusted_beatgrid();
    BOOST_CHECK_EQUAL(adjusted_beatgrid.size(), 2);
    BOOST_CHECK_EQUAL(adjusted_beatgrid[0].index, -4);
    BOOST_CHECK_CLOSE(adjusted_beatgrid[0].sample_offset, -84904.768, 0.001);
    BOOST_CHECK_EQUAL(adjusted_beatgrid[1].index, 812);
    BOOST_CHECK_CLOSE(adjusted_beatgrid[1].sample_offset, 17469046.451, 0.001);

    // Quick cue fields
    auto hot_cues = t.hot_cues();

    BOOST_CHECK(hot_cues[0]);
    BOOST_CHECK_EQUAL(hot_cues[0]->label, "Cue 1");
    BOOST_CHECK_CLOSE(hot_cues[0]->sample_offset, 1377924.5, 0.001);
    BOOST_CHECK(hot_cues[0]->color == el::standard_pad_colors::pad_1);

    BOOST_CHECK(!hot_cues[1]);

    BOOST_CHECK(hot_cues[2]);
    BOOST_CHECK_EQUAL(hot_cues[2]->label, "Cue 3");
    BOOST_CHECK_CLOSE(hot_cues[2]->sample_offset, 5508265.964, 0.001);
    BOOST_CHECK(hot_cues[2]->color == el::standard_pad_colors::pad_3);

    BOOST_CHECK(!hot_cues[3]);

    BOOST_CHECK(hot_cues[4]);
    BOOST_CHECK_EQUAL(hot_cues[4]->label, "Cue 5");
    BOOST_CHECK_CLOSE(hot_cues[4]->sample_offset, 8261826.939, 0.001);
    BOOST_CHECK(hot_cues[4]->color == el::standard_pad_colors::pad_5);

    BOOST_CHECK(hot_cues[5]);
    BOOST_CHECK_EQUAL(hot_cues[5]->label, "Cue 6");
    BOOST_CHECK_CLOSE(hot_cues[5]->sample_offset, 9638607.427, 0.001);
    BOOST_CHECK(hot_cues[5]->color == el::standard_pad_colors::pad_6);

    BOOST_CHECK(!hot_cues[6]);
    BOOST_CHECK(!hot_cues[7]);

    BOOST_CHECK_CLOSE(t.adjusted_main_cue(), 1377924.5, 0.001);
    BOOST_CHECK_CLOSE(t.default_main_cue(), 1144.012, 0.001);

    // Loop fields
    std::cout << "Checking loops..." << std::endl;
    auto loops = t.loops();

    BOOST_CHECK(loops[0]);
    BOOST_CHECK_EQUAL(loops[0]->label, "Loop 1");
    BOOST_CHECK_CLOSE(loops[0]->start_sample_offset, 1144.012, 0.001);
    BOOST_CHECK_CLOSE(loops[0]->end_sample_offset, 345339.134, 0.001);
    BOOST_CHECK(loops[0]->color == el::standard_pad_colors::pad_1);

    BOOST_CHECK(loops[1]);
    BOOST_CHECK_EQUAL(loops[1]->label, "Loop 2");
    BOOST_CHECK_CLOSE(loops[1]->start_sample_offset, 2582607.427, 0.001);
    BOOST_CHECK_CLOSE(loops[1]->end_sample_offset, 2754704.988, 0.001);
    BOOST_CHECK(loops[1]->color == el::standard_pad_colors::pad_2);

    BOOST_CHECK(!loops[2]);

    BOOST_CHECK(loops[3]);
    BOOST_CHECK_EQUAL(loops[3]->label, "Loop 4");
    BOOST_CHECK_CLOSE(loops[3]->start_sample_offset, 4131485.476, 0.001);
    BOOST_CHECK_CLOSE(loops[3]->end_sample_offset, 4303583.037, 0.001);
    BOOST_CHECK(loops[3]->color == el::standard_pad_colors::pad_4);

    BOOST_CHECK(!loops[4]);
    BOOST_CHECK(!loops[5]);
    BOOST_CHECK(!loops[6]);
    BOOST_CHECK(!loops[7]);

    // Overview waveform data
    BOOST_CHECK_EQUAL(t.overview_waveform().size(), 1024);

    // High-res waveform data
    BOOST_CHECK_EQUAL(t.waveform().size(), 41554);
}

static void populate_track_2(djinterop::track& t)
{
    // Usual meta-data (not under test but for completeness).
    t.set_track_number(3);
    t.set_bpm(128);
    t.set_year(2018);
    t.set_title("Made-up Track (Foo Bar Remix)");
    t.set_artist("Not A Real Artist");
    t.set_album("Fake Album");
    t.set_genre("Progressive House");
    t.set_comment("Comment goes here");
    t.set_publisher("Here is the publisher text");
    t.set_composer("And the composer text");
    t.set_key(djinterop::musical_key::c_major);
    t.set_relative_path(
        "../03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
    t.set_last_modified_at(c::system_clock::time_point{c::seconds{1517413933}});
    t.set_bitrate(1411);
    t.set_last_played_at(c::system_clock::time_point{c::seconds{1518739200}});
    t.set_last_accessed_at(c::system_clock::time_point{c::seconds{1518815683}});
    t.set_import_info({"e535b170-26ef-4f30-8cb2-5b9fa4c2a27f", 123});
    
    // Track data fields
    t.set_sampling(djinterop::sampling_info{48000, 10795393});
    t.set_key(djinterop::musical_key::b_minor);
    t.set_average_loudness(0.5);

    // Beat data fields
    t.set_default_beatgrid({{-4, -107595.55}, {402, 10820254.92}});
    t.set_adjusted_beatgrid({{-4, -107595.55}, {402, 10820254.92}});

    // Quick cue fields
    std::array<djinterop::stdx::optional<djinterop::hot_cue>, 8> cues;
    cues[1] =
        djinterop::hot_cue{"Cue 2", 1234567.89, el::standard_pad_colors::pad_2};
    t.set_hot_cues(std::move(cues));
    t.set_adjusted_main_cue(1234500.01);
    t.set_default_main_cue(12345.678);

    // Loop fields
    t.set_loops({});
    t.set_loop_at(
        1, {"Loop 2", 2345600, 2345700, el::standard_pad_colors::pad_2});

    // High-resolution waveform data
    int64_t waveform_size = t.recommended_waveform_size();
    std::vector<djinterop::waveform_entry> waveform;
    waveform.reserve(waveform_size);
    for (int64_t i = 0; i < waveform_size; ++i)
    {
        waveform.push_back({
                {(uint8_t)(i * 255 / waveform_size),
                 (uint8_t)(i * 255 / waveform_size)},
                {(uint8_t)(i * 127 / waveform_size),
                 (uint8_t)(i * 127 / waveform_size)},
                {(uint8_t)(i * 63 / waveform_size),
                 (uint8_t)(i * 63 / waveform_size)}});
    }
    t.set_waveform(std::move(waveform));
}

static void check_track_2(const djinterop::track& t)
{
    // Track data fields
    BOOST_CHECK_CLOSE(t.sampling()->sample_rate, 48000.0, 0.001);
    BOOST_CHECK_CLOSE(*t.average_loudness(), 0.5, 0.001);
    BOOST_CHECK(t.duration() == c::milliseconds{224904});

    // Beat data fields
    auto default_beatgrid = t.default_beatgrid();
    BOOST_CHECK_EQUAL(default_beatgrid.size(), 2);
    BOOST_CHECK_EQUAL(default_beatgrid[0].index, -4);
    BOOST_CHECK_CLOSE(default_beatgrid[0].sample_offset, -107595.55, 0.001);
    BOOST_CHECK_EQUAL(default_beatgrid[1].index, 402);
    BOOST_CHECK_CLOSE(default_beatgrid[1].sample_offset, 10820254.92, 0.001);
    auto adjusted_beatgrid = t.adjusted_beatgrid();
    BOOST_CHECK_EQUAL(adjusted_beatgrid.size(), 2);
    BOOST_CHECK_EQUAL(adjusted_beatgrid[0].index, -4);
    BOOST_CHECK_CLOSE(adjusted_beatgrid[0].sample_offset, -107595.55, 0.001);
    BOOST_CHECK_EQUAL(adjusted_beatgrid[1].index, 402);
    BOOST_CHECK_CLOSE(adjusted_beatgrid[1].sample_offset, 10820254.92, 0.001);

    // Quick cue fields
    auto hot_cues = t.hot_cues();

    BOOST_CHECK(!hot_cues[0]);

    BOOST_CHECK(hot_cues[1]);
    BOOST_CHECK_EQUAL(hot_cues[1]->label, "Cue 2");
    BOOST_CHECK_CLOSE(hot_cues[1]->sample_offset, 1234567.89, 0.001);
    BOOST_CHECK(hot_cues[1]->color == el::standard_pad_colors::pad_2);

    BOOST_CHECK(!hot_cues[2]);
    BOOST_CHECK(!hot_cues[3]);
    BOOST_CHECK(!hot_cues[4]);
    BOOST_CHECK(!hot_cues[5]);
    BOOST_CHECK(!hot_cues[6]);
    BOOST_CHECK(!hot_cues[7]);

    BOOST_CHECK_CLOSE(t.adjusted_main_cue(), 1234500.01, 0.001);
    BOOST_CHECK_CLOSE(t.default_main_cue(), 12345.678, 0.001);

    // Loop fields
    auto loops = t.loops();

    BOOST_CHECK(!loops[0]);

    BOOST_CHECK(loops[1]);
    BOOST_CHECK_EQUAL(loops[1]->label, "Loop 2");
    BOOST_CHECK_CLOSE(loops[1]->start_sample_offset, 2345600, 0.001);
    BOOST_CHECK_CLOSE(loops[1]->end_sample_offset, 2345700, 0.001);
    BOOST_CHECK(loops[1]->color == el::standard_pad_colors::pad_2);

    BOOST_CHECK(!loops[2]);
    BOOST_CHECK(!loops[3]);
    BOOST_CHECK(!loops[4]);
    BOOST_CHECK(!loops[5]);
    BOOST_CHECK(!loops[6]);
    BOOST_CHECK(!loops[7]);

    // Overview waveform data
    BOOST_CHECK_EQUAL(t.overview_waveform().size(), 1024);

    // High-res waveform data
    BOOST_CHECK_EQUAL(t.waveform().size(), 23614);
}

BOOST_AUTO_TEST_CASE(save__new_track_good_values__saves)
{
    // Arrange/Act
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto t = db.create_track("");
    populate_track_1(t);

    // Assert
    check_track_1(t);
    auto t_reloaded = *db.track_by_id(t.id());
    BOOST_CHECK_EQUAL(t_reloaded.id(), t.id());
    check_track_1(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(save__existing_track__saves)
{
    // Arrange/Act
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto t = db.create_track("");
    populate_track_1(t);
    populate_track_2(t);

    // Assert
    check_track_2(t);
    auto t_reloaded = *db.track_by_id(t.id());
    BOOST_CHECK_EQUAL(t_reloaded.id(), t.id());
    check_track_2(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(set_hot_cue_at__empty_track_valid_entries__succeeds)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto t = db.create_track("");

    // Act
    djinterop::hot_cue hc{"My Cue", 12345.6789, el::standard_pad_colors::pad_3};
    t.set_hot_cue_at(1, hc);

    // Assert
    auto hot_cues = t.hot_cues();
    BOOST_CHECK(hot_cues[1]);
}

