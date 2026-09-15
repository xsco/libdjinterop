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

#include <djinterop/onelibrary/v1/property_table.hpp>

#include <utility>

#include "../onelibrary_context.hpp"
namespace djinterop::onelibrary::v1
{
property_table::property_table(std::shared_ptr<onelibrary_context> context) :
    context_{std::move(context)}
{
}

std::optional<property_row> property_table::get() const
{
    std::optional<property_row> result;

    context_->db << "SELECT deviceName, dbVersion, numberOfContents, "
                    "createdDate FROM property LIMIT 1" >>
        [&](std::optional<std::string> device_name,
            std::optional<std::string> db_version,
            std::optional<int64_t> number_of_contents,
            std::optional<std::string> created_date)
    {
        property_row row;
        row.device_name = std::move(device_name);
        row.db_version = std::move(db_version);
        row.number_of_contents = number_of_contents;
        row.created_date = std::move(created_date);
        result = std::move(row);
    };

    return result;
}

std::optional<std::string> property_table::get_db_version() const
{
    std::optional<std::string> result;
    context_->db << "SELECT dbVersion FROM property LIMIT 1" >>
        [&](std::optional<std::string> version)
    { result = std::move(version); };

    return result;
}

}  // namespace djinterop::onelibrary::v1
