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

#pragma once

#include <cassert>

#include <djinterop/database.hpp>

/// Assertion macro allowing a custom message to be printed in case of failure.
#define assertm(exp, msg) assert(((void)msg, exp))

/// Macro to skip a test if required database features are not met.
#define DJINTEROP_TEST_NEEDS_FEATURE(db, feature) \
    if (!db.features().test(feature))              \
    {                                              \
        return;                                    \
    }
