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
#include "schema.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE schema_test 

#include <cstdio>
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "djinterop/enginelibrary/schema_version.hpp"
#include "sqlite_modern_cpp.h"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace el = djinterop::enginelibrary;
namespace fs = boost::filesystem;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el1"};

namespace boost {
namespace test_tools {
	template<>
	struct print_log_value<el::schema_version>
	{
		void operator ()(std::ostream &os, const el::schema_version &v)
		{
			os << v;
		}
	};
} // test_tools
} // boost

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

BOOST_AUTO_TEST_CASE (operators_equality__various__expected)
{
    // Arrange/Act/Assert
    BOOST_CHECK(  el::version_firmware_1_0_0 == el::version_firmware_1_0_0);
    BOOST_CHECK(  el::version_firmware_1_0_3 == el::version_firmware_1_0_3);
    BOOST_CHECK(!(el::version_firmware_1_0_0 == el::version_firmware_1_0_3));
    BOOST_CHECK(!(el::version_firmware_1_0_3 == el::version_firmware_1_0_0));
    BOOST_CHECK(  el::version_firmware_1_0_0 != el::version_firmware_1_0_3);
    BOOST_CHECK(  el::version_firmware_1_0_3 != el::version_firmware_1_0_0);
    BOOST_CHECK(!(el::version_firmware_1_0_0 != el::version_firmware_1_0_0));
    BOOST_CHECK(!(el::version_firmware_1_0_3 != el::version_firmware_1_0_3));
}

BOOST_AUTO_TEST_CASE (operators_ordering__various__expected)
{
    // Arrange/Act/Assert
    BOOST_CHECK(  el::version_firmware_1_0_0 <= el::version_firmware_1_0_0);
    BOOST_CHECK(  el::version_firmware_1_0_3 <= el::version_firmware_1_0_3);
    BOOST_CHECK(  el::version_firmware_1_0_0 <= el::version_firmware_1_0_3);
    BOOST_CHECK(!(el::version_firmware_1_0_3 <= el::version_firmware_1_0_0));
    BOOST_CHECK(!(el::version_firmware_1_0_0 < el::version_firmware_1_0_0));
    BOOST_CHECK(!(el::version_firmware_1_0_3 < el::version_firmware_1_0_3));
    BOOST_CHECK(  el::version_firmware_1_0_0 < el::version_firmware_1_0_3);
    BOOST_CHECK(!(el::version_firmware_1_0_3 < el::version_firmware_1_0_0));
    BOOST_CHECK(  el::version_firmware_1_0_0 >= el::version_firmware_1_0_0);
    BOOST_CHECK(  el::version_firmware_1_0_3 >= el::version_firmware_1_0_3);
    BOOST_CHECK(!(el::version_firmware_1_0_0 >= el::version_firmware_1_0_3));
    BOOST_CHECK(  el::version_firmware_1_0_3 >= el::version_firmware_1_0_0);
    BOOST_CHECK(!(el::version_firmware_1_0_0 > el::version_firmware_1_0_0));
    BOOST_CHECK(!(el::version_firmware_1_0_3 > el::version_firmware_1_0_3));
    BOOST_CHECK(!(el::version_firmware_1_0_0 > el::version_firmware_1_0_3));
    BOOST_CHECK(  el::version_firmware_1_0_3 > el::version_firmware_1_0_0);
}

BOOST_AUTO_TEST_CASE (verify_music_schema__db_at_1_0_0__verified)
{
    // Arrange
    sqlite::database db{sample_path + "/m.db"};

    // Act
    auto version = el::verify_music_schema(db);

    // Assert
	BOOST_CHECK_EQUAL(version, el::version_firmware_1_0_0);
}

BOOST_AUTO_TEST_CASE (verify_performance_schema__db_at_1_0_0__verified)
{
    // Arrange
    sqlite::database db{sample_path + "/p.db"};

    // Act
    auto version = el::verify_performance_schema(db);

    // Assert
	BOOST_CHECK_EQUAL(version, el::version_firmware_1_0_0);
}

BOOST_AUTO_TEST_CASE (create_music_schema__version_1_0_0__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "m.db";
    sqlite::database db{db_path.string()};

    // Act
    el::create_music_schema(db, el::version_firmware_1_0_0);

    // Assert
    el::verify_music_schema(db);
    fs::remove_all(temp_dir);
}

BOOST_AUTO_TEST_CASE (create_performance_schema__version_1_0_0__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "p.db";
    sqlite::database db{db_path.string()};

    // Act
    el::create_performance_schema(db, el::version_firmware_1_0_0);

    // Assert
    el::verify_performance_schema(db);
    remove_temp_dir(temp_dir);
}

BOOST_AUTO_TEST_CASE (create_music_schema__version_1_0_3__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "m.db";
    sqlite::database db{db_path.string()};

    // Act
    el::create_music_schema(db, el::version_firmware_1_0_3);

    // Assert
    el::verify_music_schema(db);
    fs::remove_all(temp_dir);
}

BOOST_AUTO_TEST_CASE (create_performance_schema__version_1_0_3__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "p.db";
    sqlite::database db{db_path.string()};

    // Act
    el::create_performance_schema(db, el::version_firmware_1_0_3);

    // Assert
    el::verify_performance_schema(db);
    remove_temp_dir(temp_dir);
}

