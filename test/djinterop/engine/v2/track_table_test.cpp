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

#define BOOST_TEST_MODULE engine_v2_track_table_test
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

#include <sstream>

#include <djinterop/engine/engine.hpp>
#include <djinterop/engine/v2/engine_library.hpp>

#include "../../boost_test_printable.hpp"
#include "../../boost_test_utils.hpp"
#include "../../temporary_directory.hpp"
#include "example_track_row_data.hpp"

namespace utf = boost::unit_test;
namespace e = djinterop::engine;
namespace ev2 = djinterop::engine::v2;

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
    add__valid__adds, e::all_v2_versions* all_example_track_row_types, version,
    row_type)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(version);
    auto track_tbl = library.track();
    ev2::track_row row{0};
    populate_track_row(row_type, row);

    // Act
    auto id = track_tbl.add(row);

    // Assert
    BOOST_CHECK_NE(id, 0);
}

BOOST_TEST_DECORATOR(*utf::description("add() with an existing id"))
BOOST_DATA_TEST_CASE(add__existing_id__throws, e::all_v2_versions, version)
{
    // Arrange
    auto library = ev2::engine_library::create_temporary(version);
    auto track_tbl = library.track();
    ev2::track_row row{123};

    // Act/Assert
    BOOST_CHECK_THROW(track_tbl.add(row), ev2::track_row_id_error);
}

BOOST_TEST_DECORATOR(*utf::description("get() with a valid id"))
BOOST_DATA_TEST_CASE(
    get__valid__gets, e::all_v2_versions* all_example_track_row_types, version,
    row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << row_type
            << ") Creating temporary database...");
    auto library = ev2::engine_library::create_temporary(version);

    auto track_tbl = library.track();
    ev2::track_row expected{0};
    populate_track_row(row_type, expected);

    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << row_type << ") Creating track...");
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
        "(" << version << ", " << row_type << ") Fetching track...");
    auto actual = track_tbl.get(id);

    // Assert
    BOOST_CHECK(actual != djinterop::stdx::nullopt);
    BOOST_CHECK_EQUAL(expected, *actual);
}

BOOST_TEST_DECORATOR(*utf::description("update() with valid data"))
BOOST_DATA_TEST_CASE(
    update__valid__updates,
    e::all_v2_versions* all_example_track_row_types*
        all_example_track_row_types,
    version, initial_row_type, update_row_type)
{
    // Arrange
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << initial_row_type << ", " << update_row_type
            << ") Creating temporary database...");
    auto library = ev2::engine_library::create_temporary(version);

    auto track_tbl = library.track();
    ev2::track_row initial{0};
    populate_track_row(initial_row_type, initial);

    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << initial_row_type << ", " << update_row_type
            << ") Creating initial track...");
    auto id = track_tbl.add(initial);

    ev2::track_row expected{id};
    populate_track_row(update_row_type, expected);

    // Act
    BOOST_TEST_CHECKPOINT(
        "(" << version << ", " << initial_row_type << ", " << update_row_type
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
        "(" << version << ", " << initial_row_type << ", " << update_row_type
            << ") Fetching track...");
    auto actual = track_tbl.get(id);
    BOOST_CHECK(actual != djinterop::stdx::nullopt);
    BOOST_CHECK_EQUAL(expected, *actual);
}

// The act of defining very similar test cases for all the getters and setters
// on the track table is highly tedious.  As such, some macros to generate these
// more efficiently make for a more succinct way to define tests.
#define DEFINE_GETTER_VALID_TEST_CASE(engine_column)                         \
    BOOST_TEST_DECORATOR(                                                    \
        *utf::description("get_" #engine_column "() with valid track"))      \
    BOOST_DATA_TEST_CASE(                                                    \
        get_##engine_column##__expected, e::all_v2_versions, version)        \
    {                                                                        \
        auto library = ev2::engine_library::create_temporary(version);       \
        auto track_tbl = library.track();                                    \
        ev2::track_row row{0};                                               \
        populate_track_row(example_track_row_type::fully_analysed_1, row);   \
        auto id = track_tbl.add(row);                                        \
        auto expected = row.engine_column;                                   \
                                                                             \
        auto actual = track_tbl.get_##engine_column(id);                     \
                                                                             \
        BOOST_CHECK_EQUAL(make_printable(expected), make_printable(actual)); \
    }

#define DEFINE_SETTER_VALID_TEST_CASE(engine_column)                         \
    BOOST_TEST_DECORATOR(                                                    \
        *utf::description("set_" #engine_column "() with valid track"))      \
    BOOST_DATA_TEST_CASE(                                                    \
        set_##engine_column##__valid__expected, e::all_v2_versions, version) \
    {                                                                        \
        auto library = ev2::engine_library::create_temporary(version);       \
        auto track_tbl = library.track();                                    \
        ev2::track_row row{0};                                               \
        populate_track_row(example_track_row_type::minimal_1, row);          \
        auto id = track_tbl.add(row);                                        \
                                                                             \
        populate_track_row(example_track_row_type::fully_analysed_1, row);   \
        auto expected = row.engine_column;                                   \
                                                                             \
        track_tbl.set_##engine_column(id, expected);                         \
                                                                             \
        auto actual = track_tbl.get_##engine_column(id);                     \
        BOOST_CHECK_EQUAL(make_printable(expected), make_printable(actual)); \
    }

#define DEFINE_GETTER_INVALID_TEST_CASE(engine_column)                       \
    BOOST_TEST_DECORATOR(                                                    \
        *utf::description("get_" #engine_column "() with invalid track"))    \
    BOOST_DATA_TEST_CASE(                                                    \
        get_##engine_column##__invalid__throws, e::all_v2_versions, version) \
    {                                                                        \
        auto library = ev2::engine_library::create_temporary(version);       \
        auto track_tbl = library.track();                                    \
                                                                             \
        BOOST_CHECK_THROW(                                                   \
            track_tbl.get_##engine_column(12345), ev2::track_row_id_error);  \
    }

#define DEFINE_SETTER_INVALID_TEST_CASE(engine_column)                       \
    BOOST_TEST_DECORATOR(                                                    \
        *utf::description("set_" #engine_column "() with invalid track"))    \
    BOOST_DATA_TEST_CASE(                                                    \
        set_##engine_column##__invalid__throws, e::all_v2_versions, version) \
    {                                                                        \
        auto library = ev2::engine_library::create_temporary(version);       \
        auto track_tbl = library.track();                                    \
        ev2::track_row row{0};                                               \
        populate_track_row(example_track_row_type::fully_analysed_1, row);   \
                                                                             \
        BOOST_CHECK_THROW(                                                   \
            track_tbl.set_##engine_column(12345, row.engine_column),         \
            ev2::track_row_id_error);                                        \
    }

#define DEFINE_GETTER_SETTER_TEST_CASES(engine_column) \
    DEFINE_GETTER_VALID_TEST_CASE(engine_column)       \
    DEFINE_GETTER_INVALID_TEST_CASE(engine_column)     \
    DEFINE_SETTER_VALID_TEST_CASE(engine_column)       \
    DEFINE_SETTER_INVALID_TEST_CASE(engine_column)

DEFINE_GETTER_SETTER_TEST_CASES(play_order)
DEFINE_GETTER_SETTER_TEST_CASES(length)
DEFINE_GETTER_SETTER_TEST_CASES(bpm)
DEFINE_GETTER_SETTER_TEST_CASES(year)
DEFINE_GETTER_SETTER_TEST_CASES(path)
DEFINE_GETTER_SETTER_TEST_CASES(filename)
DEFINE_GETTER_SETTER_TEST_CASES(bitrate)
DEFINE_GETTER_SETTER_TEST_CASES(bpm_analyzed)
DEFINE_GETTER_SETTER_TEST_CASES(album_art_id)
DEFINE_GETTER_SETTER_TEST_CASES(file_bytes)
DEFINE_GETTER_SETTER_TEST_CASES(title)
DEFINE_GETTER_SETTER_TEST_CASES(artist)
DEFINE_GETTER_SETTER_TEST_CASES(album)
DEFINE_GETTER_SETTER_TEST_CASES(genre)
DEFINE_GETTER_SETTER_TEST_CASES(comment)
DEFINE_GETTER_SETTER_TEST_CASES(label)
DEFINE_GETTER_SETTER_TEST_CASES(composer)
DEFINE_GETTER_SETTER_TEST_CASES(remixer)
DEFINE_GETTER_SETTER_TEST_CASES(key)
DEFINE_GETTER_SETTER_TEST_CASES(rating)
DEFINE_GETTER_SETTER_TEST_CASES(album_art)
DEFINE_GETTER_SETTER_TEST_CASES(time_last_played)
DEFINE_GETTER_SETTER_TEST_CASES(is_played)
DEFINE_GETTER_SETTER_TEST_CASES(file_type)
DEFINE_GETTER_SETTER_TEST_CASES(is_analyzed)
DEFINE_GETTER_SETTER_TEST_CASES(date_created)
DEFINE_GETTER_SETTER_TEST_CASES(date_added)
DEFINE_GETTER_SETTER_TEST_CASES(is_available)
DEFINE_GETTER_SETTER_TEST_CASES(is_metadata_of_packed_track_changed)
DEFINE_GETTER_SETTER_TEST_CASES(is_performance_data_of_packed_track_changed)
DEFINE_GETTER_SETTER_TEST_CASES(played_indicator)
DEFINE_GETTER_SETTER_TEST_CASES(is_metadata_imported)
DEFINE_GETTER_SETTER_TEST_CASES(pdb_import_key)
DEFINE_GETTER_SETTER_TEST_CASES(streaming_source)
DEFINE_GETTER_SETTER_TEST_CASES(uri)
DEFINE_GETTER_SETTER_TEST_CASES(is_beat_grid_locked)
DEFINE_GETTER_SETTER_TEST_CASES(origin_database_uuid)
DEFINE_GETTER_SETTER_TEST_CASES(origin_track_id)
DEFINE_GETTER_SETTER_TEST_CASES(track_data)
DEFINE_GETTER_SETTER_TEST_CASES(overview_waveform_data)
DEFINE_GETTER_SETTER_TEST_CASES(beat_data)
DEFINE_GETTER_SETTER_TEST_CASES(quick_cues)
DEFINE_GETTER_SETTER_TEST_CASES(loops)
DEFINE_GETTER_SETTER_TEST_CASES(third_party_source_id)
DEFINE_GETTER_SETTER_TEST_CASES(streaming_flags)
DEFINE_GETTER_SETTER_TEST_CASES(explicit_lyrics)

#undef DEFINE_GETTER_SETTER_TEST_CASES
#undef DEFINE_SETTER_INVALID_TEST_CASE
#undef DEFINE_SETTER_VALID_TEST_CASE
#undef DEFINE_GETTER_INVALID_TEST_CASE
#undef DEFINE_GETTER_VALID_TEST_CASE

BOOST_TEST_DECORATOR(*utf::description("operator<<() with valid track row"))
BOOST_DATA_TEST_CASE(
    operator_stream_output__valid__outputs, e::all_v2_versions* all_example_track_row_types, version,
    row_type)
{
    // Arrange
    ev2::track_row row{0};
    populate_track_row(row_type, row);
    std::stringstream ss;

    // Act
    ss << row;

    // Assert
    BOOST_CHECK_NE(ss.str(), "");
}

