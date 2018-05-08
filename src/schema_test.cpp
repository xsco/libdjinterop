/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <schema.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE schema_test 

#include <cstdio>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "engineprime/schema_version.hpp"
#include "sqlite_modern_cpp.h"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
namespace fs = boost::filesystem;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el1"};

namespace boost {
namespace test_tools {
	template<>
	struct print_log_value<ep::schema_version>
	{
		void operator ()(std::ostream &os, const ep::schema_version &v)
		{
			::operator<<(os, v);
		}
	};

	inline bool operator ==(
			const ep::schema_version &a, const ep::schema_version &b)
	{
		return ::operator==(a, b);
	}
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
    return temp_dir;
}

BOOST_AUTO_TEST_CASE (verify_music_schema__db_at_1_0_0__verified)
{
    // Arrange
    sqlite::database db{sample_path + "/m.db"};

    // Act
    auto version = ep::verify_music_schema(db);

    // Assert
	BOOST_CHECK_EQUAL(version, ep::version_firmware_1_0_0);
}

BOOST_AUTO_TEST_CASE (verify_performance_schema__db_at_1_0_0__verified)
{
    // Arrange
    sqlite::database db{sample_path + "/p.db"};

    // Act
    auto version = ep::verify_performance_schema(db);

    // Assert
	BOOST_CHECK_EQUAL(version, ep::version_firmware_1_0_0);
}

BOOST_AUTO_TEST_CASE (create_music_schema__version_1_0_0__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "m.db";
    sqlite::database db{db_path.string()};

    // Act
    ep::create_music_schema(db, ep::version_firmware_1_0_0);

    // Assert
    ep::verify_music_schema(db);
    fs::remove_all(temp_dir);
}

BOOST_AUTO_TEST_CASE (create_performance_schema__version_1_0_0__creates_verified)
{
    // Arrange
    auto temp_dir = create_temp_dir();
    auto db_path = temp_dir / "p.db";
    sqlite::database db{db_path.string()};

    // Act
    ep::create_performance_schema(db, ep::version_firmware_1_0_0);

    // Assert
    ep::verify_performance_schema(db);
    fs::remove_all(temp_dir);
}

