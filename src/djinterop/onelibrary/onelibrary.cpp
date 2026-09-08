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

#include <memory>
#include <string>

#include <djinterop/exceptions.hpp>

#include "../util/crypto/encrypted_database.hpp"
#include "../util/filesystem.hpp"
#include "loader.hpp"
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
    // A path that names the database directly implies its device root, which
    // is three levels up: `<root>/PIONEER/rekordbox/exportLibrary.db`.
    if (!util::path_is_directory(path))
    {
        // Walking up by index, rather than by assigning a piece of a string
        // back to itself three times over, which is a shape GCC's -Wrestrict
        // cannot see the safety of.
        auto end = path.size();
        for (int level = 0; level < 3; ++level)
        {
            const auto separator = end == 0 ? std::string::npos
                                            : path.find_last_of("/\\", end - 1);

            // A relative path with nothing above it sits in the working
            // directory, which is then the root of the device.
            if (separator == std::string::npos)
                return resolved_location{".", path};

            end = separator;
        }

        return resolved_location{path.substr(0, end), path};
    }

    return resolved_location{path, path + "/" + database_relative_path};
}

}  // anonymous namespace

std::shared_ptr<onelibrary_context> load_context(
    const std::string& path, const std::string& passphrase)
{
    const auto location = resolve(path);

    if (!util::path_exists(location.database_path))
        throw database_not_found{location.database_path};

    // Key derivation is expensive, so the passphrase is not tested
    // separately: a wrong one shows up as the database failing to open.
    std::shared_ptr<onelibrary_context> context;
    try
    {
        context = std::make_shared<onelibrary_context>(
            location.directory, util::crypto::open_encrypted_database(
                                    location.database_path, passphrase));

        // Opening a database reads nothing, so touch it here: a wrong
        // passphrase would otherwise not be noticed until the first query.
        context->db << "SELECT COUNT(*) FROM sqlite_master" >> [](int64_t) {};
    }
    catch (const sqlite::sqlite_exception&)
    {
        throw unsupported_database{
            "The file `" + location.database_path +
            "` is not a SQLCipher database that the given passphrase opens"};
    }
    catch (const util::crypto::encrypted_database_error& e)
    {
        throw unsupported_database{e.what()};
    }
    catch (const util::crypto::encryption_unsupported& e)
    {
        throw unsupported_database{e.what()};
    }

    // Fail here, while the caller still has the path in hand.
    v1::database_impl{context}.verify();

    return context;
}

bool database_exists(const std::string& path)
{
    const auto location = resolve(path);
    return util::path_exists(location.database_path);
}

database load_database(const std::string& path, const std::string& passphrase)
{
    return database{
        std::make_shared<v1::database_impl>(load_context(path, passphrase))};
}

}  // namespace djinterop::onelibrary
