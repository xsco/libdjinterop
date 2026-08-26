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

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include <djinterop/djinterop.hpp>
#include <djinterop/onelibrary/onelibrary.hpp>

#include "../../../src/djinterop/util/filesystem.hpp"
#include "../boost_test_printable.hpp"
#include "../sqlcipher_encryptor.hpp"
#include "../temporary_directory.hpp"
#include "onelibrary_schema.hpp"

namespace utf = boost::unit_test;
namespace ol = djinterop::onelibrary;
namespace crypto = djinterop::util::crypto;

namespace
{
const std::string passphrase = "a passphrase for the fixture";

/// The encryptor that every fixture is written with.
///
/// Key derivation is deliberately expensive -- the format stretches the
/// passphrase 256,000 times -- so one salt, and hence one derived key, is
/// shared by every fixture rather than made afresh for each.  A database
/// carries its own salt, so nothing about reading one depends on this.
const sqlcipher_encryptor& fixture_encryptor()
{
    static const sqlcipher_encryptor encryptor{passphrase};
    return encryptor;
}

/// Encrypt a plain SQLite file in place, as SQLCipher would have written it.
///
/// The fixture is built as an ordinary database and encrypted afterwards,
/// which is the only direction the library offers: it decrypts a device to
/// read it, and never writes one.
void encrypt_in_place(const std::string& path)
{
    std::vector<uint8_t> plain;
    {
        std::ifstream file{path, std::ios::binary};
        plain.assign(
            std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{});
    }

    const crypto::sqlcipher_parameters params;
    BOOST_REQUIRE(!plain.empty());
    BOOST_REQUIRE_EQUAL(plain.size() % params.page_size, 0u);

    const auto& encryptor = fixture_encryptor();

    std::vector<uint8_t> encrypted(plain.size());
    for (size_t index = 0; index < plain.size() / params.page_size; ++index)
    {
        encryptor.encrypt_page(
            static_cast<uint32_t>(index + 1),
            plain.data() + (index * params.page_size),
            encrypted.data() + (index * params.page_size));
    }

    std::ofstream{path, std::ios::binary | std::ios::trunc}.write(
        reinterpret_cast<const char*>(encrypted.data()),
        static_cast<std::streamsize>(encrypted.size()));
}

/// Prepare a plain database to hold pages of the shape SQLCipher expects.
void prepare_plain_database(sqlite3* db)
{
    const crypto::sqlcipher_parameters params;
    char* error = nullptr;
    const auto sql = "PRAGMA page_size = " + std::to_string(params.page_size);
    BOOST_REQUIRE_EQUAL(
        sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error), SQLITE_OK);
    sqlite3_free(error);

    auto reserve = static_cast<int>(params.reserve);
    BOOST_REQUIRE_EQUAL(
        sqlite3_file_control(db, "main", SQLITE_FCNTL_RESERVE_BYTES, &reserve),
        SQLITE_OK);
}

void execute(sqlite3* db, const std::string& sql)
{
    char* error = nullptr;
    const auto rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error);
    const std::string message = error != nullptr ? error : "";
    sqlite3_free(error);
    BOOST_REQUIRE_MESSAGE(
        rc == SQLITE_OK, "failed to run \"" << sql << "\": " << message);
}

void create_onelibrary_schema(sqlite3* db)
{
    for (const auto& statement : onelibrary_schema_statements())
        execute(db, statement);
}

/// Build a device holding a small OneLibrary export.
///
/// The schema is the one a real export carries, abbreviated to the tables this
/// library reads.  The data covers the awkward cases: metadata that is absent,
/// present but empty, and non-ASCII; a key notation that is not understood;
/// and a nested playlist.
std::string make_device(const temporary_directory& temp_dir)
{
    const auto root = temp_dir.temp_dir;
    djinterop::util::create_dir(root + "/PIONEER");
    djinterop::util::create_dir(root + "/PIONEER/rekordbox");

    const auto path = root + "/" + ol::database_relative_path;

    sqlite3* db = nullptr;
    BOOST_REQUIRE_EQUAL(
        sqlite3_open_v2(
            path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr),
        SQLITE_OK);

    prepare_plain_database(db);

    create_onelibrary_schema(db);

    execute(
        db,
        "INSERT INTO artist VALUES (1, 'Aphex Twin', 'APHEX TWIN'), "
        "(2, 'Кто-то', ''), (3, 'A Composer', '')");
    execute(db, "INSERT INTO album VALUES (1, 'Album One', 1, 0, 0, '')");
    execute(db, "INSERT INTO genre VALUES (1, 'Electro')");
    execute(db, "INSERT INTO label VALUES (1, 'Warp')");
    execute(
        db,
        "INSERT INTO \"key\" VALUES (1, 'F#m'), (2, 'Bb'), "
        "(3, 'Camelot 8A')");

    // A fully populated track.
    execute(
        db,
        "INSERT INTO content (content_id, title, bpmx100, length, trackNo, "
        "artist_id_artist, artist_id_composer, album_id, genre_id, label_id, "
        "key_id, djComment, rating, releaseYear, path, fileName, fileSize, "
        "bitrate, samplingRate, masterDbId, analysisDataFilePath, color_id) "
        "VALUES (1, 'Alpha Track', 12400, "
        "391, 7, 1, 3, 1, 1, 1, 1, 'feelin good', 4, 2025, "
        "'/Contents/Aphex/alpha.mp3', 'alpha.mp3', 6580703, 320, 44100, "
        "4056018032, '/PIONEER/USBANLZ/P016/0000875e/ANLZ0000.DAT', 6)");

    // Non-ASCII metadata, an empty comment, and no rating.
    execute(
        db,
        "INSERT INTO content (content_id, title, bpmx100, length, "
        "artist_id_artist, key_id, djComment, rating, path, samplingRate, "
        "masterDbId, color_id) VALUES (2, 'Бета Трек', 12800, 245, 2, 2, '', "
        "0, '/Contents/Various/beta.flac', 48000, 4056018032, 0)");

    // Almost nothing set, and a key notation this library does not know.
    execute(
        db,
        "INSERT INTO content (content_id, title, key_id, rating, path) "
        "VALUES (3, 'Gamma', 3, 5, '/Contents/Third/gamma.wav')");

    execute(
        db,
        "INSERT INTO playlist VALUES (1, 1, 'Sets', 0, 0, 0), "
        "(2, 1, 'Warm Up', 0, 0, 1), (3, 2, 'Peak Time', 0, 0, 0)");
    execute(
        db,
        "INSERT INTO playlist_content VALUES (2, 1, 1), (2, 3, 2), "
        "(3, 2, 1)");
    execute(
        db,
        "INSERT INTO property VALUES ('FIXTURE', '1000', 3, "
        "'2026-01-01', 0, 0)");

    sqlite3_close(db);
    encrypt_in_place(path);
    return root;
}

/// The device that the tests read, built once.
///
/// Key derivation is deliberately expensive -- the format stretches the
/// passphrase 256,000 times -- so a test that built its own device and opened
/// it would pay for that twice over, every time.
struct shared_device
{
    temporary_directory temp_dir;
    std::string path;

    shared_device() : path{make_device(temp_dir)} {}
};

const shared_device& device_fixture()
{
    static const shared_device instance;
    return instance;
}

/// The shared device, opened once.
djinterop::database loaded_database()
{
    static const djinterop::database db =
        ol::load_database(device_fixture().path, passphrase);
    return db;
}

/// The shared device, opened once as a library.
const ol::library& loaded_library()
{
    static const ol::library lib{device_fixture().path, passphrase};
    return lib;
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description(
    "database_exists() finds a device by its root or by its file"))
BOOST_AUTO_TEST_CASE(database_exists__a_device__is_found)
{
    const auto& device = device_fixture().path;

    BOOST_CHECK(ol::database_exists(device));
    BOOST_CHECK(ol::database_exists(device + "/" + ol::database_relative_path));
    BOOST_CHECK(!ol::database_exists(device + "/nowhere"));
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() reports the identity of a device"))
BOOST_AUTO_TEST_CASE(load_database__a_device__reports_its_identity)
{
    const auto& device = device_fixture().path;
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
    const auto& device = device_fixture().path;
    auto db = ol::load_database(
        device + "/" + ol::database_relative_path, passphrase);

    BOOST_CHECK_EQUAL(db.tracks().size(), 3u);

    // Naming the file still identifies the device it belongs to, because
    // track paths are relative to that and not to the file.
    BOOST_CHECK_EQUAL(db.directory(), device);
}

BOOST_TEST_DECORATOR(
    *utf::description("load_database() refuses a wrong passphrase"))
BOOST_AUTO_TEST_CASE(load_database__a_wrong_passphrase__is_refused)
{
    const auto& device = device_fixture().path;

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
    temporary_directory temp_dir;
    const auto path = temp_dir.temp_dir + "/deep.db";

    sqlite3* db = nullptr;
    BOOST_REQUIRE_EQUAL(
        sqlite3_open_v2(
            path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr),
        SQLITE_OK);
    prepare_plain_database(db);
    create_onelibrary_schema(db);

    // Root
    //  +- Middle A        (sequence 1)
    //  |   +- Leaf A      (sequence 1)
    //  +- Middle B        (sequence 2)
    execute(
        db,
        "INSERT INTO playlist VALUES (1, 1, 'Root', 0, 0, 0), "
        "(2, 1, 'Middle A', 0, 0, 1), (3, 2, 'Middle B', 0, 0, 1), "
        "(4, 1, 'Leaf A', 0, 0, 2)");
    execute(
        db,
        "INSERT INTO property VALUES ('FIXTURE', '1000', 0, "
        "'2026-01-01', 0, 0)");
    sqlite3_close(db);
    encrypt_in_place(path);

    auto loaded = ol::load_database(path, passphrase);
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
    // write-ahead-logged.  SQLite refuses to open one of those read-only
    // without the log, so the header has to be rewritten as it is decrypted.
    temporary_directory temp_dir;
    const auto path = temp_dir.temp_dir + "/checkpointed.db";

    sqlite3* db = nullptr;
    BOOST_REQUIRE_EQUAL(
        sqlite3_open_v2(
            path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr),
        SQLITE_OK);
    prepare_plain_database(db);
    create_onelibrary_schema(db);
    execute(db, "PRAGMA journal_mode = WAL");
    execute(
        db,
        "INSERT INTO content (content_id, title, path) "
        "VALUES (1, 'Checkpointed', '/a.mp3')");
    execute(
        db,
        "INSERT INTO property VALUES ('FIXTURE', '1000', 1, "
        "'2026-01-01', 0, 0)");

    // Closing folds the log back in, exactly as ejecting a device does,
    // leaving nothing beside the database but its header still saying WAL.
    sqlite3_close(db);
    std::remove((path + "-wal").c_str());

    encrypt_in_place(path);

    auto loaded = ol::load_database(path, passphrase);
    const auto tracks = loaded.tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 1u);
    BOOST_CHECK_EQUAL(tracks[0].title().value(), "Checkpointed");
}

BOOST_TEST_DECORATOR(
    *utf::description("verify() rejects a database missing its tables"))
BOOST_AUTO_TEST_CASE(verify__a_database_missing_its_tables__is_rejected)
{
    temporary_directory temp_dir;
    const auto path = temp_dir.temp_dir + "/not-onelibrary.db";

    sqlite3* db = nullptr;
    BOOST_REQUIRE_EQUAL(
        sqlite3_open_v2(
            path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr),
        SQLITE_OK);
    prepare_plain_database(db);
    execute(db, "CREATE TABLE something_else(id INTEGER PRIMARY KEY)");
    sqlite3_close(db);
    encrypt_in_place(path);

    BOOST_CHECK_THROW(
        ol::load_database(path, passphrase), djinterop::database_inconsistency);
}

namespace
{
/// Read a whole file.
std::vector<uint8_t> read_file(const std::string& path)
{
    std::ifstream file{path, std::ios::binary};
    return std::vector<uint8_t>{
        std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

void write_file(const std::string& path, const std::vector<uint8_t>& data)
{
    std::ofstream{path, std::ios::binary | std::ios::trunc}.write(
        reinterpret_cast<const char*>(data.data()),
        static_cast<std::streamsize>(data.size()));
}

uint32_t load_be32(const uint8_t* p)
{
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

void store_be32(uint8_t* p, uint32_t value)
{
    p[0] = static_cast<uint8_t>(value >> 24);
    p[1] = static_cast<uint8_t>(value >> 16);
    p[2] = static_cast<uint8_t>(value >> 8);
    p[3] = static_cast<uint8_t>(value);
}

/// Encrypt a plain database and its write-ahead log the way SQLCipher does.
///
/// SQLCipher encrypts the page inside each log frame, and SQLite checksums the
/// frame over the bytes as they end up in the file -- so over the ciphertext.
/// Reproducing that here is what makes this a test of reading a real log
/// rather than of reading one this library made up.
void encrypt_database_and_log(
    const std::string& path, const sqlcipher_encryptor& encryptor)
{
    const auto& params = encryptor.params();

    auto plain = read_file(path);
    BOOST_REQUIRE_EQUAL(plain.size() % params.page_size, 0u);
    std::vector<uint8_t> encrypted(plain.size());
    for (size_t index = 0; index < plain.size() / params.page_size; ++index)
        encryptor.encrypt_page(
            static_cast<uint32_t>(index + 1),
            plain.data() + (index * params.page_size),
            encrypted.data() + (index * params.page_size));
    write_file(path, encrypted);

    auto log = read_file(path + "-wal");
    BOOST_REQUIRE_GE(log.size(), 32u);

    // The checksums of the log are computed over its contents in the byte
    // order its magic number selects, and stored big-endian.
    const auto big_endian = (load_be32(log.data()) & 1) != 0;
    const auto read_word = [&](const uint8_t* p)
    {
        if (big_endian)
            return load_be32(p);

        return static_cast<uint32_t>(
            (static_cast<uint32_t>(p[3]) << 24) |
            (static_cast<uint32_t>(p[2]) << 16) |
            (static_cast<uint32_t>(p[1]) << 8) | static_cast<uint32_t>(p[0]));
    };

    uint32_t s0 = 0;
    uint32_t s1 = 0;
    const auto accumulate = [&](const uint8_t* data, size_t length)
    {
        for (size_t offset = 0; offset + 8 <= length; offset += 8)
        {
            s0 += read_word(data + offset) + s1;
            s1 += read_word(data + offset + 4) + s0;
        }
    };

    accumulate(log.data(), 24);
    BOOST_REQUIRE_EQUAL(s0, load_be32(log.data() + 24));
    BOOST_REQUIRE_EQUAL(s1, load_be32(log.data() + 28));

    const auto frame_length = 24 + params.page_size;
    size_t frames = 0;
    for (size_t offset = 32; offset + frame_length <= log.size();
         offset += frame_length)
    {
        auto* frame = log.data() + offset;
        const auto page_number = load_be32(frame);

        std::vector<uint8_t> page(params.page_size);
        encryptor.encrypt_page(page_number, frame + 24, page.data());
        std::memcpy(frame + 24, page.data(), page.size());

        accumulate(frame, 8);
        accumulate(frame + 24, params.page_size);
        store_be32(frame + 16, s0);
        store_be32(frame + 20, s1);
        ++frames;
    }

    BOOST_REQUIRE_GT(frames, 0u);
    write_file(path + "-wal", log);
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(
    *utf::description("load_database() reads data left in the write-ahead log"))
BOOST_AUTO_TEST_CASE(load_database__data_left_in_the_log__is_read)
{
    // rekordbox leaves most of a fresh export in the log rather than in the
    // database file, so a reader that ignores it reports a nearly empty
    // library, with no error at all.
    temporary_directory temp_dir;
    const auto device = temp_dir.temp_dir + "/device";
    djinterop::util::create_dir(device);
    djinterop::util::create_dir(device + "/PIONEER");
    djinterop::util::create_dir(device + "/PIONEER/rekordbox");

    const auto path = device + "/" + std::string{ol::database_relative_path};

    sqlite3* db = nullptr;
    BOOST_REQUIRE_EQUAL(
        sqlite3_open_v2(
            path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr),
        SQLITE_OK);
    prepare_plain_database(db);
    create_onelibrary_schema(db);
    execute(
        db,
        "INSERT INTO content (content_id, title, path) "
        "VALUES (1, 'In The Database', '/a.mp3')");
    execute(
        db,
        "INSERT INTO property VALUES ('FIXTURE', '1000', 2, "
        "'2026-01-01', 0, 0)");

    // From here on, everything lands in the log and stays there.
    execute(db, "PRAGMA journal_mode = WAL");
    execute(db, "PRAGMA wal_autocheckpoint = 0");
    execute(
        db,
        "INSERT INTO content (content_id, title, path) "
        "VALUES (2, 'In The Log', '/b.mp3')");

    // Copy the pair aside while the connection is open, because closing it
    // would fold the log back in.
    const auto db_copy = temp_dir.temp_dir + "/copy.db";
    write_file(db_copy, read_file(path));
    write_file(db_copy + "-wal", read_file(path + "-wal"));
    sqlite3_close(db);

    write_file(path, read_file(db_copy));
    write_file(path + "-wal", read_file(db_copy + "-wal"));

    encrypt_database_and_log(path, fixture_encryptor());

    auto loaded = ol::load_database(device, passphrase);
    const auto tracks = loaded.tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 2u);
    BOOST_CHECK_EQUAL(tracks[0].title().value(), "In The Database");
    BOOST_CHECK_EQUAL(tracks[1].title().value(), "In The Log");
}

BOOST_TEST_DECORATOR(
    *utf::description("library reaches the device through both interfaces"))
BOOST_AUTO_TEST_CASE(library__a_device__is_read_through_its_database)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act
    auto db = lib.db();

    // Assert
    BOOST_CHECK_EQUAL(lib.directory(), device_fixture().path);
    BOOST_CHECK_EQUAL(db.directory(), device_fixture().path);
    BOOST_CHECK_EQUAL(db.tracks().size(), 3u);
}

BOOST_TEST_DECORATOR(*utf::description(
    "library::analysis_path() gives a path relative to the device"))
BOOST_AUTO_TEST_CASE(analysis_path__a_track_with_analysis_data__is_relative)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act
    const auto path = lib.analysis_path(1);

    // Assert
    BOOST_REQUIRE(path);
    BOOST_CHECK_EQUAL(*path, "PIONEER/USBANLZ/P016/0000875e/ANLZ0000.DAT");
}

BOOST_TEST_DECORATOR(*utf::description(
    "library::analysis_path() for a track that carries none, and for one that "
    "is not there"))
BOOST_AUTO_TEST_CASE(analysis_path__no_analysis_data__is_absent)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK(!lib.analysis_path(2));
    BOOST_CHECK(!lib.analysis_path(1234));
}

BOOST_TEST_DECORATOR(*utf::description(
    "library::key_name() gives back the notation the device holds"))
BOOST_AUTO_TEST_CASE(key_name__any_track__is_the_notation_on_the_device)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK_EQUAL(lib.key_name(1).value(), "F#m");
    BOOST_CHECK_EQUAL(lib.key_name(2).value(), "Bb");

    // A notation the library does not parse is still given back whole.
    BOOST_CHECK_EQUAL(lib.key_name(3).value(), "Camelot 8A");

    BOOST_CHECK(!lib.key_name(1234));
}

BOOST_TEST_DECORATOR(
    *utf::description("library::color_id() reads the colour of a track"))
BOOST_AUTO_TEST_CASE(color_id__marked_and_unmarked_tracks__reports_each)
{
    // Arrange
    const auto& lib = loaded_library();

    // Act, Assert
    BOOST_CHECK_EQUAL(lib.color_id(1).value(), 6);

    // A colour of zero is no colour, as is a column that is not set at all.
    BOOST_CHECK(!lib.color_id(2));
    BOOST_CHECK(!lib.color_id(3));
    BOOST_CHECK(!lib.color_id(1234));
}
