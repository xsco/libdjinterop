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

#include "random.hpp"

#include <ios>
#include <random>
#include <sstream>

namespace djinterop::util
{
int64_t generate_random_int64()
{
    static std::random_device rng;
    static std::mt19937_64 generator{rng()};
    static std::uniform_int_distribution<int64_t> dist{1ll << 61, 1ll << 62};
    return dist(generator);
}

std::string generate_random_uuid()
{
    static std::random_device rng;
    static std::mt19937 generator{rng()};
    static std::uniform_int_distribution<int> nibble_dist{0, 15};
    static std::uniform_int_distribution<int> variant_nibble_dist{8, 11};

    // Generate a version 4 (random), variant 1 UUID.
    std::stringstream ss;
    ss << std::hex << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator) << "-"
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << "-4"  // Version 4 indicator
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << "-"
       << variant_nibble_dist(generator)  // Variant 1 indicator
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << "-" << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator) << nibble_dist(generator)
       << nibble_dist(generator);
    return ss.str();
}

}  // namespace djinterop::util
