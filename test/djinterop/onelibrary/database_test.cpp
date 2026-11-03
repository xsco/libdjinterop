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

#define BOOST_TEST_MODULE onelibrary_database_test
#include <boost/test/included/unit_test.hpp>

#include <filesystem>
#include <string>

#include <djinterop/djinterop.hpp>
#include <djinterop/onelibrary/onelibrary.hpp>
#include <djinterop/onelibrary/v1/library.hpp>

#include "../boost_test_printable.hpp"
#include "onelibrary_fixture.hpp"

namespace utf = boost::unit_test;
namespace ol = djinterop::onelibrary;
namespace olv1 = djinterop::onelibrary::v1;

namespace
{
const std::string passphrase = "a passphrase for the fixture";

/// A device holding a small OneLibrary export, built once for the whole
/// module.
///
/// The data covers the awkward cases: metadata that is absent, present but
/// empty, and non-ASCII; a key notation that is not understood; and a nested
/// playlist.
///
/// Key derivation is deliberately expensive -- the format stretches the
/// passphrase 256,000 times -- so a test that built its own device and opened
/// it would pay for that twice over, every time.
const onelibrary_device& device_fixture()
{
    static const onelibrary_device instance{
        {
            "INSERT INTO artist VALUES (1, 'Aphex Twin', 'APHEX TWIN'), "
            "(2, 'Кто-то', ''), (3, 'A Composer', '');",
            "INSERT INTO album VALUES (1, 'Album One', 1, 0, 0, '');",
            "INSERT INTO genre VALUES (1, 'Electro');",
            "INSERT INTO label VALUES (1, 'Warp');",
            "INSERT INTO \"key\" VALUES (1, 'F#m'), (2, 'Bb'), "
            "(3, 'Camelot 8A');",

            // A fully populated track.
            "INSERT INTO content (content_id, title, bpmx100, length, "
            "trackNo, artist_id_artist, artist_id_composer, album_id, "
            "genre_id, label_id, key_id, djComment, rating, releaseYear, "
            "path, fileName, fileSize, bitrate, samplingRate, masterDbId, "
            "analysisDataFilePath, color_id) VALUES (1, 'Alpha Track', "
            "12400, 391, 7, 1, 3, 1, 1, 1, 1, 'feelin good', 4, 2025, "
            "'/Contents/Aphex/alpha.mp3', 'alpha.mp3', 6580703, 320, 44100, "
            "4056018032, '/PIONEER/USBANLZ/P016/0000875e/ANLZ0000.DAT', 6);",

            // Non-ASCII metadata, an empty comment, and no rating.
            "INSERT INTO content (content_id, title, bpmx100, length, "
            "artist_id_artist, key_id, djComment, rating, path, "
            "samplingRate, masterDbId, color_id) VALUES (2, 'Бета Трек', "
            "12800, 245, 2, 2, '', 0, '/Contents/Various/beta.flac', 48000, "
            "4056018032, 0);",

            // Almost nothing set, and a key notation this library does not
            // know.
            "INSERT INTO content (content_id, title, key_id, rating, path) "
            "VALUES (3, 'Gamma', 3, 5, '/Contents/Third/gamma.wav');",

            "INSERT INTO playlist VALUES (1, 1, 'Sets', 0, 0, 0), "
            "(2, 1, 'Warm Up', 0, 0, 1), (3, 2, 'Peak Time', 0, 0, 0);",
            "INSERT INTO playlist_content VALUES (2, 1, 1), (2, 3, 2), "
            "(3, 2, 1);",
            "UPDATE property SET deviceName = 'FIXTURE', "
            "numberOfContents = 3, createdDate = '2026-01-01';",
        },
        passphrase};

    return instance;
}

/// The shared device, opened once as a library.
const olv1::library& loaded_library()
{
    return device_fixture().library;
}

/// The shared device, through the same connection as `loaded_library()`.
djinterop::database loaded_database()
{
    return loaded_library().database();
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description(
    "database_exists() finds a device by its root or by its file"))
BOOST_AUTO_TEST_CASE(database_exists__a_device__is_found)
{
    const auto& device = device_fixture().root;

    BOOST_CHECK(ol::database_exists(device));
    BOOST_CHECK(ol::database_exists(device / ol::database_relative_path));
    BOOST_CHECK(!ol::database_exists(device / "nowhere"));
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() reports the identity of a device"))
BOOST_AUTO_TEST_CASE(load_database__a_device__reports_its_identity)
{
    const auto& device = device_fixture().root;
    auto db = loaded_database();

    BOOST_CHECK_EQUAL(db.version_name(), "OneLibrary 1000");
    BOOST_CHECK_EQUAL(db.uuid(), "4056018032");
    BOOST_CHECK_EQUAL(db.directory(), device);

    // The format keeps one tree, used as both playlists and crates.
    BOOST_CHECK(!db.supports_feature(
        djinterop::feature::playlists_and_crates_are_distinct));
    BOOST_CHECK(
        db.supports_feature(djinterop::feature::supports_nested_playlists));
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() accepts the database file itself"))
BOOST_AUTO_TEST_CASE(load_database__the_database_file_itself__is_accepted)
{
    const auto& device = device_fixture().root;
    auto db =
        ol::load_database(device / ol::database_relative_path, passphrase);

    BOOST_CHECK_EQUAL(db.tracks().size(), 3u);

    // Naming the file still identifies the device it belongs to, because
    // track paths are relative to that and not to the file.
    BOOST_CHECK_EQUAL(db.directory(), device);
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() refuses a wrong passphrase"))
BOOST_AUTO_TEST_CASE(load_database__a_wrong_passphrase__is_refused)
{
    const auto& device = device_fixture().root;

    BOOST_CHECK_THROW(
        ol::load_database(device, "some other passphrase"),
        djinterop::unsupported_database);
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() throws when no database is present"))
BOOST_AUTO_TEST_CASE(load_database__no_database__throws)
{
    temporary_directory temp_dir;

    BOOST_CHECK_THROW(
        ol::load_database(temp_dir.temp_dir, passphrase),
        djinterop::database_not_found);
}

BOOST_TEST_DECORATOR(
    *utf::description("tracks() reads every field of a populated track"))
BOOST_AUTO_TEST_CASE(tracks__a_populated_track__reads_every_field)
{
    auto db = loaded_database();

    const auto track = db.track_by_id(1);
    BOOST_REQUIRE(track);

    BOOST_CHECK_EQUAL(track->title().value(), "Alpha Track");
    BOOST_CHECK_EQUAL(track->artist().value(), "Aphex Twin");
    BOOST_CHECK_EQUAL(track->composer().value(), "A Composer");
    BOOST_CHECK_EQUAL(track->album().value(), "Album One");
    BOOST_CHECK_EQUAL(track->genre().value(), "Electro");
    BOOST_CHECK_EQUAL(track->publisher().value(), "Warp");
    BOOST_CHECK_EQUAL(track->comment().value(), "feelin good");

    // Tempo is stored in hundredths of a beat per minute.
    BOOST_CHECK_CLOSE(track->bpm().value(), 124.0, 0.001);

    // Duration is stored in whole seconds.
    BOOST_CHECK_EQUAL(track->duration().value().count(), 391000);

    BOOST_CHECK_EQUAL(track->track_number().value(), 7);
    BOOST_CHECK_EQUAL(track->year().value(), 2025);
    BOOST_CHECK_EQUAL(track->bitrate().value(), 320);
    BOOST_CHECK_CLOSE(track->sample_rate().value(), 44100.0, 0.001);

    // Four stars of five, on djinterop's scale of one hundred.
    BOOST_CHECK_EQUAL(track->rating().value(), 80);

    BOOST_CHECK(track->key().value() == djinterop::musical_key::f_sharp_minor);

    // Paths are absolute within the device; djinterop wants them relative to
    // its root.
    BOOST_CHECK_EQUAL(track->relative_path(), "Contents/Aphex/alpha.mp3");
    BOOST_CHECK_EQUAL(track->filename(), "alpha.mp3");
    BOOST_CHECK_EQUAL(track->file_extension(), "mp3");
}

BOOST_TEST_DECORATOR(
    *utf::description("tracks() reads sparse metadata as absent"))
BOOST_AUTO_TEST_CASE(tracks__sparse_metadata__reads_as_absent)
{
    auto db = loaded_database();

    const auto track = db.track_by_id(3);
    BOOST_REQUIRE(track);

    BOOST_CHECK_EQUAL(track->title().value(), "Gamma");
    BOOST_CHECK(!track->artist());
    BOOST_CHECK(!track->album());
    BOOST_CHECK(!track->bpm());
    BOOST_CHECK(!track->duration());
    BOOST_CHECK(!track->bitrate());
    BOOST_CHECK(!track->sample_rate());
    BOOST_CHECK(!track->sample_count());

    // A key notation this library does not know reads as no key, rather than
    // as a guess.
    BOOST_CHECK(!track->key());

    // An empty comment is metadata the track does not carry.
    const auto other = db.track_by_id(2);
    BOOST_REQUIRE(other);
    BOOST_CHECK(!other->comment());
    BOOST_CHECK_EQUAL(other->title().value(), "Бета Трек");
    BOOST_CHECK_EQUAL(other->artist().value(), "Кто-то");
    BOOST_CHECK(other->key().value() == djinterop::musical_key::b_flat_major);
}

BOOST_TEST_DECORATOR(*utf::description(
    "tracks() implies a sample count from the duration and rate"))
BOOST_AUTO_TEST_CASE(tracks__a_known_duration_and_rate__implies_a_sample_count)
{
    auto db = loaded_database();

    const auto track = db.track_by_id(1);
    BOOST_REQUIRE(track);

    // The database records whole seconds and no sample count, so the count is
    // only recoverable to that precision.
    BOOST_CHECK_EQUAL(track->sample_count().value(), 391ull * 44100);
}

BOOST_TEST_DECORATOR(
    *utf::description("tracks_by_relative_path() finds a track by its path"))
BOOST_AUTO_TEST_CASE(tracks_by_relative_path__a_known_path__finds_the_track)
{
    auto db = loaded_database();

    const auto found = db.tracks_by_relative_path("Contents/Aphex/alpha.mp3");
    BOOST_REQUIRE_EQUAL(found.size(), 1u);
    BOOST_CHECK_EQUAL(found.front().id(), 1);

    // The same path written the way the database stores it.
    BOOST_CHECK_EQUAL(
        db.tracks_by_relative_path("/Contents/Aphex/alpha.mp3").size(), 1u);

    BOOST_CHECK(db.tracks_by_relative_path("nothing/here.mp3").empty());
}

BOOST_TEST_DECORATOR(
    *utf::description("track_by_id() for a track that is not there"))
BOOST_AUTO_TEST_CASE(track_by_id__an_unknown_track__is_absent)
{
    auto db = loaded_database();

    BOOST_CHECK(!db.track_by_id(999));
}

BOOST_TEST_DECORATOR(*utf::description("playlists walk a nested tree"))
BOOST_AUTO_TEST_CASE(playlists__a_nested_tree__is_walked)
{
    auto db = loaded_database();

    const auto roots = db.root_playlists();
    BOOST_REQUIRE_EQUAL(roots.size(), 2u);
    BOOST_CHECK_EQUAL(roots[0].name(), "Sets");
    BOOST_CHECK_EQUAL(roots[1].name(), "Peak Time");

    // The folder itself holds no tracks; its child does.
    BOOST_CHECK(roots[0].tracks().empty());

    const auto children = roots[0].children();
    BOOST_REQUIRE_EQUAL(children.size(), 1u);
    BOOST_CHECK_EQUAL(children[0].name(), "Warm Up");

    const auto tracks = children[0].tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 2u);
    BOOST_CHECK_EQUAL(tracks[0].id(), 1);
    BOOST_CHECK_EQUAL(tracks[1].id(), 3);

    BOOST_REQUIRE(children[0].parent());
    BOOST_CHECK_EQUAL(children[0].parent()->name(), "Sets");
    BOOST_CHECK(!roots[0].parent());
}

BOOST_TEST_DECORATOR(*utf::description("crates see the same tree as playlists"))
BOOST_AUTO_TEST_CASE(crates__the_same_tree_as_playlists__is_seen)
{
    auto db = loaded_database();

    const auto roots = db.root_crates();
    BOOST_REQUIRE_EQUAL(roots.size(), 2u);

    const auto sets = db.root_crate_by_name("Sets");
    BOOST_REQUIRE(sets);
    BOOST_CHECK_EQUAL(sets->descendants().size(), 1u);

    const auto warm_up = sets->sub_crate_by_name("Warm Up");
    BOOST_REQUIRE(warm_up);
    BOOST_CHECK_EQUAL(warm_up->tracks().size(), 2u);

    BOOST_CHECK(!db.root_crate_by_name("No Such Crate"));

    const auto track = db.track_by_id(1);
    BOOST_REQUIRE(track);
    const auto containing = track->containing_crates();
    BOOST_REQUIRE_EQUAL(containing.size(), 1u);
    BOOST_CHECK_EQUAL(containing.front().name(), "Warm Up");
}

BOOST_TEST_DECORATOR(*utf::description("every kind of write is refused"))
BOOST_AUTO_TEST_CASE(writes__every_kind__are_refused)
{
    auto db = loaded_database();

    BOOST_CHECK_THROW(
        db.create_root_crate("New"), djinterop::unsupported_operation);
    BOOST_CHECK_THROW(
        db.create_root_playlist("New"), djinterop::unsupported_operation);
    BOOST_CHECK_THROW(
        db.create_track(djinterop::track_snapshot{}),
        djinterop::unsupported_operation);

    auto track = db.tracks().front();
    BOOST_CHECK_THROW(
        track.set_title(std::string{"New"}), djinterop::unsupported_operation);
    BOOST_CHECK_THROW(track.set_bpm(100.0), djinterop::unsupported_operation);

    auto crate = db.root_crates().front();
    BOOST_CHECK_THROW(crate.set_name("New"), djinterop::unsupported_operation);
    BOOST_CHECK_THROW(
        crate.create_sub_crate("New"), djinterop::unsupported_operation);
    BOOST_CHECK_THROW(db.remove_crate(crate), djinterop::unsupported_operation);
}

BOOST_TEST_DECORATOR(*utf::description("crates walk a deep tree breadth first"))
BOOST_AUTO_TEST_CASE(
    crates__a_tree_several_levels_deep__is_walked_breadth_first)
{
    // The shared fixture is only one level deep, which does not exercise the
    // recursion in `descendant_ids` or the order it returns.
    //
    // Root
    //  +- Middle A        (sequence 1)
    //  |   +- Leaf A      (sequence 1)
    //  +- Middle B        (sequence 2)
    const onelibrary_device deep{{
        "INSERT INTO playlist VALUES (1, 1, 'Root', 0, 0, 0), "
        "(2, 1, 'Middle A', 0, 0, 1), (3, 2, 'Middle B', 0, 0, 1), "
        "(4, 1, 'Leaf A', 0, 0, 2);",
    }};

    auto loaded = deep.library.database();
    const auto root = loaded.root_crate_by_name("Root");
    BOOST_REQUIRE(root);

    const auto descendants = root->descendants();
    BOOST_REQUIRE_EQUAL(descendants.size(), 3u);
    BOOST_CHECK_EQUAL(descendants[0].name(), "Middle A");
    BOOST_CHECK_EQUAL(descendants[1].name(), "Middle B");
    BOOST_CHECK_EQUAL(descendants[2].name(), "Leaf A");

    // Children stay one level deep, unlike descendants.
    BOOST_CHECK_EQUAL(root->children().size(), 2u);
}

BOOST_TEST_DECORATOR(*utf::description(
    "load_database() reads a database whose log was checkpointed"))
BOOST_AUTO_TEST_CASE(load_database__a_checkpointed_log__is_read)
{
    // A real export is written in WAL mode and checkpointed on eject, which
    // removes the log but leaves the header declaring the database
    // write-ahead-logged.  SQLite will not open one of those read-only unless
    // it can create the log again.
    //
    // Creating a database in WAL mode does the same: closing the connection
    // that wrote it checkpoints the log into the database.
    temporary_directory temp_dir;
    const auto device = temp_dir.temp_dir / "device";
    const auto scripts = temp_dir.temp_dir / "scripts";
    write_script_on_schema(
        scripts, {
                     "PRAGMA journal_mode = WAL;",
                     "INSERT INTO content (content_id, title, path) "
                     "VALUES (1, 'Checkpointed', '/a.mp3');",
                 });
    ol::create_database_from_scripts(device, scripts, passphrase);

    // Creating it also loads it, and that read-only connection leaves an empty
    // log and index behind, so remove them to leave the header alone.
    const auto path = device / ol::database_relative_path;
    std::filesystem::remove(std::filesystem::path{path} += "-wal");
    std::filesystem::remove(std::filesystem::path{path} += "-shm");

    auto loaded = ol::load_database(path, passphrase);
    const auto tracks = loaded.tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 1u);
    BOOST_CHECK_EQUAL(tracks[0].title().value(), "Checkpointed");
}

BOOST_TEST_DECORATOR(
    *utf::description("verify() rejects a database missing a table"))
BOOST_AUTO_TEST_CASE(verify__a_database_missing_a_table__is_rejected)
{
    const std::vector<std::string> tables{
        "content",            "artist",
        "album",              "genre",
        "label",              "key",
        "color",              "image",
        "cue",                "playlist",
        "playlist_content",   "history",
        "history_content",    "hotCueBankList",
        "hotCueBankList_cue", "myTag",
        "myTag_content",      "menuItem",
        "category",           "sort",
        "property",           "recommendedLike",
    };

    for (const auto& table : tables)
    {
        BOOST_TEST_CONTEXT(table)
        {
            temporary_directory temp_dir;
            const auto scripts = temp_dir.temp_dir / "scripts";
            write_script_on_schema(scripts, {"DROP TABLE " + table + ";"});

            BOOST_CHECK_THROW(
                ol::create_database_from_scripts(
                    temp_dir.temp_dir / "device", scripts, passphrase),
                djinterop::database_inconsistency);
        }
    }
}

BOOST_TEST_DECORATOR(*utf::description(
    "verify() rejects a database missing an index"))
BOOST_AUTO_TEST_CASE(verify__a_database_missing_an_index__is_rejected)
{
    const std::vector<std::string> indexes{
        "index_playlist_content_playlist_id",
        "index_myTag_content_myTag_id",
        "index_myTag_content_content_id",
        "index_hotCueBankList_cue_hotCueBankList_id",
    };

    for (const auto& index : indexes)
    {
        BOOST_TEST_CONTEXT(index)
        {
            temporary_directory temp_dir;
            const auto scripts = temp_dir.temp_dir / "scripts";
            write_script_on_schema(scripts, {"DROP INDEX " + index + ";"});

            BOOST_CHECK_THROW(
                ol::create_database_from_scripts(
                    temp_dir.temp_dir / "device", scripts, passphrase),
                djinterop::database_inconsistency);
        }
    }
}

BOOST_TEST_DECORATOR(*utf::description(
    "verify() rejects a database without exactly one versioned property row"))
BOOST_AUTO_TEST_CASE(verify__a_bad_property_table__is_rejected)
{
    // Every export carries exactly one `property` row, recording `dbVersion`.
    const std::vector<std::vector<std::string>> bad_property_tables{
        {"DELETE FROM property;"},
        {"INSERT INTO property (dbVersion) VALUES ('1000');"},
        {"UPDATE property SET dbVersion = NULL;"},
        {"UPDATE property SET dbVersion = '';"},
    };

    for (const auto& statements : bad_property_tables)
    {
        BOOST_TEST_CONTEXT(statements.front())
        {
            temporary_directory temp_dir;
            const auto scripts = temp_dir.temp_dir / "scripts";
            write_script_on_schema(scripts, statements);

            BOOST_CHECK_THROW(
                ol::create_database_from_scripts(
                    temp_dir.temp_dir / "device", scripts, passphrase),
                djinterop::database_inconsistency);
        }
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("library reaches the device through both interfaces"))
BOOST_AUTO_TEST_CASE(library__a_device__is_read_through_its_database)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act
    auto db = lib.database();

    // Assert
    BOOST_CHECK_EQUAL(lib.directory(), device_fixture().root);
    BOOST_CHECK_EQUAL(db.directory(), device_fixture().root);
    BOOST_CHECK_EQUAL(db.tracks().size(), 3u);
}

BOOST_TEST_DECORATOR(*utf::description(
    "content_table::get_analysis_path() gives the path the device records"))
BOOST_AUTO_TEST_CASE(get_analysis_path__a_track_with_analysis_data__is_read)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act
    const auto path = lib.content().get_analysis_path(1);

    // Assert
    BOOST_REQUIRE(path);
    BOOST_CHECK_EQUAL(*path, "/PIONEER/USBANLZ/P016/0000875e/ANLZ0000.DAT");
}

BOOST_TEST_DECORATOR(*utf::description(
    "content_table::get_analysis_path() for a track that carries none, and "
    "for one that is not there"))
BOOST_AUTO_TEST_CASE(get_analysis_path__no_analysis_data__is_absent)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK(!lib.content().get_analysis_path(2));
    BOOST_CHECK(!lib.content().get_analysis_path(1234));
}

BOOST_TEST_DECORATOR(*utf::description(
    "content_table::get_key() gives back the notation the device holds"))
BOOST_AUTO_TEST_CASE(get_key__any_track__is_the_notation_on_the_device)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK_EQUAL(lib.content().get_key(1).value(), "F#m");
    BOOST_CHECK_EQUAL(lib.content().get_key(2).value(), "Bb");

    // A notation the library does not parse is still given back whole.
    BOOST_CHECK_EQUAL(lib.content().get_key(3).value(), "Camelot 8A");

    BOOST_CHECK(!lib.content().get_key(1234));
}

BOOST_TEST_DECORATOR(*utf::description(
    "content_table::get_color_id() reads the colour of a track"))
BOOST_AUTO_TEST_CASE(get_color_id__marked_and_unmarked_tracks__reports_each)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK_EQUAL(lib.content().get_color_id(1).value(), 6);

    // The low-level API reports what the column holds, and `COLOR_ID_NONE` is
    // what an unmarked track carries.
    BOOST_CHECK_EQUAL(
        lib.content().get_color_id(2).value(), olv1::COLOR_ID_NONE);
    BOOST_CHECK(!lib.content().get_color_id(3));
    BOOST_CHECK(!lib.content().get_color_id(1234));
}

BOOST_TEST_DECORATOR(*utf::description(
    "property_table::get_db_version() reports the schema version"))
BOOST_AUTO_TEST_CASE(get_db_version__a_device__is_the_version_it_records)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK_EQUAL(
        lib.property().get_db_version().value(), olv1::supported_db_version);
}

BOOST_TEST_DECORATOR(
    *utf::description("snapshot() converts the units of the format"))
BOOST_AUTO_TEST_CASE(snapshot__a_populated_track__converts_its_units)
{
    // Arrange
    auto db = loaded_database();
    const auto track = db.track_by_id(1);
    BOOST_REQUIRE(track);

    // Act
    const auto snapshot = track->snapshot();

    // Assert
    BOOST_CHECK_CLOSE(snapshot.bpm.value(), 124.0, 0.001);
    BOOST_CHECK_EQUAL(snapshot.duration.value().count(), 391000);

    // Ratings are whole stars in the database, and out of one hundred here.
    BOOST_CHECK_EQUAL(snapshot.rating.value(), 80);

    // Paths in the database begin with a separator, and here they do not.
    BOOST_CHECK_EQUAL(
        snapshot.relative_path.value(), "Contents/Aphex/alpha.mp3");

    // No sample count is recorded, so it follows from the duration and rate.
    BOOST_CHECK_EQUAL(snapshot.sample_count.value(), 391ull * 44100);
    BOOST_CHECK_EQUAL(snapshot.publisher.value(), "Warp");
    BOOST_CHECK(snapshot.key == djinterop::musical_key::f_sharp_minor);

    // rekordbox leaves these in the ANLZ files beside the database.
    BOOST_CHECK(snapshot.beatgrid.empty());
    BOOST_CHECK(snapshot.waveform.empty());
    BOOST_CHECK(snapshot.hot_cues.empty());
    BOOST_CHECK(snapshot.loops.empty());
}

BOOST_TEST_DECORATOR(
    *utf::description("key() for each notation rekordbox may write"))
BOOST_AUTO_TEST_CASE(key__each_notation__is_understood_or_not_guessed)
{
    // Arrange
    const onelibrary_device keys{{
        "INSERT INTO \"key\" VALUES (1, 'C'), (2, 'Am'), (3, 'F#m'), "
        "(4, 'Bb'), (5, 'F♯m'), (6, 'B♭'), (7, ''), (8, 'H'), (9, '8A'), "
        "(10, 'Camelot 8A');",
        "INSERT INTO content (content_id, key_id) VALUES (1, 1), (2, 2), "
        "(3, 3), (4, 4), (5, 5), (6, 6), (7, 7), (8, 8), (9, 9), (10, 10);",
    }};
    auto db = keys.library.database();
    const auto key_of = [&](int64_t id) { return db.track_by_id(id)->key(); };

    // Act / Assert
    BOOST_CHECK(key_of(1) == djinterop::musical_key::c_major);
    BOOST_CHECK(key_of(2) == djinterop::musical_key::a_minor);
    BOOST_CHECK(key_of(3) == djinterop::musical_key::f_sharp_minor);
    BOOST_CHECK(key_of(4) == djinterop::musical_key::b_flat_major);

    // The typographic accidentals mean the same as the ASCII ones.
    BOOST_CHECK(key_of(5) == djinterop::musical_key::f_sharp_minor);
    BOOST_CHECK(key_of(6) == djinterop::musical_key::b_flat_major);

    // Anything else, including the Camelot and Open Key wheels, is not read.
    BOOST_CHECK(!key_of(7));
    BOOST_CHECK(!key_of(8));
    BOOST_CHECK(!key_of(9));
    BOOST_CHECK(!key_of(10));
}

BOOST_TEST_DECORATOR(
    *utf::description("library::exists() and library::load() find a device"))
BOOST_AUTO_TEST_CASE(library__exists_and_load__find_a_device)
{
    // Arrange
    const auto& device = device_fixture().root;

    // Act
    const auto lib = olv1::library::load(device, passphrase);

    // Assert
    BOOST_CHECK(olv1::library::exists(device));
    BOOST_CHECK(!olv1::library::exists(device / "nowhere"));
    BOOST_CHECK_EQUAL(lib.directory(), device);
    BOOST_CHECK_EQUAL(lib.content().all_ids().size(), 3u);
}

BOOST_TEST_DECORATOR(*utf::description(
    "create_database_from_scripts() refuses to overwrite a database"))
BOOST_AUTO_TEST_CASE(create_database_from_scripts__an_existing_one__throws)
{
    // Arrange
    const auto& device = device_fixture().root;

    // Act / Assert
    BOOST_CHECK_THROW(
        ol::create_database_from_scripts(
            device, onelibrary_v1_script_directory(), passphrase),
        std::runtime_error);
}
