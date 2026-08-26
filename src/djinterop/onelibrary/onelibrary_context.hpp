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

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <sqlite_modern_cpp.h>

#include <djinterop/exceptions.hpp>

namespace djinterop::onelibrary
{
/// State shared by everything belonging to one loaded OneLibrary database.
struct onelibrary_context
{
    onelibrary_context(std::string directory, sqlite::database db) :
        directory{std::move(directory)}, db{std::move(db)}
    {
    }

    /// Root directory of the device.
    ///
    /// Track paths in the database are relative to this, not to the directory
    /// the database file itself sits in.
    const std::string directory;

    /// The database, decrypted from the device into memory.
    sqlite::database db;
};

/// Refuse an operation that would change the database.
[[noreturn]] inline void read_only()
{
    throw unsupported_operation{
        "OneLibrary databases are currently read-only in libdjinterop"};
}

/// Run a query whose rows are each a single identifier.
template <typename... Args>
std::vector<int64_t> collect_ids(
    onelibrary_context& context, const char* sql, const Args&... args)
{
    std::vector<int64_t> results;
    auto query = context.db << sql;
    ((query << args), ...);
    query >> [&](int64_t id) { results.push_back(id); };
    return results;
}

/// Run a query whose rows are each a single identifier, and take the first.
template <typename... Args>
std::optional<int64_t> first_id(
    onelibrary_context& context, const char* sql, const Args&... args)
{
    std::optional<int64_t> result;
    auto query = context.db << sql;
    ((query << args), ...);
    query >> [&](int64_t id) { result = id; };
    return result;
}

/// Test whether a query matches any row at all.
template <typename... Args>
bool any_row(onelibrary_context& context, const char* sql, const Args&... args)
{
    return first_id(context, sql, args...).has_value();
}

}  // namespace djinterop::onelibrary
