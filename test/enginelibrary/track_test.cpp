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
#define BOOST_TEST_MODULE track_test

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <djinterop/crate.hpp>
#include <djinterop/database.hpp>
#include <djinterop/enginelibrary.hpp>
#include <djinterop/track.hpp>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace el = djinterop::enginelibrary;
namespace fs = boost::filesystem;
namespace c = std::chrono;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el2"};

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

static void remove_temp_dir(const fs::path &temp_dir)
{
    fs::remove_all(temp_dir);
    std::cout << "Removed temp dir at " << temp_dir.string() << std::endl;
}

static void populate_example_track_1(djinterop::track &t)
{
    t.set_track_number(1);
    t.set_bpm(123);
    t.set_year(2017);
    t.set_title("Mad (Original Mix)");
    t.set_artist("Dennis Cruz");
    t.set_album("Mad EP");
    t.set_genre("Tech House");
    t.set_comment("Purchased at Beatport.com");
    t.set_publisher("Stereo Productions");
    t.set_composer(std::experimental::nullopt);
    t.set_key(djinterop::musical_key::a_minor);
    t.set_relative_path("../01 - Dennis Cruz - Mad (Original Mix).mp3");
    t.set_last_modified_at(c::system_clock::time_point{c::seconds{1509371790}});
    t.set_bitrate(320);
    t.set_last_played_at(std::experimental::nullopt);
    t.set_last_accessed_at(c::system_clock::time_point{c::seconds{1509321600}});
    t.set_import_info(std::experimental::nullopt);
    t.set_album_art_id(2);
}

static void check_track_1(djinterop::track &t)
{
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK_EQUAL(*t.track_number(), 1);
    BOOST_CHECK_CLOSE(*t.bpm(), 123.0, 0.001);
    BOOST_CHECK_EQUAL(*t.year(), 2017);
    BOOST_CHECK_EQUAL(*t.title(), "Mad (Original Mix)");
    BOOST_CHECK_EQUAL(*t.artist(), "Dennis Cruz");
    BOOST_CHECK_EQUAL(*t.album(), "Mad EP");
    BOOST_CHECK_EQUAL(*t.genre(), "Tech House");
    BOOST_CHECK_EQUAL(*t.comment(), "Purchased at Beatport.com");
    BOOST_CHECK_EQUAL(*t.publisher(), "Stereo Productions");
    BOOST_CHECK(!t.composer());
    BOOST_CHECK(*t.key() == djinterop::musical_key::a_minor);
    BOOST_CHECK_EQUAL(
        t.relative_path(), "../01 - Dennis Cruz - Mad (Original Mix).mp3");
    BOOST_CHECK_EQUAL(
        t.filename(), "01 - Dennis Cruz - Mad (Original Mix).mp3");
    BOOST_CHECK_EQUAL(t.file_extension(), "mp3");
    BOOST_CHECK_EQUAL(
        t.last_modified_at()->time_since_epoch().count() *
            c::system_clock::period::num / c::system_clock::period::den,
        1509371790);
    BOOST_CHECK_EQUAL(*t.bitrate(), 320);
    BOOST_CHECK(!t.last_played_at());
    BOOST_CHECK_EQUAL(
        t.last_accessed_at()->time_since_epoch().count() *
            c::system_clock::period::num / c::system_clock::period::den,
        1509321600);
    BOOST_CHECK(!t.import_info());
    BOOST_CHECK_EQUAL(*t.album_art_id(), 2);
}

static void populate_example_track_2(djinterop::track &t)
{
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
    t.set_album_art_id(1);
}

static void check_track_2(djinterop::track &t)
{
    BOOST_CHECK(t.is_valid());
    BOOST_CHECK_EQUAL(*t.track_number(), 3);
    BOOST_CHECK_EQUAL(*t.bpm(), 128);
    BOOST_CHECK_EQUAL(*t.year(), 2018);
    BOOST_CHECK_EQUAL(*t.title(), "Made-up Track (Foo Bar Remix)");
    BOOST_CHECK_EQUAL(*t.artist(), "Not A Real Artist");
    BOOST_CHECK_EQUAL(*t.album(), "Fake Album");
    BOOST_CHECK_EQUAL(*t.genre(), "Progressive House");
    BOOST_CHECK_EQUAL(*t.comment(), "Comment goes here");
    BOOST_CHECK_EQUAL(*t.publisher(), "Here is the publisher text");
    BOOST_CHECK_EQUAL(*t.composer(), "And the composer text");
    BOOST_CHECK(*t.key() == djinterop::musical_key::c_major);
    BOOST_CHECK_EQUAL(
        t.relative_path(),
        "../03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
    BOOST_CHECK_EQUAL(
        t.filename(),
        "03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
    BOOST_CHECK_EQUAL(t.file_extension(), "flac");
    BOOST_CHECK_EQUAL(
        t.last_modified_at()->time_since_epoch().count() *
            c::system_clock::period::num / c::system_clock::period::den,
        1517413933);
    BOOST_CHECK_EQUAL(*t.bitrate(), 1411);
    BOOST_CHECK_EQUAL(
        t.last_played_at()->time_since_epoch().count() *
            c::system_clock::period::num / c::system_clock::period::den,
        1518739200);
    BOOST_CHECK_EQUAL(
        t.last_accessed_at()->time_since_epoch().count() *
            c::system_clock::period::num / c::system_clock::period::den,
        1518825600);
    BOOST_CHECK_EQUAL(
        t.import_info()->external_db_uuid(),
        "e535b170-26ef-4f30-8cb2-5b9fa4c2a27f");
    BOOST_CHECK_EQUAL(t.import_info()->external_track_id(), 123);
    BOOST_CHECK(!t.album_art_id());
}

BOOST_AUTO_TEST_CASE(all_track_ids__sample_db__expected_ids)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto results = db.tracks();

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 1);
    BOOST_CHECK_EQUAL(results[0].id(), 1);
}

BOOST_AUTO_TEST_CASE(tracks_by_relative_path__valid_path__expected_id)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto results = db.tracks_by_relative_path(
        "../01 - Dennis Cruz - Mad (Original Mix).mp3");

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 1);
    BOOST_CHECK_EQUAL(results[0].id(), 1);
}

BOOST_AUTO_TEST_CASE(tracks_by_relative_path__invalid_path__no_ids)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto results = db.tracks_by_relative_path("Does Not Exist.mp3");

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 0);
}

BOOST_AUTO_TEST_CASE(ctor__track1__correct_fields)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto t = *db.track_by_id(1);

    // Assert
    BOOST_CHECK_EQUAL(t.id(), 1);
    check_track_1(t);
}

BOOST_AUTO_TEST_CASE(ctor__track_deleted__throws)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act / Assert
    BOOST_CHECK(!db.track_by_id(123));
}

BOOST_AUTO_TEST_CASE(save__new_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);

    // Act
    auto t = db.create_track("");
    populate_example_track_1(t);

    // Assert
    BOOST_CHECK_NE(t.id(), 0);
    check_track_1(t);
    auto t_reloaded = *db.track_by_id(t.id());
    check_track_1(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(ctor_copy__saved_track__copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto t = db.create_track("");
    populate_example_track_1(t);

    // Act
    auto copy = t;

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), t.id());
    check_track_1(t);
    check_track_1(copy);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(save__existing_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto t = db.create_track("");
    populate_example_track_1(t);
    auto track_id = t.id();

    // Act
    populate_example_track_2(t);

    // Assert
    BOOST_CHECK_EQUAL(t.id(), track_id);
    auto t_reloaded = *db.track_by_id(t.id());
    check_track_2(t);
    check_track_2(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(op_copy_assign__saved_track__copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);

    auto t = db.create_track("");
    populate_example_track_1(t);

    // Act
    auto copy = t;

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), t.id());
    check_track_1(copy);
    remove_temp_dir(temp_dir);
}

