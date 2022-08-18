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

#include <boost/filesystem.hpp>

struct temporary_directory
{
    temporary_directory() :
        temp_dir_path{create_temp_dir()}, temp_dir{temp_dir_path.string()}
    {
    }

    ~temporary_directory() { boost::filesystem::remove_all(temp_dir); }

    boost::filesystem::path temp_dir_path;
    std::string temp_dir;

private:
    static boost::filesystem::path create_temp_dir()
    {
        boost::filesystem::path temp_dir{
            boost::filesystem::temp_directory_path()};
        temp_dir /= boost::filesystem::unique_path();
        if (!boost::filesystem::create_directory(temp_dir))
        {
            throw std::runtime_error{"Failed to create tmp_dir"};
        }
        return temp_dir;
    }
};

std::ostream& operator<<(std::ostream& os, const temporary_directory& td)
{
    os << td.temp_dir;
    return os;
}
