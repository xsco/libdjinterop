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
#include <fstream>
#include <memory>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
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
    constexpr std::array<std::string_view, 9> required_tables{
        "content",  "artist",   "album",            "genre",   "label",
        "key",      "playlist", "playlist_content", "property"};

    std::unordered_set<std::string> present;
    context.db << "SELECT name FROM sqlite_master WHERE type = 'table'" >>
        [&](std::string name) { present.insert(std::move(name)); };

    for (const auto& table : required_tables)
    {
        std::string name{table};
        if (present.count(name) == 0)
            throw database_inconsistency{
                "The table `" + name +
                "` is missing, so this is not a valid OneLibrary database"};
    }

    // Every export carries exactly one `property` row, recording the version
    // of its schema.  A database without one has not been written properly.
    int64_t property_rows = 0;
    int64_t versioned_rows = 0;
    context.db << "SELECT COUNT(*), COUNT(NULLIF(dbVersion, '')) "
                  "FROM property" >>
        std::tie(property_rows, versioned_rows);

    if (property_rows != 1)
        throw database_inconsistency{
            "The table `property` holds " + std::to_string(property_rows) +
            " rows rather than one, so this is not a valid OneLibrary "
            "database"};

    if (versioned_rows != 1)
        throw database_inconsistency{
            "The table `property` records no `dbVersion`, so this is not a "
            "valid OneLibrary database"};
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

std::shared_ptr<onelibrary_context> create_context_from_scripts(
    const std::string& directory, const std::string& script_directory,
    const std::string& passphrase)
{
    const auto script_path = script_directory + "/exportLibrary.db.sql";
    std::ifstream script{script_path};
    if (!script)
        throw std::runtime_error{
            "Cannot read the script `" + script_path + "`"};

    const auto database_path = directory + "/" + database_relative_path;
    if (std::filesystem::exists(database_path))
        throw std::runtime_error{
            "A database already exists at `" + database_path + "`"};

    std::filesystem::create_directories(
        std::filesystem::path{database_path}.parent_path());

    {
        auto db = util::create_encrypted_database(database_path, passphrase);

        // One statement per line, as the Engine scripts are written, with
        // blank lines and comments between them.
        std::string statement;
        while (std::getline(script, statement))
        {
            if (statement.empty() || statement.rfind("--", 0) == 0)
                continue;

            try
            {
                db << statement;
            }
            catch (const std::exception& e)
            {
                throw std::runtime_error{
                    "Error in script `" + script_path +
                    "` whilst executing line \"" + statement + "\": " +
                    e.what()};
            }
        }
    }

    return load_context(directory, passphrase);
}

database load_database(const std::string& path, const std::string& passphrase)
{
    return database{
        std::make_shared<v1::database_impl>(load_context(path, passphrase))};
}

database create_database_from_scripts(
    const std::string& directory, const std::string& script_directory,
    const std::string& passphrase)
{
    return database{std::make_shared<v1::database_impl>(
        create_context_from_scripts(directory, script_directory, passphrase))};
}

}  // namespace djinterop::onelibrary
