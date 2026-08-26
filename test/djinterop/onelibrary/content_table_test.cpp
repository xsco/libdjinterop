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

#include <memory>
#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/musical_key.hpp>

#include "../../../src/djinterop/onelibrary/content_table.hpp"
#include "../../../src/djinterop/onelibrary/onelibrary_context.hpp"
#include "../boost_test_printable.hpp"
#include "onelibrary_schema.hpp"

namespace utf = boost::unit_test;
namespace ol = djinterop::onelibrary;

namespace
{
/// Build a context over an in-memory database holding a small library.
///
/// The tables are read directly here, with no encryption in the way, which
/// keeps these tests clear of the deliberately expensive key derivation that
/// a real database needs.
std::shared_ptr<ol::onelibrary_context> make_context()
{
    sqlite::database db{":memory:"};
    for (const auto& statement : onelibrary_schema_statements())
        db << statement;

    db << "INSERT INTO artist VALUES (1, 'Aphex Twin', ''), "
          "(2, 'Squarepusher', '')";
    db << "INSERT INTO album VALUES (1, 'Selected Ambient', 1, 0, 0, '')";
    db << "INSERT INTO genre VALUES (1, 'Electro')";
    db << "INSERT INTO label VALUES (1, 'Warp')";
    db << R"(INSERT INTO "key" VALUES (1, 'F#m'), (2, 'Camelot 8A'))";

    // A fully populated track.
    db << "INSERT INTO content (content_id, title, bpmx100, length, trackNo, "
          "artist_id_artist, artist_id_composer, album_id, genre_id, "
          "label_id, key_id, djComment, rating, releaseYear, path, fileName, "
          "fileSize, bitrate, samplingRate) VALUES (1, 'Alpha Track', 12400, "
          "391, 7, 1, 2, 1, 1, 1, 1, 'feelin good', 4, 2025, "
          "'/Contents/Aphex/alpha.mp3', 'alpha.mp3', 6580703, 320, 44100)";

    // Metadata that is present but empty, and a key notation not understood.
    db << "INSERT INTO content (content_id, title, djComment, key_id, path) "
          "VALUES (2, 'Beta Track', '', 2, '/Contents/Various/beta.flac')";

    return std::make_shared<ol::onelibrary_context>("/device", std::move(db));
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("get() resolves the lookup tables"))
BOOST_AUTO_TEST_CASE(get__a_populated_row__resolves_its_lookups)
{
    // Arrange
    const ol::content_table content{make_context()};

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
    BOOST_CHECK_EQUAL(row->length_seconds.value(), 391);
    BOOST_CHECK_EQUAL(row->rating_stars.value(), 4);
    BOOST_CHECK_EQUAL(row->path.value(), "/Contents/Aphex/alpha.mp3");
}

BOOST_TEST_DECORATOR(*utf::description("get() reads an empty column as absent"))
BOOST_AUTO_TEST_CASE(get__an_empty_column__reads_as_absent)
{
    // Arrange
    const ol::content_table content{make_context()};

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
    const ol::content_table content{make_context()};

    // Act
    const auto row = content.get(404);

    // Assert
    BOOST_CHECK(!row);
}

BOOST_TEST_DECORATOR(*utf::description("all_ids() is ordered by identifier"))
BOOST_AUTO_TEST_CASE(all_ids__a_populated_table__is_ordered)
{
    // Arrange
    const ol::content_table content{make_context()};

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
    const ol::content_table content{make_context()};

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
    const ol::content_table content{make_context()};

    // Act / Assert
    BOOST_CHECK(content.exists(1));
    BOOST_CHECK(!content.exists(404));
}

BOOST_TEST_DECORATOR(
    *utf::description("to_snapshot() converts the units of the format"))
BOOST_AUTO_TEST_CASE(to_snapshot__a_populated_row__converts_its_units)
{
    // Arrange
    const ol::content_table content{make_context()};
    const auto row = content.get(1);
    BOOST_REQUIRE(row);

    // Act
    const auto snapshot = ol::to_snapshot(*row);

    // Assert
    BOOST_CHECK_CLOSE(snapshot.bpm.value(), 124.0, 0.001);
    BOOST_CHECK_EQUAL(snapshot.duration.value().count(), 391000);

    // Ratings are whole stars in the database, and out of one hundred here.
    BOOST_CHECK_EQUAL(snapshot.rating.value(), 80);

    // Paths are absolute within the device, and relative to it here.
    BOOST_CHECK_EQUAL(
        snapshot.relative_path.value(), "Contents/Aphex/alpha.mp3");

    // No sample count is recorded, so it follows from the duration and rate.
    BOOST_CHECK_EQUAL(snapshot.sample_count.value(), 391 * 44100);
    BOOST_CHECK_EQUAL(snapshot.publisher.value(), "Warp");
    BOOST_CHECK(snapshot.key == djinterop::musical_key::f_sharp_minor);
}

BOOST_TEST_DECORATOR(
    *utf::description("to_snapshot() for data the database does not hold"))
BOOST_AUTO_TEST_CASE(to_snapshot__any_row__has_no_performance_data)
{
    // Arrange
    const ol::content_table content{make_context()};
    const auto row = content.get(1);
    BOOST_REQUIRE(row);

    // Act
    const auto snapshot = ol::to_snapshot(*row);

    // Assert: rekordbox leaves these in the ANLZ files beside the database.
    BOOST_CHECK(snapshot.beatgrid.empty());
    BOOST_CHECK(snapshot.waveform.empty());
    BOOST_CHECK(snapshot.hot_cues.empty());
    BOOST_CHECK(snapshot.loops.empty());
}

BOOST_TEST_DECORATOR(
    *utf::description("parse_musical_key() for the notations rekordbox uses"))
BOOST_AUTO_TEST_CASE(parse_musical_key__known_notations__are_understood)
{
    // Act / Assert
    BOOST_CHECK(ol::parse_musical_key("C") == djinterop::musical_key::c_major);
    BOOST_CHECK(ol::parse_musical_key("Am") == djinterop::musical_key::a_minor);
    BOOST_CHECK(
        ol::parse_musical_key("F#m") == djinterop::musical_key::f_sharp_minor);
    BOOST_CHECK(
        ol::parse_musical_key("Bb") == djinterop::musical_key::b_flat_major);

    // The typographic accidentals mean the same as the ASCII ones.
    BOOST_CHECK(
        ol::parse_musical_key("F♯m") == djinterop::musical_key::f_sharp_minor);
    BOOST_CHECK(
        ol::parse_musical_key("B♭") == djinterop::musical_key::b_flat_major);
}

BOOST_TEST_DECORATOR(
    *utf::description("parse_musical_key() for notations it does not know"))
BOOST_AUTO_TEST_CASE(parse_musical_key__unknown_notations__are_not_guessed)
{
    // Act / Assert
    BOOST_CHECK(!ol::parse_musical_key(""));
    BOOST_CHECK(!ol::parse_musical_key("H"));

    // The Camelot and Open Key wheels are not read.
    BOOST_CHECK(!ol::parse_musical_key("8A"));
    BOOST_CHECK(!ol::parse_musical_key("Camelot 8A"));
}
