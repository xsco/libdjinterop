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

#define BOOST_TEST_MODULE engine_v3_album_art_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v3/engine_library.hpp>

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev3 = djinterop::engine::v3;

namespace
{
std::vector<std::byte> make_hash(std::initializer_list<int> bytes)
{
    std::vector<std::byte> result;
    result.reserve(bytes.size());
    for (auto b : bytes)
        result.push_back(static_cast<std::byte>(b));

    return result;
}

/// An example hash, of the 20-byte width that Engine writes.
const std::vector<std::byte> example_hash_bytes =
    make_hash({0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
               0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13});

/// The same, as the column holds it: a binary hash, which is what a
/// library with its art in files beneath `Artwork` writes.
const ev3::album_art_hash example_hash =
    ev3::album_art_binary_hash{example_hash_bytes};

/// A hash as a library that keeps its art in the `albumArt` column writes
/// it: text, in a column declared text.
const ev3::album_art_hash example_text_hash =
    ev3::album_art_text_hash{"0001020304050607080910111213"};

/// What Engine's own rekordbox import writes instead of a hash.
const ev3::album_art_uri example_uri{
    "image://fileart/media/DJ-STICK/PIONEER/Artwork/0123abcd.jpg"};

/// A small image, standing in for one stored in the database.
const std::vector<std::byte> example_image =
    make_hash({0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46});

struct file_name_test_case
{
    std::vector<std::byte> hash;
    std::string expected;

    friend std::ostream& operator<<(
        std::ostream& os, const file_name_test_case& obj)
    {
        os << "file_name_test_case{expected=" << obj.expected << "}";
        return os;
    }
};

const std::vector<file_name_test_case> file_name_test_cases{
    {example_hash_bytes, "AAECAwQFBgcICQoLDA0ODxAREhM.jpg"},

    // A whole group, exercising both characters that base64url spells
    // differently from base64.
    {make_hash({0xff, 0x00, 0x7f}), "_wB_.jpg"},

    // Partial groups, which are not padded.
    {make_hash({0xfb}), "-w.jpg"},
    {make_hash({0xfb, 0xef}), "--8.jpg"},

    {make_hash({}), ".jpg"},
};
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("album_art_file_name() with a hash"))
BOOST_DATA_TEST_CASE(
    album_art_file_name__hash__expected, file_name_test_cases, test_case)
{
    // Arrange/Act
    auto actual = ev3::album_art_file_name(test_case.hash);

    // Assert
    BOOST_CHECK_EQUAL(test_case.expected, actual);
}

BOOST_TEST_DECORATOR(*utf::description("add() with a valid album art row"))
BOOST_DATA_TEST_CASE(add__valid_row__adds, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev3::album_art_row row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt};

    // Act
    auto id = table.add(row);

    // Assert
    BOOST_CHECK_NE(id, ev3::ALBUM_ART_ROW_ID_NONE);
    BOOST_CHECK(table.exists(id));
    row.id = id;
    BOOST_CHECK_EQUAL(row, *table.get(id));
}

BOOST_TEST_DECORATOR(
    *utf::description("add() with a row that already has an id"))
BOOST_DATA_TEST_CASE(add__existing_id__throws, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev3::album_art_row row{123, example_hash, std::nullopt};

    // Act/Assert
    BOOST_CHECK_THROW(table.add(row), ev3::album_art_row_id_error);
}

BOOST_TEST_DECORATOR(*utf::description("find_id() with a known hash"))
BOOST_DATA_TEST_CASE(
    find_id__known_hash__finds, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt});

    // Act
    auto found = table.find_id(example_hash);

    // Assert
    BOOST_REQUIRE(found.has_value());
    BOOST_CHECK_EQUAL(id, *found);
}

BOOST_TEST_DECORATOR(*utf::description("find_id() with an unknown hash"))
BOOST_DATA_TEST_CASE(
    find_id__unknown_hash__none, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto found = table.find_id(example_hash);

    // Assert
    BOOST_CHECK(!found.has_value());
}

BOOST_TEST_DECORATOR(*utf::description("get() with a non-existent id"))
BOOST_DATA_TEST_CASE(get__missing__none, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act/Assert
    BOOST_CHECK(!table.get(123).has_value());
}

BOOST_TEST_DECORATOR(*utf::description("all_ids() on a newly-created library"))
BOOST_DATA_TEST_CASE(
    all_ids__new_library__default_row_only, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto ids = table.all_ids();

    // Assert: the schema creates one row, which is the one that tracks
    // without album art refer to.
    BOOST_REQUIRE_EQUAL(ids.size(), 1);
    BOOST_CHECK_EQUAL(ids[0], ev3::ALBUM_ART_ID_NONE);
}

BOOST_TEST_DECORATOR(*utf::description("remove() with an existing row"))
BOOST_DATA_TEST_CASE(remove__existing__removes, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt});

    // Act
    table.remove(id);

    // Assert
    BOOST_CHECK(!table.exists(id));
}

BOOST_TEST_DECORATOR(*utf::description("update() with an existing row"))
BOOST_DATA_TEST_CASE(update__existing__updates, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt});
    ev3::album_art_row updated{
        id, ev3::album_art_binary_hash{make_hash({0x01, 0x02, 0x03})},
        std::nullopt};

    // Act
    table.update(updated);

    // Assert
    BOOST_CHECK_EQUAL(updated, *table.get(id));
    BOOST_CHECK(!table.find_id(example_hash).has_value());
}

BOOST_TEST_DECORATOR(*utf::description("update() with a row that has no id"))
BOOST_DATA_TEST_CASE(update__no_id__throws, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev3::album_art_row row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt};

    // Act/Assert
    BOOST_CHECK_THROW(table.update(row), ev3::album_art_row_id_error);
}

BOOST_TEST_DECORATOR(
    *utf::description("add() with an image in the `albumArt` column"))
BOOST_DATA_TEST_CASE(
    add__with_image__round_trips, e::supported_v3_schemas, schema)
{
    // Arrange: Engine itself always leaves this column null, but the column
    // exists, and what is written to it must come back.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto image = make_hash({0xff, 0xd8, 0xff, 0xe0, 0x00});
    ev3::album_art_row row{ev3::ALBUM_ART_ROW_ID_NONE, example_hash, image};

    // Act
    auto id = table.add(row);

    // Assert
    auto actual = table.get(id);
    BOOST_REQUIRE(actual.has_value());
    BOOST_REQUIRE(actual->album_art.has_value());
    BOOST_CHECK(*actual->album_art == image);
}

BOOST_TEST_DECORATOR(
    *utf::description("get() on the row created by the schema"))
BOOST_DATA_TEST_CASE(
    get__default_row__no_image, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto row = table.get(ev3::ALBUM_ART_ID_NONE);

    // Assert
    BOOST_REQUIRE(row.has_value());
    // The schema writes this row with an empty text hash, which is a text
    // hash and not a binary one: the row exists to be pointed at, not to
    // name a file.
    BOOST_REQUIRE(std::holds_alternative<ev3::album_art_text_hash>(row->hash));
    BOOST_CHECK(std::get<ev3::album_art_text_hash>(row->hash).value.empty());
    BOOST_CHECK(!row->album_art.has_value());
    // And so there is nowhere to look for an image: a caller that took
    // "text hash" for "the image is in the column" would dereference an
    // empty optional on the one row every artless track points at.
    BOOST_CHECK(ev3::storage_of(*row) == ev3::album_art_storage::none);
}

BOOST_TEST_DECORATOR(*utf::description("a text hash comes back as a text hash"))
BOOST_DATA_TEST_CASE(
    add_get__text_hash__round_trips, e::supported_v3_schemas, schema)
{
    // Arrange
    // What a library written before Engine 4.5.0 holds: the image in the
    // `albumArt` column, and a hash the column's declared type actually
    // fits.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev3::album_art_row row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_text_hash, example_image};

    // Act
    auto id = table.add(row);
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_REQUIRE(
        std::holds_alternative<ev3::album_art_text_hash>(reloaded->hash));
    BOOST_CHECK_EQUAL(
        std::get<ev3::album_art_text_hash>(reloaded->hash).value,
        std::get<ev3::album_art_text_hash>(example_text_hash).value);
    BOOST_CHECK(
        ev3::storage_of(*reloaded) == ev3::album_art_storage::in_database);
    BOOST_REQUIRE(reloaded->album_art.has_value());
    BOOST_CHECK(*reloaded->album_art == example_image);
}

BOOST_TEST_DECORATOR(
    *utf::description("a binary hash does not come back as text"))
BOOST_DATA_TEST_CASE(
    add_get__binary_hash__stays_binary, e::supported_v3_schemas, schema)
{
    // Arrange
    // The two are the same column and SQLite keeps them apart by storage
    // class, which is the whole reason the kinds are separate types.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt});
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_REQUIRE(
        std::holds_alternative<ev3::album_art_binary_hash>(reloaded->hash));
    BOOST_CHECK(
        std::get<ev3::album_art_binary_hash>(reloaded->hash).value ==
        example_hash_bytes);
    BOOST_CHECK(ev3::storage_of(*reloaded) == ev3::album_art_storage::as_file);
}

BOOST_TEST_DECORATOR(
    *utf::description("a URI comes back as a URI, not as a hash"))
BOOST_DATA_TEST_CASE(add_get__uri__round_trips, e::supported_v3_schemas, schema)
{
    // Arrange
    // What Engine's own rekordbox import writes into this column.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_uri, std::nullopt});
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_REQUIRE(std::holds_alternative<ev3::album_art_uri>(reloaded->hash));
    BOOST_CHECK_EQUAL(
        std::get<ev3::album_art_uri>(reloaded->hash).value, example_uri.value);
    BOOST_CHECK(
        ev3::storage_of(*reloaded) == ev3::album_art_storage::elsewhere);
}

BOOST_TEST_DECORATOR(
    *utf::description("find_id() tells a text hash from a binary one"))
BOOST_DATA_TEST_CASE(
    find_id__same_bytes_other_kind__none, e::supported_v3_schemas, schema)
{
    // Arrange
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_text_hash, example_image});

    // Act
    // The same characters, asked for as bytes. SQLite does not equate a
    // TEXT value with a BLOB of the same content, and neither does this.
    const auto& text =
        std::get<ev3::album_art_text_hash>(example_text_hash).value;
    std::vector<std::byte> as_bytes;
    as_bytes.reserve(text.size());
    for (char c : text)
        as_bytes.push_back(static_cast<std::byte>(c));
    auto found_as_bytes = table.find_id(ev3::album_art_binary_hash{as_bytes});

    // The control, without which this case would also pass if find_id()
    // found nothing at all: the row is there, and its own kind finds it.
    auto found_as_text = table.find_id(example_text_hash);

    // Assert
    BOOST_CHECK(!found_as_bytes.has_value());
    BOOST_CHECK(found_as_text.has_value());
}

BOOST_TEST_DECORATOR(
    *utf::description("a null hash column is readable, not an exception"))
BOOST_DATA_TEST_CASE(
    add_get__null_hash__round_trips, e::supported_v3_schemas, schema)
{
    // Arrange
    // The column is nullable in every schema, so a library written by
    // something else may hold one.  Reading it must not throw: `get()` is
    // called in a loop over `all_ids()`, and one unreadable row there takes
    // the whole enumeration with it.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, std::monostate{}, std::nullopt});

    // Act
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_CHECK(std::holds_alternative<std::monostate>(reloaded->hash));
    BOOST_CHECK(ev3::storage_of(*reloaded) == ev3::album_art_storage::none);
    BOOST_CHECK(table.find_id(std::monostate{}) == id);
}

BOOST_TEST_DECORATOR(
    *utf::description("an empty binary hash is refused, not written as null"))
BOOST_DATA_TEST_CASE(
    add__empty_binary_hash__throws, e::supported_v3_schemas, schema)
{
    // Arrange
    // SQLite binds a zero-length blob as a null column, so storing this
    // would hand back a row that does not equal the one written and that
    // find_id() can never match again.
    auto library = ev3::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev3::album_art_row row{
        ev3::ALBUM_ART_ROW_ID_NONE, ev3::album_art_binary_hash{}, std::nullopt};

    // Act/Assert
    BOOST_CHECK_THROW(table.add(row), std::invalid_argument);
    BOOST_CHECK_EQUAL(table.all_ids().size(), 1);

    auto persisted = table.add(ev3::album_art_row{
        ev3::ALBUM_ART_ROW_ID_NONE, example_hash, std::nullopt});
    row.id = persisted;
    BOOST_CHECK_THROW(table.update(row), std::invalid_argument);
    auto unchanged = table.get(persisted);
    BOOST_REQUIRE(unchanged.has_value());
    BOOST_CHECK(unchanged->hash == example_hash);
}
