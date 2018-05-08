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
#include <string>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
namespace fs = boost::filesystem;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el2"};


static fs::path create_temp_dir()
{
    fs::path temp_dir{fs::temp_directory_path()};
    temp_dir /= fs::unique_path();
    if (!fs::create_directory(temp_dir))
    {
        throw std::runtime_error{"Failed to create tmp_dir"};
    }
    return temp_dir;
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
	BOOST_CHECK_EQUAL(t.path(), "../01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.filename(), "01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.file_extension(), "mp3");
	BOOST_CHECK_EQUAL(t.last_modified_at().time_since_epoch().count() *
			std::chrono::system_clock::period::num /
			std::chrono::system_clock::period::den, 1509371790); 
	BOOST_CHECK_EQUAL(t.bitrate(), 320);
	BOOST_CHECK_EQUAL(t.ever_played(), false);
	BOOST_CHECK_EQUAL(t.last_played_at().time_since_epoch().count(), 0);
	BOOST_CHECK_EQUAL(t.last_loaded_at().time_since_epoch().count() *
			std::chrono::system_clock::period::num /
			std::chrono::system_clock::period::den, 1509321600);
	BOOST_CHECK_EQUAL(t.is_imported(), false);
	BOOST_CHECK_EQUAL(t.external_database_id(), "");
	BOOST_CHECK_EQUAL(t.track_id_in_external_database(), 0);
	BOOST_CHECK_EQUAL(t.album_art_id(), 2);
	BOOST_CHECK_EQUAL(t.has_album_art(), true);
}

BOOST_AUTO_TEST_CASE (ctor_nonexistent_track__throws)
{
	// Arrange
	ep::database db{sample_path};

	// Act / Assert
	// Note the extra brackets around the invocation, because the preprocessor
	// doesn't understand curly braces!
	BOOST_CHECK_EXCEPTION((ep::track{db, 123}), ep::nonexistent_track,
			[](const ep::nonexistent_track &e) { return e.id() == 123; });
}

BOOST_AUTO_TEST_CASE (save__no_values__throws)
{
    // TODO
}

BOOST_AUTO_TEST_CASE (save__good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = ep::create_database(temp_dir.string(), ep::version_firmware_1_0_3);
    ep::track t{};
    // TODO - set values on new track

    // Act
    t.save(db);

    // Assert
    BOOST_CHECK(t.id() != 0);
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
	BOOST_CHECK_EQUAL(t.path(), "../01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.filename(), "01 - Dennis Cruz - Mad (Original Mix).mp3");
	BOOST_CHECK_EQUAL(t.file_extension(), "mp3");
	BOOST_CHECK_EQUAL(t.last_modified_at().time_since_epoch().count() *
			std::chrono::system_clock::period::num /
			std::chrono::system_clock::period::den, 1509371790); 
	BOOST_CHECK_EQUAL(t.bitrate(), 320);
	BOOST_CHECK_EQUAL(t.ever_played(), false);
	BOOST_CHECK_EQUAL(t.last_played_at().time_since_epoch().count(), 0);
	BOOST_CHECK_EQUAL(t.last_loaded_at().time_since_epoch().count() *
			std::chrono::system_clock::period::num /
			std::chrono::system_clock::period::den, 1509321600);
	BOOST_CHECK_EQUAL(t.is_imported(), false);
	BOOST_CHECK_EQUAL(t.external_database_id(), "");
	BOOST_CHECK_EQUAL(t.track_id_in_external_database(), 0);
	BOOST_CHECK_EQUAL(t.album_art_id(), 2);
	BOOST_CHECK_EQUAL(t.has_album_art(), true);
    fs::remove_all(temp_dir);
}
