/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <engineprime/track.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE track_test

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
namespace fs = boost::filesystem;
namespace c = std::chrono;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el2"};

namespace boost {
namespace test_tools {
    template<>
    struct print_log_value<ep::musical_key>
    {
        void operator ()(std::ostream &os, ep::musical_key o)
        {
            os << static_cast<int>(o);
        }
    };
} // test_tools
} // boost


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

static void populate_example_track_1(ep::track &t)
{
    t.set_track_number(1);
    t.set_duration(std::chrono::seconds{396});
    t.set_bpm(123);
    t.set_year(2017);
    t.set_title("Mad (Original Mix)");
	t.set_artist("Dennis Cruz");
	t.set_album("Mad EP");
	t.set_genre("Tech House");
	t.set_comment("Purchased at Beatport.com");
	t.set_publisher("Stereo Productions");
	t.set_composer("");
    t.set_key(ep::musical_key::a_minor);
	t.set_path("../01 - Dennis Cruz - Mad (Original Mix).mp3");
	t.set_filename("01 - Dennis Cruz - Mad (Original Mix).mp3");
	t.set_file_extension("mp3");
    t.set_last_modified_at(c::system_clock::time_point{c::seconds{1509371790}});
    t.set_bitrate(320);
    t.set_ever_played(false);
    t.set_last_played_at(c::system_clock::time_point{});
    t.set_last_loaded_at(c::system_clock::time_point{c::seconds{1509321600}});
    t.set_imported(false);
    t.set_album_art_id(2);
}

static void check_track_1(ep::track &t)
{
	BOOST_CHECK_EQUAL(t.track_number(), 1);
	BOOST_CHECK_EQUAL(t.duration().count(), 396);
	BOOST_CHECK_EQUAL(t.bpm(), 123);
	BOOST_CHECK_EQUAL(t.year(), 2017);
	BOOST_CHECK_EQUAL(t.title(), "Mad (Original Mix)");
	BOOST_CHECK_EQUAL(t.artist(), "Dennis Cruz");
	BOOST_CHECK_EQUAL(t.album(), "Mad EP");
	BOOST_CHECK_EQUAL(t.genre(), "Tech House");
	BOOST_CHECK_EQUAL(t.comment(), "Purchased at Beatport.com");
	BOOST_CHECK_EQUAL(t.publisher(), "Stereo Productions");
	BOOST_CHECK_EQUAL(t.composer(), "");
    BOOST_CHECK_EQUAL(t.key(), ep::musical_key::a_minor);
	BOOST_CHECK_EQUAL(t.path(), "../01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.filename(), "01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.file_extension(), "mp3");
	BOOST_CHECK_EQUAL(t.last_modified_at().time_since_epoch().count() *
			c::system_clock::period::num /
			c::system_clock::period::den, 1509371790); 
	BOOST_CHECK_EQUAL(t.bitrate(), 320);
	BOOST_CHECK_EQUAL(t.ever_played(), false);
	BOOST_CHECK_EQUAL(t.last_played_at().time_since_epoch().count(), 0);
	BOOST_CHECK_EQUAL(t.last_loaded_at().time_since_epoch().count() *
			c::system_clock::period::num /
			c::system_clock::period::den, 1509321600);
	BOOST_CHECK_EQUAL(t.imported(), false);
	BOOST_CHECK_EQUAL(t.external_database_uuid(), "");
	BOOST_CHECK_EQUAL(t.track_id_in_external_database(), 0);
	BOOST_CHECK_EQUAL(t.album_art_id(), 2);
	BOOST_CHECK_EQUAL(t.has_album_art(), true);
    BOOST_CHECK(t.has_title());
    BOOST_CHECK(t.has_artist());
    BOOST_CHECK(t.has_album());
    BOOST_CHECK(t.has_genre());
    BOOST_CHECK(t.has_comment());
    BOOST_CHECK(t.has_publisher());
    BOOST_CHECK(!t.has_composer());
    BOOST_CHECK(t.has_key());
}

static void populate_example_track_2(ep::track &t)
{
    t.set_track_number(3);
    t.set_duration(std::chrono::seconds{301});
    t.set_bpm(128);
    t.set_year(2018);
    t.set_title("Made-up Track (Foo Bar Remix)");
	t.set_artist("Not A Real Artist");
	t.set_album("Fake Album");
	t.set_genre("Progressive House");
	t.set_comment("Comment goes here");
	t.set_publisher("Here is the publisher text");
	t.set_composer("And the composer text");
    t.set_key(ep::musical_key::c_major);
	t.set_path("../03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
	t.set_filename("03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
	t.set_file_extension("flac");
    t.set_last_modified_at(c::system_clock::time_point{c::seconds{1517413933}});
    t.set_bitrate(1411);
    t.set_ever_played(true);
    t.set_last_played_at(c::system_clock::time_point{c::seconds{1518739200}});
    t.set_last_loaded_at(c::system_clock::time_point{c::seconds{1518815683}});
    t.set_imported(true, "e535b170-26ef-4f30-8cb2-5b9fa4c2a27f", 123);
    t.set_album_art_id(1);
}

static void check_track_2(ep::track &t)
{
	BOOST_CHECK_EQUAL(t.track_number(), 3);
	BOOST_CHECK_EQUAL(t.duration().count(), 301);
	BOOST_CHECK_EQUAL(t.bpm(), 128);
	BOOST_CHECK_EQUAL(t.year(), 2018);
	BOOST_CHECK_EQUAL(t.title(), "Made-up Track (Foo Bar Remix)");
	BOOST_CHECK_EQUAL(t.artist(), "Not A Real Artist");
	BOOST_CHECK_EQUAL(t.album(), "Fake Album");
	BOOST_CHECK_EQUAL(t.genre(), "Progressive House");
	BOOST_CHECK_EQUAL(t.comment(), "Comment goes here");
	BOOST_CHECK_EQUAL(t.publisher(), "Here is the publisher text");
	BOOST_CHECK_EQUAL(t.composer(), "And the composer text");
    BOOST_CHECK_EQUAL(t.key(), ep::musical_key::c_major);
	BOOST_CHECK_EQUAL(t.path(), "../03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
	BOOST_CHECK_EQUAL(t.filename(), "03 - Not A Real Artist - Made-up Track (Foo Bar Remix).flac");
	BOOST_CHECK_EQUAL(t.file_extension(), "flac");
	BOOST_CHECK_EQUAL(t.last_modified_at().time_since_epoch().count() *
			c::system_clock::period::num /
			c::system_clock::period::den, 1517413933); 
	BOOST_CHECK_EQUAL(t.bitrate(), 1411);
	BOOST_CHECK_EQUAL(t.ever_played(), true);
	BOOST_CHECK_EQUAL(t.last_played_at().time_since_epoch().count() *
            c::system_clock::period::num /
            c::system_clock::period::den, 1518739200);
	BOOST_CHECK_EQUAL(t.last_loaded_at().time_since_epoch().count() *
			c::system_clock::period::num /
			c::system_clock::period::den, 1518815683);
	BOOST_CHECK_EQUAL(t.imported(), true);
	BOOST_CHECK_EQUAL(t.external_database_uuid(), "e535b170-26ef-4f30-8cb2-5b9fa4c2a27f");
	BOOST_CHECK_EQUAL(t.track_id_in_external_database(), 123);
	BOOST_CHECK_EQUAL(t.album_art_id(), 1);
	BOOST_CHECK_EQUAL(t.has_album_art(), false);
    BOOST_CHECK(t.has_title());
    BOOST_CHECK(t.has_artist());
    BOOST_CHECK(t.has_album());
    BOOST_CHECK(t.has_genre());
    BOOST_CHECK(t.has_comment());
    BOOST_CHECK(t.has_publisher());
    BOOST_CHECK(t.has_composer());
    BOOST_CHECK(t.has_key());
}

BOOST_AUTO_TEST_CASE (all_track_ids__sample_db__expected_ids)
{
    // Arrange
    ep::database db{sample_path};
    
    // Act
    auto results = ep::all_track_ids(db);
    
    // Assert
    BOOST_CHECK_EQUAL(results.size(), 1);
    BOOST_CHECK_EQUAL(results[0], 1);
}

BOOST_AUTO_TEST_CASE (ctor__track1__correct_fields)
{
	// Arrange
	ep::database db{sample_path};

	// Act
	ep::track t{db, 1};

	// Assert
	BOOST_CHECK_EQUAL(t.id(), 1);
    check_track_1(t);
}

BOOST_AUTO_TEST_CASE (ctor__nonexistent_track__throws)
{
	// Arrange
	ep::database db{sample_path};

	// Act / Assert
	// Note the extra brackets around the invocation, because the preprocessor
	// doesn't understand curly braces!
	BOOST_CHECK_EXCEPTION((ep::track{db, 123}), ep::nonexistent_track,
			[](const ep::nonexistent_track &e) { return e.id() == 123; });
}

BOOST_AUTO_TEST_CASE (setters__good_values__values_stored)
{
    // Arrange
    ep::track t{};

    // Act
    populate_example_track_1(t);

    // Assert
    BOOST_CHECK_EQUAL(t.id(), 0);
    check_track_1(t);
}

BOOST_AUTO_TEST_CASE (save__new_track_no_values__throws)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};

    // Act/Assert
    BOOST_CHECK_THROW(t.save(db), std::invalid_argument);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__new_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};
    populate_example_track_1(t);

    // Act
    t.save(db);

    // Assert
    BOOST_CHECK_NE(t.id(), 0);
    check_track_1(t);
    ep::track t_reloaded{db, t.id()};
    check_track_1(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (ctor_copy__saved_track__zero_id_and_copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};
    populate_example_track_1(t);
    t.save(db);

    // Act
    ep::track copy{t};

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), 0);
    check_track_1(t);
    check_track_1(copy);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__existing_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};
    populate_example_track_1(t);
    t.save(db);
    auto track_id = t.id();
    populate_example_track_2(t);

    // Act
    t.save(db);

    // Assert
    BOOST_CHECK_EQUAL(t.id(), track_id);
    ep::track t_reloaded{db, t.id()};
    check_track_2(t);
    check_track_2(t_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (op_copy_assign__saved_track__zero_id_and_copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};
    populate_example_track_1(t);
    t.save(db);

    // Act
    auto copy = t;

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), 0);
    check_track_1(copy);
    remove_temp_dir(temp_dir);
}

