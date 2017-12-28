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

#include <boost/test/unit_test.hpp>
#include "sqlite_modern_cpp.h"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el1"};

namespace boost {
namespace test_tools {

} // test_tools
} // boost

BOOST_AUTO_TEST_CASE (verify_music_db)
{
    sqlite::database db{sample_path + "/m.db"};
    ep::verify_music_schema(db);
}

BOOST_AUTO_TEST_CASE (verify_performance_db)
{
    sqlite::database db{sample_path + "/p.db"};
    ep::verify_performance_schema(db);
}

