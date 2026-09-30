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

#include <string>

#include <sqlite_modern_cpp.h>

namespace djinterop::util
{
// Exactly one implementation of this header is compiled in: `sqlcipher.cpp`
// where the build has SQLCipher, and otherwise `sqlcipher_unsupported.cpp`.

/// Open a SQLCipher database for reading.
///
/// The connection serves the decrypted contents, including anything still in
/// the write-ahead log.  Reading a write-ahead-logged database creates the
/// `-shm` and `-wal` files beside it if they are missing, so the directory
/// holding it must be writable.
///
/// \throws djinterop::unsupported_database If this build cannot open
///         encrypted databases, or the passphrase does not open the file.
[[nodiscard]] sqlite::database open_encrypted_database(
    const std::string& path, const std::string& passphrase);

}  // namespace djinterop::util
