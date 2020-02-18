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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE crate_test

#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <djinterop/crate.hpp>
#include <djinterop/database.hpp>
#include <djinterop/enginelibrary.hpp>
#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace el = djinterop::enginelibrary;
namespace fs = boost::filesystem;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el3"};

static fs::path create_temp_dir()
{
    fs::path temp_dir{fs::temp_directory_path()};
    temp_dir /= fs::unique_path();
    if (!fs::create_directory(temp_dir))
    {
        throw std::runtime_error{"Failed to create tmp_dir"};
    }
    std::cout << "Created temp dir at " << temp_dir.string() << std::endl;
    return temp_dir;
}

static void remove_temp_dir(const fs::path &temp_dir)
{
    fs::remove_all(temp_dir);
    std::cout << "Removed temp dir at " << temp_dir.string() << std::endl;
}

static void copy_test_db_to_temp_dir(const fs::path &temp_dir)
{
    auto db = el::load_database(sample_path);
    fs::path music_db_path{el::music_db_path(db)};
    fs::path perfdata_db_path{el::perfdata_db_path(db)};
    fs::copy_file(music_db_path, temp_dir / music_db_path.filename());
    fs::copy_file(perfdata_db_path, temp_dir / perfdata_db_path.filename());
}

static void check_crate_2(djinterop::crate c)
{
    BOOST_CHECK(c.is_valid());
    BOOST_CHECK_EQUAL(c.name(), "Bar Crate");
    BOOST_CHECK(!c.parent());
    BOOST_CHECK(c.children().empty());
    BOOST_CHECK(c.descendants().empty());
    BOOST_CHECK(c.tracks().empty());
}

/*
// Not currently used in any test cases.
static void populate_crate_3(djinterop::crate &c)
{
    c.set_name("Sub-Foo Crate");
    c.set_parent_id(1);
    std::unordered_set<int> track_ids;
    track_ids.insert(1);
    c.set_tracks(std::begin(track_ids), std::end(track_ids));
}
*/

static void check_crate_3(djinterop::crate &c)
{
    BOOST_CHECK_EQUAL(c.name(), "Sub-Foo Crate");
    BOOST_CHECK(c.parent());
    BOOST_CHECK_EQUAL(c.parent()->id(), 1);

    auto children = c.children();
    BOOST_REQUIRE_EQUAL(children.size(), 1);
    BOOST_CHECK_EQUAL(children[0].id(), 4);

    auto tracks = c.tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 1);
    BOOST_CHECK_EQUAL(tracks[0].id(), 1);
}

BOOST_AUTO_TEST_CASE(all_crates__sample_db__expected_ids)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto results = db.crates();

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 4);
    BOOST_CHECK_EQUAL(results[0].id(), 1);
    BOOST_CHECK_EQUAL(results[1].id(), 2);
    BOOST_CHECK_EQUAL(results[2].id(), 3);
    BOOST_CHECK_EQUAL(results[3].id(), 4);
}

BOOST_AUTO_TEST_CASE(all_root_crates__sample_db__expected_ids)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto results = db.root_crates();

    // Assert
    BOOST_CHECK_EQUAL(results.size(), 2);
    BOOST_CHECK_EQUAL(results[0].id(), 1);
    BOOST_CHECK_EQUAL(results[1].id(), 2);
}

BOOST_AUTO_TEST_CASE(ctor__crate3__correct_fields)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto c = *db.crate_by_id(3);

    // Assert
    BOOST_CHECK_EQUAL(c.id(), 3);
    check_crate_3(c);
}

BOOST_AUTO_TEST_CASE(ctor__nonexistent_crate__none)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act / Assert
    BOOST_CHECK(!db.crate_by_id(123));
}

BOOST_AUTO_TEST_CASE(create_root_crate__good_values__succeeds)
{
    // Arrange/Act
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Bar Crate");

    // Assert
    BOOST_CHECK_NE(c.id(), 0);
    check_crate_2(c);
    auto c_reloaded = *db.crate_by_id(c.id());
    check_crate_2(c_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(ctor_copy__saved_track__copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Bar Crate");

    // Act
    djinterop::crate copy{c};

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), c.id());
    check_crate_2(c);
    check_crate_2(copy);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(set_name__existing_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Foo Crate");
    auto crate_id = c.id();

    // Act
    c.set_name("Bar Crate");

    // Assert
    BOOST_CHECK_EQUAL(c.id(), crate_id);
    auto c_reloaded = *db.crate_by_id(c.id());
    check_crate_2(c);
    check_crate_2(c_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(set_parent__change_hierarchy__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    // Arrange a hierarchy of c1 (root) -> c2 -> c3
    auto c1 = db.create_root_crate("Grandfather");
    auto c2 = db.create_root_crate("Father");
    auto c3 = db.create_root_crate("Son");

    // Act
    c2.set_parent(c1);
    c3.set_parent(c2);
    // Change c2's parent
    c2.set_parent({});

    // Assert
    BOOST_CHECK(!c1.parent());
    BOOST_CHECK(!c2.parent());
    BOOST_CHECK(c3.parent());
    BOOST_CHECK_EQUAL(c3.parent()->id(), c2.id());
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(add_track__valid_track__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    copy_test_db_to_temp_dir(temp_dir);
    auto db = el::load_database(temp_dir.string());
    auto c = *db.crate_by_id(2);

    // Act
    c.add_track(*db.track_by_id(1));

    // Assert
    auto c_reloaded = *db.crate_by_id(2);
    auto tracks = c_reloaded.tracks();
    BOOST_REQUIRE_EQUAL(tracks.size(), 1);
    BOOST_CHECK_EQUAL(tracks[0].id(), 1);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(op_copy_assign__saved_track__copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Bar Crate");

    // Act
    auto copy = c;

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), c.id());
    check_crate_2(copy);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE(crate_by_name__crate2__found)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto crates = db.crates_by_name("Bar Crate");

    // Assert
    BOOST_CHECK_EQUAL(crates.size(), 1);
    BOOST_CHECK_EQUAL(crates[0].id(), 2);
}

BOOST_AUTO_TEST_CASE(crate_by_name__invalid_crate__not_found)
{
    // Arrange
    auto db = el::load_database(sample_path);

    // Act
    auto crates = db.crates_by_name("Non-existent Crate");

    // Assert
    BOOST_CHECK(crates.empty());
}

BOOST_AUTO_TEST_CASE(set_name__invalid_name__throws)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Root");

    // Act/Assert
    BOOST_CHECK_THROW(
        c.set_name(""), djinterop::crate_invalid_name);
    BOOST_CHECK_THROW(
        c.set_name("Contains ; semicolon"), djinterop::crate_invalid_name);
}

BOOST_AUTO_TEST_CASE(create_sub_crate__valid_name__succeeds)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    auto c = db.create_root_crate("Root");

    // Act
    auto sc = c.create_sub_crate("Sub-Crate");

    // Assert
    auto parent = sc.parent();
    BOOST_CHECK(parent);
    BOOST_CHECK_EQUAL(parent.value().id(), c.id());
    auto children = c.children();
    BOOST_CHECK_EQUAL(children.size(), 1);
    auto child = children[0];
    BOOST_CHECK_EQUAL(child.id(), sc.id());
}

