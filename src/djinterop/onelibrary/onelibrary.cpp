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

#include <djinterop/onelibrary/onelibrary.hpp>

#include <array>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include <djinterop/exceptions.hpp>

#include "../util/sqlcipher.hpp"
#include "onelibrary_context.hpp"
#include "v1/database_impl.hpp"

namespace djinterop::onelibrary
{
namespace
{
struct resolved_location
{
    /// Root directory of the device, to which track paths are relative.
    std::string directory;

    std::string database_path;
};

/// Work out where the database is, given a device or the file itself.
resolved_location resolve(const std::string& path)
{
    if (std::filesystem::is_directory(path))
        return resolved_location{path, path + "/" + database_relative_path};

    // A path that names the database directly implies its device root, which
    // is three levels up: `<root>/PIONEER/rekordbox/exportLibrary.db`.  A
    // relative path with nothing above it sits in the working directory, which
    // is then the root of the device.
    auto root =
        std::filesystem::path{path}.parent_path().parent_path().parent_path();
    if (root.empty())
        root = ".";

    return resolved_location{root.string(), path};
}

}  // anonymous namespace

void verify_schema(onelibrary_context& context)
{
    // A real export has twenty-two tables; demanding the ones this library
    // does not read would reject a database that is merely older or newer.
    constexpr std::array<const char*, 8> required_tables{
        "content",  "artist",           "album",   "genre", "label",
        "playlist", "playlist_content", "property"};

    std::set<std::string> present;
    context.db << "SELECT name FROM sqlite_master WHERE type = 'table'" >>
        [&](std::string name) { present.insert(std::move(name)); };

    for (const auto& table : required_tables)
        if (present.count(table) == 0)
            throw database_inconsistency{
                std::string{"The table `"} + table +
                "` is missing, so this is not a OneLibrary database"};
}

std::shared_ptr<onelibrary_context> load_context(
    const std::string& path, const std::string& passphrase)
{
    const auto location = resolve(path);

    if (!std::filesystem::exists(location.database_path))
        throw database_not_found{location.database_path};

    auto context = std::make_shared<onelibrary_context>(
        location.directory, util::open_encrypted_database(
                                location.database_path, passphrase));

    // Fail here, while the caller still has the path in hand.
    verify_schema(*context);

    return context;
}

bool database_exists(const std::string& path)
{
    const auto location = resolve(path);
    return std::filesystem::exists(location.database_path);
}

database load_database(const std::string& path, const std::string& passphrase)
{
    return database{
        std::make_shared<v1::database_impl>(load_context(path, passphrase))};
}

}  // namespace djinterop::onelibrary
