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
#ifndef DJINTEROP_ONELIBRARY_V1_PROPERTY_TABLE_HPP
#define DJINTEROP_ONELIBRARY_V1_PROPERTY_TABLE_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <djinterop/config.hpp>

namespace djinterop::onelibrary
{
struct onelibrary_context;

namespace v1
{
/// The `dbVersion` that this implementation was written against.
constexpr const char* supported_db_version = "1000";

/// The single row of the `property` table, which describes the device.
struct property_row
{
    std::optional<std::string> device_name;

    /// Version of the schema, such as `1000`.
    std::optional<std::string> db_version;

    /// Number of tracks the device was exported with, which need not be the
    /// number the `content` table now holds.
    std::optional<int64_t> number_of_contents;

    std::optional<std::string> created_date;
};

/// Read access to the `property` table.
class DJINTEROP_PUBLIC property_table
{
public:
    explicit property_table(std::shared_ptr<onelibrary_context> context);

    /// The row, or no value if the table is empty.
    [[nodiscard]] std::optional<property_row> get() const;

    [[nodiscard]] std::optional<std::string> get_db_version() const;

private:
    std::shared_ptr<onelibrary_context> context_;
};

}  // namespace v1
}  // namespace djinterop::onelibrary

#endif  // DJINTEROP_ONELIBRARY_V1_PROPERTY_TABLE_HPP
