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
#ifndef DJINTEROP_ONELIBRARY_ONELIBRARY_HPP
#define DJINTEROP_ONELIBRARY_ONELIBRARY_HPP

#include <string>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>

/// Support for the AlphaTheta OneLibrary device format, also documented as
/// Device Library Plus.
///
/// OneLibrary succeeds the DeviceSQL `export.pdb` library that rekordbox wrote
/// to USB media; a device usually carries both, and a player that understands
/// OneLibrary prefers it.  The library is one SQLite database encrypted with
/// SQLCipher 4, so reading one needs libdjinterop built with
/// `EXPERIMENTAL_ENABLE_SQLCIPHER`; without it, loading throws
/// `djinterop::unsupported_database`.
///
/// Support here is currently **read-only**.  Everything that changes a
/// database throws `djinterop::unsupported_operation`.
///
/// This header is the high-level API, which presents a device through the
/// format-agnostic `database` interface.  The low-level API, which exposes
/// the tables of the format as they are, is in `onelibrary/v1`.
namespace djinterop::onelibrary
{
/// Location of the library database within a device, relative to its root.
constexpr const char* database_relative_path =
    "PIONEER/rekordbox/exportLibrary.db";

/// The passphrase with which rekordbox encrypts every `exportLibrary.db`.
///
/// It is the same on every installation, and depends on neither licence nor
/// machine, so any player can read any device.  Pass a different one to
/// `load_database` if a future release of rekordbox changes it.
constexpr const char* default_passphrase =
    "r8gddnr4k847830ar6cqzbkk0el6qytmb3trbbx805jm74vez64i5o8fnrqryqls";

/// Test whether a OneLibrary database exists in a given location.
///
/// \param path Either the root directory of a device, or the database file.
/// \return Returns true if a database is present.
bool DJINTEROP_PUBLIC database_exists(const std::string& path);

/// Load a OneLibrary database.
///
/// \param path Either the root directory of a device, such as the mount point
///             of a USB drive, or the `exportLibrary.db` file itself.
/// \param passphrase Passphrase with which the database is encrypted.
/// \return Returns the loaded database.
/// \throws database_not_found If no database is present at the given path.
/// \throws unsupported_database If the passphrase does not open the database,
///                              or this build of libdjinterop cannot read one.
/// \throws database_inconsistency If the database does not hold the tables
///                                that a OneLibrary database is expected to.
database DJINTEROP_PUBLIC load_database(
    const std::string& path,
    const std::string& passphrase = default_passphrase);

}  // namespace djinterop::onelibrary

#endif  // DJINTEROP_ONELIBRARY_ONELIBRARY_HPP
