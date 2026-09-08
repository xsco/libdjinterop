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

#include <stdexcept>
#include <string>

#include <sqlite_modern_cpp.h>

namespace djinterop::util::crypto
{
// Exactly one implementation of this header is compiled in.  The one beside
// it decrypts the pages itself; a SQLCipher-backed one would open the file
// directly and replace that source file.

/// Thrown when a file is not an encrypted database the passphrase opens.
class encrypted_database_error : public std::runtime_error
{
public:
    explicit encrypted_database_error(const std::string& what) :
        std::runtime_error{what}
    {
    }
};

/// Thrown when this build cannot open encrypted databases at all.
class encryption_unsupported : public std::runtime_error
{
public:
    explicit encryption_unsupported(const std::string& what) :
        std::runtime_error{what}
    {
    }
};

/// Test whether this build can open encrypted databases.
[[nodiscard]] bool encrypted_databases_supported() noexcept;

/// Open an encrypted database for reading.
///
/// The connection serves the decrypted contents, with any write-ahead log
/// already folded in.  It is not a handle on the file.
///
/// \throws encryption_unsupported If this build cannot open encrypted
///         databases.
/// \throws encrypted_database_error If the passphrase does not open the file.
[[nodiscard]] sqlite::database open_encrypted_database(
    const std::string& path, const std::string& passphrase);

}  // namespace djinterop::util::crypto
