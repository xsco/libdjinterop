/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#if __cplusplus <= 199711L
  #error This library needs at least a C++11 compliant compiler
#endif

#ifndef ENGINEPRIME_TRACK_HPP
#define ENGINEPRIME_TRACK_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "database.hpp"

namespace engineprime {

struct album_art
{
    typedef uint_least8_t image_data_type;

    std::string hash_hex_str;
    std::vector<image_data_type> image_data;
};

class track
{
    track(const database &database, int id);
    ~track();

    // TODO - move to pimpl
    //int id;
    //std::shared_ptr<album_art> art;
    
    // TODO - add info from CopiedTrack
    
    int id() const;
    std::shared_ptr<album_art> art() const;
    bool has_art() const;

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

std::vector<int> track_ids(const database &database);

} // engineprime

#endif // ENGINEPRIME_TRACK_HPP
