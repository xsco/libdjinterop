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
#include <djinterop/enginelibrary/crate.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE crate_test

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

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
    el::database db{sample_path};
    fs::path m_db_path{db.music_db_path()};
    fs::path p_db_path{db.performance_db_path()};
    fs::copy_file(m_db_path, temp_dir / m_db_path.filename());
    fs::copy_file(p_db_path, temp_dir / p_db_path.filename());
}

static void populate_crate_1(el::crate &c)
{
    c.set_name("Foo Crate");
    c.set_no_parent();
    c.clear_tracks();
}

static void populate_crate_2(el::crate &c)
{
    c.set_name("Bar Crate");
    c.set_no_parent();
    c.clear_tracks();
}

static void check_crate_2(el::crate &c)
{
    BOOST_CHECK_EQUAL(c.name(), "Bar Crate");
    BOOST_CHECK_EQUAL(c.has_parent(), false);

    auto child_iter = c.child_crates_begin();
    BOOST_CHECK(child_iter == c.child_crates_end());

    auto track_iter = c.tracks_begin();
    BOOST_CHECK(track_iter == c.tracks_end());
}

static void populate_crate_3(el::crate &c)
{
    c.set_name("Sub-Foo Crate");
    c.set_parent_id(1);
    std::unordered_set<int> track_ids;
    track_ids.insert(1);
    c.set_tracks(std::begin(track_ids), std::end(track_ids));
}

static void check_crate_3(el::crate &c)
{
    BOOST_CHECK_EQUAL(c.name(), "Sub-Foo Crate");
    BOOST_CHECK_EQUAL(c.has_parent(), true);
    BOOST_CHECK_EQUAL(c.parent_id(), 1);

    auto child_iter = c.child_crates_begin();
    BOOST_REQUIRE(child_iter != c.child_crates_end());
    BOOST_CHECK_EQUAL(*child_iter, 4);
    ++child_iter;
    BOOST_CHECK(child_iter == c.child_crates_end());

    auto track_iter = c.tracks_begin();
    BOOST_REQUIRE(track_iter != c.tracks_end());
    BOOST_CHECK_EQUAL(*track_iter, 1);
    ++track_iter;
    BOOST_CHECK(track_iter == c.tracks_end());
}

BOOST_AUTO_TEST_CASE (all_crate_ids__sample_db__expected_ids)
{
    // Arrange
    el::database db{sample_path};
    
    // Act
    auto results = el::all_crate_ids(db);
    
    // Assert
    BOOST_CHECK_EQUAL(results.size(), 4);
    BOOST_CHECK_EQUAL(results[0], 1);
    BOOST_CHECK_EQUAL(results[1], 2);
    BOOST_CHECK_EQUAL(results[2], 3);
    BOOST_CHECK_EQUAL(results[3], 4);
}

BOOST_AUTO_TEST_CASE (all_root_crate_ids__sample_db__expected_ids)
{
    // Arrange
    el::database db{sample_path};
    
    // Act
    auto results = el::all_root_crate_ids(db);
    
    // Assert
    BOOST_CHECK_EQUAL(results.size(), 2);
    BOOST_CHECK_EQUAL(results[0], 1);
    BOOST_CHECK_EQUAL(results[1], 2);
}

BOOST_AUTO_TEST_CASE (ctor__crate3__correct_fields)
{
	// Arrange
	el::database db{sample_path};

	// Act
	el::crate c{db, 3};

	// Assert
	BOOST_CHECK_EQUAL(c.id(), 3);
    check_crate_3(c);
}

BOOST_AUTO_TEST_CASE (ctor__nonexistent_crate__throws)
{
	// Arrange
	el::database db{sample_path};

	// Act / Assert
	// Note the extra brackets around the invocation, because the preprocessor
	// doesn't understand curly braces!
	BOOST_CHECK_EXCEPTION((el::crate{db, 123}), el::nonexistent_crate,
			[](const el::nonexistent_crate &e) { return e.id() == 123; });
}

BOOST_AUTO_TEST_CASE (setters__good_values__values_stored)
{
    // Arrange
    el::crate c{};

    // Act
    populate_crate_2(c);

    // Assert
    BOOST_CHECK_EQUAL(c.id(), 0);
    check_crate_2(c);
}

BOOST_AUTO_TEST_CASE (save__new_crate_no_values__throws)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    el::crate c{};

    // Act/Assert
    BOOST_CHECK_THROW(c.save(db), std::invalid_argument);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__new_crate_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    el::crate c{};
    populate_crate_2(c);

    // Act
    c.save(db);

    // Assert
    BOOST_CHECK_NE(c.id(), 0);
    check_crate_2(c);
    el::crate c_reloaded{db, c.id()};
    check_crate_2(c_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (ctor_copy__saved_track__zero_id_and_copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    el::crate c{};
    populate_crate_2(c);
    c.save(db);

    // Act
    el::crate copy{c};

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), 0);
    check_crate_2(c);
    check_crate_2(copy);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__existing_track_good_values__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    el::crate c{};
    populate_crate_1(c);
    c.save(db);
    auto crate_id = c.id();
    populate_crate_2(c);

    // Act
    c.save(db);

    // Assert
    BOOST_CHECK_EQUAL(c.id(), crate_id);
    el::crate c_reloaded{db, c.id()};
    check_crate_2(c);
    check_crate_2(c_reloaded);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__change_hierarchy__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    // Arrange a hierarchy of c1 (root) -> c2 -> c3
    el::crate c1{};
    el::crate c2{};
    el::crate c3{};
    c1.set_name("Grandfather");
    c1.save(db);
    c2.set_name("Father");
    c2.set_parent_id(c1.id());
    c2.save(db);
    c3.set_name("Son");
    c3.set_parent_id(c2.id());
    c3.save(db);
    // Change c2's parent
    c2.set_no_parent();

    // Act
    c2.save(db);

    // Assert
    BOOST_CHECK(c1.has_parent() == false);
    BOOST_CHECK(c2.has_parent() == false);
    BOOST_CHECK(c3.has_parent() == true);
    BOOST_CHECK_EQUAL(c3.parent_id(), c2.id());
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (save__add_tracks__saves)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    copy_test_db_to_temp_dir(temp_dir);
    el::database db{temp_dir.string()};
    el::crate c{db, 2};
    c.add_track(1);

    // Act
    c.save(db);

    // Assert
    el::crate c_reloaded{db, 2};
    auto track_iter = c_reloaded.tracks_begin();
    BOOST_REQUIRE(track_iter != c_reloaded.tracks_end());
    BOOST_CHECK_EQUAL(*track_iter, 1);
    ++track_iter;
    BOOST_CHECK(track_iter == c_reloaded.tracks_end());
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (op_copy_assign__saved_track__zero_id_and_copied_fields)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db = el::create_database(temp_dir.string(), el::version_1_7_1);
    el::crate c{};
    populate_crate_2(c);
    c.save(db);

    // Act
    auto copy = c;

    // Assert
    BOOST_CHECK_EQUAL(copy.id(), 0);
    check_crate_2(copy);
    remove_temp_dir(temp_dir);
}
