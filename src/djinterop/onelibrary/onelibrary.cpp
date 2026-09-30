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

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <sqlite_modern_cpp.h>

#include "onelibrary_context.hpp"
#include "schema.hpp"

namespace djinterop::onelibrary
{

namespace
{
namespace fs = std::filesystem;

constexpr const char* k_db_rel_path = ".PIONEER/rekordbox/exportLibrary.db";

sqlite::database open_db(const std::string& path, bool create)
{
    if (create)
    {
        // Ensure parent directories exist.
        fs::create_directories(fs::path{path}.parent_path());
    }

    sqlite::database db{path};

    // Set key via PRAGMA.
    db << "PRAGMA key = '" + std::string{default_key} + "';";
    db << "PRAGMA cipher_compatibility = 4;";

    if (create)
    {
        // Exports use write-ahead logging.
        db << "PRAGMA journal_mode = WAL;";
    }

    return db;
}

void hydrate_database(
    const std::string& db_path, const std::string& script_path)
{
    if (!fs::exists(script_path))
    {
        throw std::runtime_error{"Script file not found at " + script_path};
    }

    auto db = open_db(db_path, /*create=*/true);

    std::ifstream script{script_path};
    std::string stmt;
    while (std::getline(script, stmt))
    {
        try
        {
            db << stmt;
        }
        catch (std::exception& e)
        {
            throw std::runtime_error{
                "Error in script " + script_path + " whilst executing line \"" +
                stmt + "\": " + e.what()};
        }
    }

    db << "PRAGMA wal_checkpoint(TRUNCATE);";
}
}  // anonymous namespace

onelibrary onelibrary::create(const std::string& directory)
{
    if (exists(directory))
    {
        throw std::runtime_error{
            "OneLibrary database already exists at " + directory};
    }

    auto db_path = fs::path{directory} / k_db_rel_path;
    auto db = open_db(db_path.string(), /*create=*/true);

    // Create schema on a brand-new encrypted database.
    djinterop::onelibrary::create(db.connection().get());
    djinterop::onelibrary::seed(db.connection().get());

    // Checkpoint the WAL.
    db << "PRAGMA wal_checkpoint(TRUNCATE);";

    auto ctx = std::make_shared<onelibrary_context>(directory, std::move(db));
    return onelibrary{ctx};
}

onelibrary onelibrary::create_from_scripts(
    const std::string& directory, const std::string& script_directory)
{
    auto db_path = fs::path{directory} / k_db_rel_path;
    auto script_path = fs::path{script_directory} / "exportLibrary.db.sql";

    hydrate_database(db_path.string(), script_path.string());

    return load(directory);
}

onelibrary onelibrary::load(const std::string& directory)
{
    if (!exists(directory))
    {
        throw std::runtime_error{
            "OneLibrary database not found at " + directory};
    }

    auto db_path = fs::path{directory} / k_db_rel_path;
    auto db = open_db(db_path.string(), /*create=*/false);

    // Verify we can read it.
    int count = 0;
    db << "SELECT count(*) FROM sqlite_master;" >> count;
    if (count == 0)
    {
        throw std::runtime_error{
            "Failed to decrypt or read OneLibrary database at " +
            db_path.string()};
    }

    auto ctx = std::make_shared<onelibrary_context>(directory, std::move(db));
    return onelibrary{ctx};
}

bool onelibrary::exists(const std::string& directory)
{
    return fs::exists(fs::path{directory} / k_db_rel_path);
}

onelibrary::onelibrary(std::shared_ptr<onelibrary_context> context) :
    context_{std::move(context)}
{
}

onelibrary::~onelibrary() = default;

std::string onelibrary::directory() const
{
    return context_->directory;
}

void onelibrary::verify() const
{
    std::string result;
    context_->db << "PRAGMA integrity_check;" >> result;
    if (result != "ok")
    {
        throw std::runtime_error{"Database integrity check failed: " + result};
    }

    djinterop::onelibrary::verify(context_->db.connection().get());
}

}  // namespace djinterop::onelibrary
