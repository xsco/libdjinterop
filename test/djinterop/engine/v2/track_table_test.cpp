/*
    This file is part of libdjinterop.

    libdjinterop is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either schema 3 of the License, or
    (at your option) any later version.

    libdjinterop is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libdjinterop.  If not, see <http://www.gnu.org/licenses/>.
 */

#define BOOST_TEST_MODULE engine_v2_track_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <sstream>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>
#include <djinterop/exceptions.hpp>

#include "../../boost_test_printable.hpp"
#include "../../boost_test_utils.hpp"
#include "../../temporary_directory.hpp"
#include "example_track_row_data.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;
using es = djinterop::engine::engine_schema;

namespace
{
const std::vector<example_track_row_type> all_example_track_row_types{
    example_track_row_type::minimal_1,
    example_track_row_type::basic_metadata_only_1,
    example_track_row_type::fully_analysed_1,
};
}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("add() with valid track row"))
BOOST_DATA_TEST_CASE(
    add__valid__adds, e::supported_v2_schemas* all_example_track_row_types,
    schema, row_type)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto track_tbl = library.track();
    ev2::track_row row{0};
    populate_track_row(row_type, row, schema);

    // Act
    auto id = track_tbl.add(row);

    // Assert
    BOOST_CHECK_NE(id, 0);
}

BOOST_TEST_DECORATOR(*utf::description("add() with an existing id"))
BOOST_DATA_TEST_CASE(add__existing_id__throws, e::supported_v2_schemas, schema)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(schema);
    auto track_tbl = library.track();
    ev2::track_row row{123};

    // Act/Assert
    BOOST_CHECK_THROW(track_tbl.add(row), ev2::track_row_id_error);
}

BOOST_TEST_DECORATOR(*utf::description("get() with a valid id"))
BOOST_DATA_TEST_CASE(
    get__valid__gets, e::supported_v2_schemas* all_example_track_row_types,
    schema, row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type
            << ") Creating temporary database...");
    auto library = ev2::engine_library::create_temporary(schema);

    auto track_tbl = library.track();
    ev2::track_row expected{0};
    populate_track_row(row_type, expected, schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type << ") Creating track...");
    auto id = track_tbl.add(expected);
    expected.id = id;

    if (expected.origin_database_uuid == "" || expected.origin_track_id == 0)
    {
        // A DB trigger will update the origin UUID and id if not set.
        expected.origin_database_uuid = library.information().get().uuid;
        expected.origin_track_id = id;
    }

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type << ") Fetching track...");
    auto actual = track_tbl.get(id);

    // Assert
    BOOST_REQUIRE(actual != std::nullopt);
    actual->last_edit_time = expected.last_edit_time;
    BOOST_CHECK_EQUAL(expected, *actual);
}

BOOST_TEST_DECORATOR(*utf::description("update() with valid data"))
BOOST_DATA_TEST_CASE(
    update__valid__updates,
    e::supported_v2_schemas* all_example_track_row_types*
        all_example_track_row_types,
    schema, initial_row_type, update_row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << initial_row_type << ", " << update_row_type
            << ") Creating temporary database...");
    auto library = ev2::engine_library::create_temporary(schema);

    auto track_tbl = library.track();
    ev2::track_row initial{0};
    populate_track_row(initial_row_type, initial, schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << initial_row_type << ", " << update_row_type
            << ") Creating initial track...");
    auto id = track_tbl.add(initial);

    ev2::track_row expected{id};
    populate_track_row(update_row_type, expected, schema);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << initial_row_type << ", " << update_row_type
            << ") Updating track...");
    track_tbl.update(expected);

    // Assert
    if (expected.origin_database_uuid == "" || expected.origin_track_id == 0)
    {
        // A DB trigger will update the origin UUID and id if not set.
        expected.origin_database_uuid = library.information().get().uuid;
        expected.origin_track_id = id;
    }

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << initial_row_type << ", " << update_row_type
            << ") Fetching track...");
    auto actual = track_tbl.get(id);
    BOOST_REQUIRE(actual != std::nullopt);
    actual->last_edit_time = expected.last_edit_time;
    BOOST_CHECK_EQUAL(expected, *actual);
}

// The act of defining very similar test cases for all the getters and setters
// on the track table is highly tedious.  As such, some macros to generate these
// more efficiently make for a more succinct way to define tests.
#define DEFINE_GETTER_VALID_TEST_CASE(engine_column, min_schema)          \
    BOOST_TEST_DECORATOR(                                                 \
        *utf::description("get_" #engine_column "() with valid track"))   \
    BOOST_DATA_TEST_CASE(                                                 \
        get_##engine_column##__expected, e::supported_v2_schemas, schema) \
    {                                                                     \
        auto library = ev2::engine_library::create_temporary(schema);     \
        auto track_tbl = library.track();                                 \
        ev2::track_row row{0};                                            \
        populate_track_row(                                               \
            example_track_row_type::fully_analysed_1, row, schema);       \
        auto id = track_tbl.add(row);                                     \
        auto expected = row.engine_column;                                \
                                                                          \
        if (schema >= min_schema)                                         \
        {                                                                 \
            auto actual = track_tbl.get_##engine_column(id);              \
            BOOST_CHECK_EQUAL(                                            \
                make_printable(expected), make_printable(actual));        \
        }                                                                 \
        else                                                              \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                track_tbl.get_##engine_column(id),                        \
                djinterop::unsupported_operation);                        \
        }                                                                 \
    }

#define DEFINE_SETTER_VALID_TEST_CASE(engine_column, min_schema)            \
    BOOST_TEST_DECORATOR(                                                   \
        *utf::description("set_" #engine_column "() with valid track"))     \
    BOOST_DATA_TEST_CASE(                                                   \
        set_##engine_column##__valid__expected, e::supported_v2_schemas,    \
        schema)                                                             \
    {                                                                       \
        auto library = ev2::engine_library::create_temporary(schema);       \
        auto track_tbl = library.track();                                   \
        ev2::track_row row{0};                                              \
        populate_track_row(example_track_row_type::minimal_1, row, schema); \
        auto id = track_tbl.add(row);                                       \
                                                                            \
        populate_track_row(                                                 \
            example_track_row_type::fully_analysed_1, row, schema);         \
        auto expected = row.engine_column;                                  \
                                                                            \
        if (schema >= min_schema)                                           \
        {                                                                   \
            track_tbl.set_##engine_column(id, expected);                    \
                                                                            \
            auto actual = track_tbl.get_##engine_column(id);                \
            BOOST_CHECK_EQUAL(                                              \
                make_printable(expected), make_printable(actual));          \
        }                                                                   \
        else                                                                \
        {                                                                   \
            BOOST_CHECK_THROW(                                              \
                track_tbl.set_##engine_column(id, expected),                \
                djinterop::unsupported_operation);                          \
        }                                                                   \
    }

#define DEFINE_GETTER_INVALID_TEST_CASE(engine_column, min_schema)        \
    BOOST_TEST_DECORATOR(                                                 \
        *utf::description("get_" #engine_column "() with invalid track")) \
    BOOST_DATA_TEST_CASE(                                                 \
        get_##engine_column##__invalid__throws, e::supported_v2_schemas,  \
        schema)                                                           \
    {                                                                     \
        auto library = ev2::engine_library::create_temporary(schema);     \
        auto track_tbl = library.track();                                 \
                                                                          \
        if (schema >= min_schema)                                         \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                track_tbl.get_##engine_column(12345),                     \
                ev2::track_row_id_error);                                 \
        }                                                                 \
        else                                                              \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                track_tbl.get_##engine_column(12345),                     \
                djinterop::unsupported_operation);                        \
        }                                                                 \
    }

#define DEFINE_SETTER_INVALID_TEST_CASE(engine_column, min_schema)        \
    BOOST_TEST_DECORATOR(                                                 \
        *utf::description("set_" #engine_column "() with invalid track")) \
    BOOST_DATA_TEST_CASE(                                                 \
        set_##engine_column##__invalid__throws, e::supported_v2_schemas,  \
        schema)                                                           \
    {                                                                     \
        auto library = ev2::engine_library::create_temporary(schema);     \
        auto track_tbl = library.track();                                 \
        ev2::track_row row{0};                                            \
        populate_track_row(                                               \
            example_track_row_type::fully_analysed_1, row, schema);       \
                                                                          \
        if (schema >= min_schema)                                         \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                track_tbl.set_##engine_column(12345, row.engine_column),  \
                ev2::track_row_id_error);                                 \
        }                                                                 \
        else                                                              \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                track_tbl.set_##engine_column(12345, row.engine_column),  \
                djinterop::unsupported_operation);                        \
        }                                                                 \
    }

#define DEFINE_GETTER_SETTER_TEST_CASES(engine_column, min_schema) \
    DEFINE_GETTER_VALID_TEST_CASE(engine_column, min_schema)       \
    DEFINE_GETTER_INVALID_TEST_CASE(engine_column, min_schema)     \
    DEFINE_SETTER_VALID_TEST_CASE(engine_column, min_schema)       \
    DEFINE_SETTER_INVALID_TEST_CASE(engine_column, min_schema)

DEFINE_GETTER_SETTER_TEST_CASES(play_order, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(length, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(bpm, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(year, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(path, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(filename, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(bitrate, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(bpm_analyzed, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(album_art_id, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(file_bytes, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(title, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(artist, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(album, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(genre, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(comment, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(label, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(composer, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(remixer, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(key, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(rating, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(album_art, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(time_last_played, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(is_played, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(file_type, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(is_analyzed, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(date_created, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(date_added, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(is_available, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(
    is_metadata_of_packed_track_changed, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(
    is_performance_data_of_packed_track_changed, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(played_indicator, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(is_metadata_imported, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(pdb_import_key, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(streaming_source, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(uri, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(is_beat_grid_locked, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(origin_database_uuid, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(origin_track_id, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(track_data, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(overview_waveform_data, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(beat_data, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(quick_cues, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(loops, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(third_party_source_id, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(streaming_flags, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(explicit_lyrics, es::schema_2_18_0)
DEFINE_GETTER_SETTER_TEST_CASES(active_on_load_loops, es::schema_2_20_1)
DEFINE_GETTER_SETTER_TEST_CASES(last_edit_time, es::schema_2_20_3)

#undef DEFINE_GETTER_SETTER_TEST_CASES
#undef DEFINE_SETTER_INVALID_TEST_CASE
#undef DEFINE_SETTER_VALID_TEST_CASE
#undef DEFINE_GETTER_INVALID_TEST_CASE
#undef DEFINE_GETTER_VALID_TEST_CASE

BOOST_TEST_DECORATOR(*utf::description("operator<<() with valid track row"))
BOOST_DATA_TEST_CASE(
    operator_stream_output__valid__outputs,
    e::supported_v2_schemas* all_example_track_row_types, schema, row_type)
{
    // Arrange
    ev2::track_row row{0};
    populate_track_row(row_type, row, schema);
    std::stringstream ss;

    // Act
    ss << row;

    // Assert
    BOOST_CHECK_NE(ss.str(), "");
}
