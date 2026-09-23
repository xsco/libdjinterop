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

#include <djinterop/onelibrary/v1/library.hpp>

#include "../onelibrary_context.hpp"
#include "database_impl.hpp"
namespace djinterop::onelibrary::v1
{
library::library(const std::string& path, const std::string& passphrase) :
    context_{load_context(path, passphrase)}
{
}

content_table library::content() const
{
    return content_table{context_};
}

playlist_table library::playlist() const
{
    return playlist_table{context_};
}

property_table library::property() const
{
    return property_table{context_};
}

djinterop::database library::database() const
{
    return djinterop::database{std::make_shared<database_impl>(context_)};
}

const std::string& library::directory() const
{
    return context_->directory;
}

}  // namespace djinterop::onelibrary::v1
