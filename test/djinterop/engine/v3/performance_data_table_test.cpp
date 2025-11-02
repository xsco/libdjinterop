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

#define BOOST_TEST_MODULE engine_v3_performance_data_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <sstream>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v3/engine_library.hpp>
#include <djinterop/exceptions.hpp>

#include "../../boost_test_printable.hpp"
#include "../../boost_test_utils.hpp"
#include "example_performance_data_row_data.hpp"
#include "example_track_row_data.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev3 = djinterop::engine::v3;
using es = djinterop::engine::engine_schema;

namespace
{
const std::vector<example_performance_data_row_type>
    all_example_performance_data_row_types{
        example_performance_data_row_type::minimal_1,
        example_performance_data_row_type::fully_analysed_1,
    };
}  // anonymous namespace

BOOST_TEST_DECORATOR(
    *utf::description("Adding a track adds a performance data row"))
BOOST_DATA_TEST_CASE(
    add_track_adds_performance_data, e::supported_v3_schemas, schema)
{
    // Arrange
    BOOST_TEST_CHECKPOINT("(" << schema << ") Creating temporary database...");
    auto library = ev3::engine_library::create_temporary(schema);

    // Act
    BOOST_TEST_CHECKPOINT("(" << schema << ") Adding track...");
    ev3::track_row track{ev3::TRACK_ROW_ID_NONE};
    populate_track_row(example_track_row_type::minimal_1, track, schema);
    const auto track_id = library.track().add(track);

    // Assert
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ") Checking for presence of performance data row...");
    BOOST_CHECK(library.performance_data().exists(track_id));
}

BOOST_TEST_DECORATOR(*utf::description("get() with a valid id"))
BOOST_DATA_TEST_CASE(
    get_valid_id_gets,
    e::supported_v3_schemas* all_example_performance_data_row_types, schema,
    row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type
            << ") Creating temporary database...");
    auto library = ev3::engine_library::create_temporary(schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type << ") Adding track...");
    ev3::track_row track{ev3::TRACK_ROW_ID_NONE};
    populate_track_row(example_track_row_type::minimal_1, track, schema);
    const auto track_id = library.track().add(track);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << row_type
            << ") Getting performance data row...");
    auto actual = library.performance_data().get(track_id);

    // Assert
    BOOST_REQUIRE(actual != std::nullopt);
    BOOST_CHECK_EQUAL(actual->track_id, track_id);
}

BOOST_TEST_DECORATOR(*utf::description("update() with valid data"))
BOOST_DATA_TEST_CASE(
    update_valid_row_updates,
    e::supported_v3_schemas* all_example_performance_data_row_types, schema,
    update_row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << update_row_type
            << ") Creating temporary database...");
    auto library = ev3::engine_library::create_temporary(schema);

    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << update_row_type << ") Adding track...");
    ev3::track_row track{ev3::TRACK_ROW_ID_NONE};
    populate_track_row(example_track_row_type::minimal_1, track, schema);
    const auto track_id = library.track().add(track);

    ev3::performance_data_row expected{track_id};
    populate_performance_data_row(update_row_type, expected, schema);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << update_row_type
            << ") Updating performance data...");
    library.performance_data().update(expected);

    // Assert
    BOOST_TEST_CHECKPOINT(
        "(" << schema << ", " << update_row_type
            << ") Getting performance data...");
    auto actual = library.performance_data().get(track_id);
    BOOST_REQUIRE(actual != std::nullopt);
    BOOST_CHECK_EQUAL(expected, *actual);
}

// The act of defining very similar test cases for all the getters and setters
// on the track table is highly tedious.  As such, some macros to generate these
// more efficiently make for a more succinct way to define tests.
#define DEFINE_GETTER_VALID_TEST_CASE(engine_column, min_schema)               \
    BOOST_TEST_DECORATOR(                                                      \
        *utf::description("get_" #engine_column "() with valid track"))        \
    BOOST_DATA_TEST_CASE(                                                      \
        get_##engine_column##_with_valid_track_id_gets,                        \
        e::supported_v3_schemas, schema)                                       \
    {                                                                          \
        auto library = ev3::engine_library::create_temporary(schema);          \
        ev3::track_row track{ev3::TRACK_ROW_ID_NONE};                          \
        populate_track_row(example_track_row_type::minimal_1, track, schema);  \
        const auto track_id = library.track().add(track);                      \
                                                                               \
        ev3::performance_data_row row{track_id};                               \
        populate_performance_data_row(                                         \
            example_performance_data_row_type::fully_analysed_1, row, schema); \
        library.performance_data().update(row);                                \
        auto expected = row.engine_column;                                     \
                                                                               \
        if (schema >= min_schema)                                              \
        {                                                                      \
            auto actual =                                                      \
                library.performance_data().get_##engine_column(track_id);      \
            BOOST_CHECK_EQUAL(                                                 \
                make_printable(expected), make_printable(actual));             \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            BOOST_CHECK_THROW(                                                 \
                library.performance_data().get_##engine_column(track_id),      \
                djinterop::unsupported_operation);                             \
        }                                                                      \
    }

#define DEFINE_SETTER_VALID_TEST_CASE(engine_column, min_schema)               \
    BOOST_TEST_DECORATOR(                                                      \
        *utf::description("set_" #engine_column "() with valid track"))        \
    BOOST_DATA_TEST_CASE(                                                      \
        set_##engine_column##_with_valid_track_id_sets,                        \
        e::supported_v3_schemas, schema)                                       \
    {                                                                          \
        auto library = ev3::engine_library::create_temporary(schema);          \
        ev3::track_row track{ev3::TRACK_ROW_ID_NONE};                          \
        populate_track_row(example_track_row_type::minimal_1, track, schema);  \
        const auto track_id = library.track().add(track);                      \
                                                                               \
        ev3::performance_data_row row{track_id};                               \
        populate_performance_data_row(                                         \
            example_performance_data_row_type::minimal_1, row, schema);        \
                                                                               \
        populate_performance_data_row(                                         \
            example_performance_data_row_type::fully_analysed_1, row, schema); \
        auto expected = row.engine_column;                                     \
                                                                               \
        if (schema >= min_schema)                                              \
        {                                                                      \
            library.performance_data().set_##engine_column(                    \
                track_id, expected);                                           \
                                                                               \
            auto actual =                                                      \
                library.performance_data().get_##engine_column(track_id);      \
            BOOST_CHECK_EQUAL(                                                 \
                make_printable(expected), make_printable(actual));             \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            BOOST_CHECK_THROW(                                                 \
                library.performance_data().set_##engine_column(                \
                    track_id, expected),                                       \
                djinterop::unsupported_operation);                             \
        }                                                                      \
    }

#define DEFINE_GETTER_INVALID_TEST_CASE(engine_column, min_schema)        \
    BOOST_TEST_DECORATOR(                                                 \
        *utf::description("get_" #engine_column "() with invalid track")) \
    BOOST_DATA_TEST_CASE(                                                 \
        get_##engine_column##_with_invalid_track_id_throws,               \
        e::supported_v3_schemas, schema)                                  \
    {                                                                     \
        auto library = ev3::engine_library::create_temporary(schema);     \
                                                                          \
        if (schema >= min_schema)                                         \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                library.performance_data().get_##engine_column(12345),    \
                ev3::performance_data_row_id_error);                      \
        }                                                                 \
        else                                                              \
        {                                                                 \
            BOOST_CHECK_THROW(                                            \
                library.performance_data().get_##engine_column(12345),    \
                djinterop::unsupported_operation);                        \
        }                                                                 \
    }

#define DEFINE_SETTER_INVALID_TEST_CASE(engine_column, min_schema)             \
    BOOST_TEST_DECORATOR(                                                      \
        *utf::description("set_" #engine_column "() with invalid track"))      \
    BOOST_DATA_TEST_CASE(                                                      \
        set_##engine_column##_with_invalid_track_id_throws,                    \
        e::supported_v3_schemas, schema)                                       \
    {                                                                          \
        auto library = ev3::engine_library::create_temporary(schema);          \
        ev3::performance_data_row row{0};                                      \
        populate_performance_data_row(                                         \
            example_performance_data_row_type::fully_analysed_1, row, schema); \
                                                                               \
        if (schema >= min_schema)                                              \
        {                                                                      \
            BOOST_CHECK_THROW(                                                 \
                library.performance_data().set_##engine_column(                \
                    12345, row.engine_column),                                 \
                ev3::performance_data_row_id_error);                           \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            BOOST_CHECK_THROW(                                                 \
                library.performance_data().set_##engine_column(                \
                    12345, row.engine_column),                                 \
                djinterop::unsupported_operation);                             \
        }                                                                      \
    }

#define DEFINE_GETTER_SETTER_TEST_CASES(engine_column, min_schema) \
    DEFINE_GETTER_VALID_TEST_CASE(engine_column, min_schema)       \
    DEFINE_GETTER_INVALID_TEST_CASE(engine_column, min_schema)     \
    DEFINE_SETTER_VALID_TEST_CASE(engine_column, min_schema)       \
    DEFINE_SETTER_INVALID_TEST_CASE(engine_column, min_schema)

DEFINE_GETTER_SETTER_TEST_CASES(track_data, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(overview_waveform_data, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(beat_data, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(quick_cues, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(loops, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(third_party_source_id, es::schema_3_1_0)
DEFINE_GETTER_SETTER_TEST_CASES(active_on_load_loops, es::schema_3_1_0)

#undef DEFINE_GETTER_SETTER_TEST_CASES
#undef DEFINE_SETTER_INVALID_TEST_CASE
#undef DEFINE_SETTER_VALID_TEST_CASE
#undef DEFINE_GETTER_INVALID_TEST_CASE
#undef DEFINE_GETTER_VALID_TEST_CASE

BOOST_TEST_DECORATOR(
    *utf::description("operator<<() with valid performance data row"))
BOOST_DATA_TEST_CASE(
    operator_stream_output_on_valid_row_outputs,
    e::supported_v3_schemas* all_example_performance_data_row_types, schema,
    row_type)
{
    // Arrange
    ev3::performance_data_row row{123};
    populate_performance_data_row(row_type, row, schema);
    std::stringstream ss;

    // Act
    ss << row;

    // Assert
    BOOST_CHECK_NE(ss.str(), "");
}
