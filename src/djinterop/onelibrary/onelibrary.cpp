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

#include "../util/filesystem.hpp"
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
    std::filesystem::path directory;

    std::filesystem::path database_path;
};

/// Work out where the database is, given a device or the file itself.
resolved_location resolve(const std::filesystem::path& path)
{
    if (std::filesystem::is_directory(path))
        return resolved_location{path, path / database_relative_path};

    // A path that names the database directly implies its device root, which
    // is three levels up: `<root>/PIONEER/rekordbox/exportLibrary.db`.  A
    // relative path with nothing above it sits in the working directory, which
    // is then the root of the device.
    auto root = path.parent_path().parent_path().parent_path();
    if (root.empty())
        root = ".";

    return resolved_location{root, path};
}

}  // anonymous namespace

void verify_schema(onelibrary_context& context)
{
    constexpr std::array<std::string_view, 22> required_tables{
        "content",            "artist",
        "album",              "genre",
        "label",              "key",
        "color",              "image",
        "cue",                "playlist",
        "playlist_content",   "history",
        "history_content",    "hotCueBankList",
        "hotCueBankList_cue", "myTag",
        "myTag_content",      "menuItem",
        "category",           "sort",
        "property",           "recommendedLike",
    };

    constexpr std::array<std::string_view, 4> required_indexes{
        "index_playlist_content_playlist_id",
        "index_myTag_content_myTag_id",
        "index_myTag_content_content_id",
        "index_hotCueBankList_cue_hotCueBankList_id",
    };

    std::unordered_set<std::string> tables;
    context.db << "SELECT name FROM sqlite_master WHERE type = 'table'" >>
        [&](std::string name) { tables.insert(std::move(name)); };

    std::unordered_set<std::string> indexes;
    context.db << "SELECT name FROM sqlite_master WHERE type = 'index'" >>
        [&](std::string name) { indexes.insert(std::move(name)); };

    for (const auto& table : required_tables)
    {
        std::string name{table};
        if (tables.count(name) == 0)
            throw database_inconsistency{
                "The table `" + name +
                "` is missing, so this is not a valid OneLibrary database"};
    }

    for (const auto& index : required_indexes)
    {
        std::string name{index};
        if (indexes.count(name) == 0)
            throw database_inconsistency{
                "The index `" + name +
                "` is missing, so this is not a valid OneLibrary database"};
    }

    // Exports contain a single `property` row declaring the version.
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
    const std::filesystem::path& path, const std::string& passphrase)
{
    const auto location = resolve(path);

    if (!std::filesystem::exists(location.database_path))
        throw database_not_found{util::path_to_utf8(location.database_path)};

    auto context = std::make_shared<onelibrary_context>(
        location.directory,
        util::open_encrypted_database(
            util::path_to_utf8(location.database_path), passphrase));

    // Fail here, while the caller still has the path in hand.
    verify_schema(*context);

    return context;
}

bool database_exists(const std::filesystem::path& path)
{
    const auto location = resolve(path);
    return std::filesystem::exists(location.database_path);
}

std::shared_ptr<onelibrary_context> create_context_from_scripts(
    const std::filesystem::path& directory,
    const std::filesystem::path& script_directory,
    const std::string& passphrase)
{
    const auto script_path = script_directory / "exportLibrary.db.sql";
    std::ifstream script{script_path};
    if (!script)
        throw std::runtime_error{
            "Cannot read the script `" + util::path_to_utf8(script_path) + "`"};

    const auto database_path = directory / database_relative_path;
    if (std::filesystem::exists(database_path))
        throw std::runtime_error{
            "A database already exists at `" +
            util::path_to_utf8(database_path) + "`"};

    std::filesystem::create_directories(database_path.parent_path());

    {
        auto db = util::create_encrypted_database(
            util::path_to_utf8(database_path), passphrase);

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
                    "Error in script `" + util::path_to_utf8(script_path) +
                    "` whilst executing line \"" + statement +
                    "\": " + e.what()};
            }
        }
    }

    return load_context(directory, passphrase);
}

database load_database(
    const std::filesystem::path& path, const std::string& passphrase)
{
    return database{
        std::make_shared<v1::database_impl>(load_context(path, passphrase))};
}

database create_database_from_scripts(
    const std::filesystem::path& directory,
    const std::filesystem::path& script_directory,
    const std::string& passphrase)
{
    return database{std::make_shared<v1::database_impl>(
        create_context_from_scripts(directory, script_directory, passphrase))};
}

}  // namespace djinterop::onelibrary
