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

#define BOOST_TEST_MODULE engine_v2_album_art_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <array>
#include <cstddef>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

namespace
{
std::vector<std::byte> make_bytes(std::initializer_list<int> bytes)
{
    std::vector<std::byte> result;
    result.reserve(bytes.size());
    for (auto b : bytes)
        result.push_back(static_cast<std::byte>(b));

    return result;
}

ev2::album_art_binary_hash make_binary_hash(std::initializer_list<int> bytes)
{
    ev2::album_art_binary_hash result{};
    auto it = result.value.begin();
    for (auto b : bytes)
        *it++ = static_cast<std::byte>(b);

    return result;
}

const ev2::album_art_binary_hash example_binary_hash = make_binary_hash(
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13});

const ev2::album_art_hash example_hash = example_binary_hash;

/// A text hash of the form Engine writes, including the case where a leading
/// zero is dropped and the hash is only 39 characters long.
const ev2::album_art_hash example_text_hash =
    ev2::album_art_text_hash{"551c96558e2eb05ea31f3735b129f242b720c15"};

const ev2::album_art_uri example_uri{
    "image://fileart/media/DJ-STICK/PIONEER/Artwork/0123abcd.jpg"};

/// A small image, standing in for one stored in the database.
const std::vector<std::byte> example_image =
    make_bytes({0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46});

struct file_name_test_case
{
    ev2::album_art_binary_hash hash;
    std::string expected;

    friend std::ostream& operator<<(
        std::ostream& os, const file_name_test_case& obj)
    {
        os << "file_name_test_case{expected=" << obj.expected << "}";
        return os;
    }
};

const std::vector<file_name_test_case> file_name_test_cases{
    {example_binary_hash, "AAECAwQFBgcICQoLDA0ODxAREhM.jpg"},

    // Exercises both characters that base64url spells differently from
    // base64, and the unpadded final group.
    {make_binary_hash({0xff, 0x00, 0x7f, 0xff, 0x00, 0x7f, 0xff,
                       0x00, 0x7f, 0xff, 0x00, 0x7f, 0xff, 0x00,
                       0x7f, 0xff, 0x00, 0x7f, 0xfb, 0xef}),
     "_wB__wB__wB__wB__wB__wB_--8.jpg"},
};
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("album_art_file_name() with a hash"))
BOOST_DATA_TEST_CASE(
    album_art_file_name__hash__expected, file_name_test_cases, test_case)
{
    // Arrange/Act
    auto actual = ev2::album_art_file_name(test_case.hash);

    // Assert
    BOOST_CHECK_EQUAL(test_case.expected, actual);
}

BOOST_TEST_DECORATOR(*utf::description("add() with a valid album art row"))
BOOST_DATA_TEST_CASE(add__valid_row__adds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev2::album_art_row row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}};

    // Act
    auto id = table.add(row);

    // Assert
    BOOST_CHECK_NE(id, ev2::ALBUM_ART_ROW_ID_NONE);
    BOOST_CHECK(table.exists(id));
    row.id = id;
    BOOST_CHECK_EQUAL(row, *table.get(id));
}

BOOST_TEST_DECORATOR(
    *utf::description("add() with a row that already has an id"))
BOOST_DATA_TEST_CASE(add__existing_id__throws, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev2::album_art_row row{123, example_hash, {}};

    // Act/Assert
    BOOST_CHECK_THROW(table.add(row), ev2::album_art_row_id_error);
}

BOOST_TEST_DECORATOR(*utf::description("find_id() with a known hash"))
BOOST_DATA_TEST_CASE(
    find_id__known_hash__finds, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}});

    // Act
    auto found = table.find_id(example_hash);

    // Assert
    BOOST_REQUIRE(found.has_value());
    BOOST_CHECK_EQUAL(id, *found);
}

BOOST_TEST_DECORATOR(*utf::description("find_id() with an unknown hash"))
BOOST_DATA_TEST_CASE(
    find_id__unknown_hash__none, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto found = table.find_id(example_hash);

    // Assert
    BOOST_CHECK(!found.has_value());
}

BOOST_TEST_DECORATOR(*utf::description("get() with a non-existent id"))
BOOST_DATA_TEST_CASE(get__missing__none, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act/Assert
    BOOST_CHECK(!table.get(123).has_value());
}

BOOST_TEST_DECORATOR(*utf::description("all_ids() on a newly-created library"))
BOOST_DATA_TEST_CASE(
    all_ids__new_library__default_row_only, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto ids = table.all_ids();

    // Assert
    BOOST_REQUIRE_EQUAL(ids.size(), 1);
    BOOST_CHECK_EQUAL(ids[0], ev2::ALBUM_ART_ID_NONE);
}

BOOST_TEST_DECORATOR(*utf::description("remove() with an existing row"))
BOOST_DATA_TEST_CASE(remove__existing__removes, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}});

    // Act
    table.remove(id);

    // Assert
    BOOST_CHECK(!table.exists(id));
}

BOOST_TEST_DECORATOR(*utf::description("update() with an existing row"))
BOOST_DATA_TEST_CASE(update__existing__updates, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}});
    ev2::album_art_row updated{id, make_binary_hash({0x01, 0x02, 0x03}), {}};

    // Act
    table.update(updated);

    // Assert
    BOOST_CHECK_EQUAL(updated, *table.get(id));
    BOOST_CHECK(!table.find_id(example_hash).has_value());
}

BOOST_TEST_DECORATOR(*utf::description("update() with a row that has no id"))
BOOST_DATA_TEST_CASE(update__no_id__throws, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev2::album_art_row row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}};

    // Act/Assert
    BOOST_CHECK_THROW(table.update(row), ev2::album_art_row_id_error);
}

BOOST_TEST_DECORATOR(
    *utf::description("get() on the row created by the schema"))
BOOST_DATA_TEST_CASE(
    get__default_row__no_image, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto row = table.get(ev2::ALBUM_ART_ID_NONE);

    // Assert
    BOOST_REQUIRE(row.has_value());
    BOOST_REQUIRE(std::holds_alternative<ev2::album_art_text_hash>(row->hash));
    BOOST_CHECK(std::get<ev2::album_art_text_hash>(row->hash).value.empty());
    BOOST_CHECK(row->album_art.empty());
    BOOST_CHECK(ev2::storage_of(*row) == ev2::album_art_storage::none);
}

BOOST_TEST_DECORATOR(
    *utf::description("a text hash and image in the database round-trip"))
BOOST_DATA_TEST_CASE(
    add_get__text_hash__round_trips, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    ev2::album_art_row row{
        ev2::ALBUM_ART_ROW_ID_NONE, example_text_hash, example_image};

    // Act
    row.id = table.add(row);
    auto reloaded = table.get(row.id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_CHECK_EQUAL(row, *reloaded);
    BOOST_CHECK(
        ev2::storage_of(*reloaded) == ev2::album_art_storage::blob_in_database);
}

BOOST_TEST_DECORATOR(
    *utf::description("a binary hash does not come back as text"))
BOOST_DATA_TEST_CASE(
    add_get__binary_hash__stays_binary, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, example_hash, {}});
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_REQUIRE(
        std::holds_alternative<ev2::album_art_binary_hash>(reloaded->hash));
    BOOST_CHECK(
        std::get<ev2::album_art_binary_hash>(reloaded->hash) ==
        example_binary_hash);
    BOOST_CHECK(
        ev2::storage_of(*reloaded) ==
        ev2::album_art_storage::file_in_artwork_dir);
}

BOOST_TEST_DECORATOR(
    *utf::description("a URI comes back as a URI, not as a hash"))
BOOST_DATA_TEST_CASE(add_get__uri__round_trips, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();

    // Act
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, example_uri, {}});
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_REQUIRE(std::holds_alternative<ev2::album_art_uri>(reloaded->hash));
    BOOST_CHECK_EQUAL(
        std::get<ev2::album_art_uri>(reloaded->hash).value, example_uri.value);
    BOOST_CHECK(ev2::storage_of(*reloaded) == ev2::album_art_storage::uri);
}

BOOST_TEST_DECORATOR(
    *utf::description("find_id() tells a text hash from a binary one"))
BOOST_DATA_TEST_CASE(
    find_id__same_bytes_other_kind__none, e::supported_v2_schemas, schema)
{
    // Arrange: a text hash of 20 characters, and a binary hash of the same
    // 20 bytes.
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    const std::string text = "0123456789abcdef0123";
    const ev2::album_art_hash text_hash = ev2::album_art_text_hash{text};
    table.add(ev2::album_art_row{
        ev2::ALBUM_ART_ROW_ID_NONE, text_hash, example_image});

    ev2::album_art_binary_hash binary_hash{};
    for (std::size_t i = 0; i < binary_hash.value.size(); ++i)
        binary_hash.value[i] = static_cast<std::byte>(text[i]);

    // Act
    auto found_as_binary = table.find_id(binary_hash);
    auto found_as_text = table.find_id(text_hash);

    // Assert
    BOOST_CHECK(!found_as_binary.has_value());
    BOOST_CHECK(found_as_text.has_value());
}

BOOST_TEST_DECORATOR(
    *utf::description("a null hash column is readable, not an exception"))
BOOST_DATA_TEST_CASE(
    add_get__null_hash__round_trips, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto table = library.album_art();
    auto id = table.add(
        ev2::album_art_row{ev2::ALBUM_ART_ROW_ID_NONE, std::monostate{}, {}});

    // Act
    auto reloaded = table.get(id);

    // Assert
    BOOST_REQUIRE(reloaded.has_value());
    BOOST_CHECK(std::holds_alternative<std::monostate>(reloaded->hash));
    BOOST_CHECK(ev2::storage_of(*reloaded) == ev2::album_art_storage::none);
    BOOST_CHECK(table.find_id(std::monostate{}) == id);
}
