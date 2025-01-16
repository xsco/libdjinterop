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

#include <djinterop/config.hpp>
#include <djinterop/engine/v2/information_table.hpp>

namespace djinterop::engine::v3
{
/// Represents the single-row contents of the `Information` table.
using information_row = djinterop::engine::v2::information_row;

/// Represents the `Information` table in an Engine v2 database.
///
/// Note that the `Information` table in a valid database always has exactly
/// one row.  Zero rows or more than one is considered an invalid database.
using information_table = djinterop::engine::v2::information_table;

}  // namespace djinterop::engine::v3
