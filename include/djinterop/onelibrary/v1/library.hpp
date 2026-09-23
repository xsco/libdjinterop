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
#ifndef DJINTEROP_ONELIBRARY_V1_LIBRARY_HPP
#define DJINTEROP_ONELIBRARY_V1_LIBRARY_HPP

#include <memory>
#include <string>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>
#include <djinterop/onelibrary/onelibrary.hpp>
#include <djinterop/onelibrary/v1/content_table.hpp>
#include <djinterop/onelibrary/v1/playlist_table.hpp>
#include <djinterop/onelibrary/v1/property_table.hpp>

namespace djinterop::onelibrary::v1
{
/// A loaded OneLibrary device, through the low-level API.
///
/// The tables expose the format as the device holds it, translating no
/// further than resolving a lookup reference to the text behind it.
///
/// Loading derives the key that decrypts the device, which is deliberately
/// expensive; take `database()` from a library rather than also calling
/// `load_database`.
class DJINTEROP_PUBLIC library
{
public:
    /// Load the database on a device, as `load_database` does.
    explicit library(
        const std::string& path,
        const std::string& passphrase = default_passphrase);

    /// The `content` table.
    [[nodiscard]] content_table content() const;

    /// The `playlist` table, and the membership beside it.
    [[nodiscard]] playlist_table playlist() const;

    /// The `property` table, which records the schema version.
    [[nodiscard]] property_table property() const;

    /// The device, through the format-agnostic interface.
    [[nodiscard]] djinterop::database database() const;

    /// Root directory of the device, to which every path is relative, and
    /// not the directory the database file itself sits in.
    [[nodiscard]] const std::string& directory() const;

private:
    std::shared_ptr<onelibrary_context> context_;
};

}  // namespace djinterop::onelibrary::v1

#endif  // DJINTEROP_ONELIBRARY_V1_LIBRARY_HPP
