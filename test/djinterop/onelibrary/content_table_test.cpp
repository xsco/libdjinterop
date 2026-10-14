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

#define BOOST_TEST_MODULE onelibrary_content_table_test
#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <string>

#include <djinterop/onelibrary/v1/content_table.hpp>

#include "../boost_test_printable.hpp"
#include "onelibrary_fixture.hpp"

namespace utf = boost::unit_test;

namespace
{
/// A device holding a small library, built once for the whole module.
const onelibrary_device& device()
{
    static const onelibrary_device instance{{
        "INSERT INTO artist VALUES (1, 'Aphex Twin', ''), "
        "(2, 'Squarepusher', '');",
        "INSERT INTO album VALUES (1, 'Selected Ambient', 1, 0, 0, '');",
        "INSERT INTO genre VALUES (1, 'Electro');",
        "INSERT INTO label VALUES (1, 'Warp');",
        R"(INSERT INTO "key" VALUES (1, 'F#m'), (2, 'Camelot 8A');)",

        // A fully populated track.
        "INSERT INTO content (content_id, title, bpmx100, length, trackNo, "
        "artist_id_artist, artist_id_composer, album_id, genre_id, "
        "label_id, key_id, djComment, rating, releaseYear, path, fileName, "
        "fileSize, bitrate, samplingRate) VALUES (1, 'Alpha Track', 12400, "
        "391, 7, 1, 2, 1, 1, 1, 1, 'feelin good', 4, 2025, "
        "'/Contents/Aphex/alpha.mp3', 'alpha.mp3', 6580703, 320, 44100);",

        // Metadata that is present but empty, and a key notation not
        // understood.
        "INSERT INTO content (content_id, title, djComment, key_id, path) "
        "VALUES (2, 'Beta Track', '', 2, '/Contents/Various/beta.flac');",
    }};

    return instance;
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("get() resolves the lookup tables"))
BOOST_AUTO_TEST_CASE(get__a_populated_row__resolves_its_lookups)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto row = content.get(1);

    // Assert
    BOOST_REQUIRE(row);
    BOOST_CHECK_EQUAL(row->title.value(), "Alpha Track");
    BOOST_CHECK_EQUAL(row->artist.value(), "Aphex Twin");
    BOOST_CHECK_EQUAL(row->composer.value(), "Squarepusher");
    BOOST_CHECK_EQUAL(row->album.value(), "Selected Ambient");
    BOOST_CHECK_EQUAL(row->genre.value(), "Electro");
    BOOST_CHECK_EQUAL(row->label.value(), "Warp");
    BOOST_CHECK_EQUAL(row->key.value(), "F#m");
    BOOST_CHECK_EQUAL(row->bpm_x100.value(), 12400);
    BOOST_CHECK(row->length.value() == std::chrono::seconds{391});
    BOOST_CHECK_EQUAL(row->rating_stars.value(), 4);
    BOOST_CHECK_EQUAL(row->path.value(), "/Contents/Aphex/alpha.mp3");
}

BOOST_TEST_DECORATOR(*utf::description("get() reads an empty column as absent"))
BOOST_AUTO_TEST_CASE(get__an_empty_column__reads_as_absent)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto row = content.get(2);

    // Assert
    BOOST_REQUIRE(row);
    BOOST_CHECK(!row->comment);
    BOOST_CHECK(!row->artist);
    BOOST_CHECK(!row->bpm_x100);
}

BOOST_TEST_DECORATOR(*utf::description("get() for a row that is not there"))
BOOST_AUTO_TEST_CASE(get__an_unknown_row__is_absent)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto row = content.get(404);

    // Assert
    BOOST_CHECK(!row);
}

BOOST_TEST_DECORATOR(*utf::description("all_ids() is ordered by identifier"))
BOOST_AUTO_TEST_CASE(all_ids__a_populated_table__is_ordered)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto ids = content.all_ids();

    // Assert
    BOOST_REQUIRE_EQUAL(ids.size(), 2u);
    BOOST_CHECK_EQUAL(ids[0], 1);
    BOOST_CHECK_EQUAL(ids[1], 2);
}

BOOST_TEST_DECORATOR(
    *utf::description("ids_by_path() matches with or without a separator"))
BOOST_AUTO_TEST_CASE(ids_by_path__either_spelling__finds_the_row)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto absolute = content.ids_by_path("/Contents/Aphex/alpha.mp3");
    const auto relative = content.ids_by_path("Contents/Aphex/alpha.mp3");

    // Assert
    BOOST_REQUIRE_EQUAL(absolute.size(), 1u);
    BOOST_CHECK_EQUAL(absolute[0], 1);
    BOOST_CHECK(relative == absolute);
}

BOOST_TEST_DECORATOR(*utf::description("exists() for present and absent rows"))
BOOST_AUTO_TEST_CASE(exists__present_and_absent_rows__reports_each)
{
    // Arrange
    const auto content = device().library.content();

    // Act / Assert
    BOOST_CHECK(content.exists(1));
    BOOST_CHECK(!content.exists(404));
}

BOOST_TEST_DECORATOR(
    *utf::description("get_xxx() reads one column the same as get() does"))
BOOST_AUTO_TEST_CASE(get_column__a_populated_row__matches_the_whole_row)
{
    // Arrange
    const auto content = device().library.content();

    // Act
    const auto row = content.get(1);

    // Assert
    BOOST_REQUIRE(row);
    BOOST_CHECK(content.get_title(1) == row->title);
    BOOST_CHECK(content.get_artist(1) == row->artist);
    BOOST_CHECK(content.get_composer(1) == row->composer);
    BOOST_CHECK(content.get_album(1) == row->album);
    BOOST_CHECK(content.get_label(1) == row->label);
    BOOST_CHECK(content.get_length(1) == row->length);
    BOOST_CHECK(content.get_rating_stars(1) == row->rating_stars);
    BOOST_CHECK(content.get_file_size(1) == row->file_size);
    BOOST_CHECK(content.get_bitrate(1) == row->bitrate);
    BOOST_CHECK(content.get_sampling_rate(1) == row->sampling_rate);

    // An empty column reads as absent on its own, too.
    BOOST_CHECK(!content.get_comment(2));
    BOOST_CHECK(!content.get_title(404));
}
