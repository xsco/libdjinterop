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
#include <engineprime/engineprime.hpp>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE database_test

#include <boost/test/unit_test.hpp>
#include <string>

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

namespace ep = engineprime;
using namespace std;

const std::string sample_path{STRINGIFY(TESTDATA_DIR) "/el1"};


BOOST_AUTO_TEST_CASE (ctor)
{
    // Simply try to construct the database on the provided path
    ep::database{sample_path};
}

BOOST_AUTO_TEST_CASE (information)
{
    // Assess correctness of values sourced from the 'Information' table
    ep::database db{sample_path};
    
    BOOST_CHECK_EQUAL(db.directory_path(), sample_path);
    BOOST_CHECK_EQUAL(db.music_db_path(), sample_path + "/m.db");
    BOOST_CHECK_EQUAL(db.performance_db_path(), sample_path + "/p.db");
    
    BOOST_CHECK_EQUAL(db.uuid(), "e535b170-26ef-4f30-8cb2-5b9fa4c2a27f");
    BOOST_CHECK_EQUAL(db.schema_version_major(), 1);
    BOOST_CHECK_EQUAL(db.schema_version_minor(), 6);
    BOOST_CHECK_EQUAL(db.schema_version_patch(), 0);
}

