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
#ifndef DJINTEROP_OPTIONAL_HPP
#define DJINTEROP_OPTIONAL_HPP

#include <djinterop/config.hpp>

#ifdef DJINTEROP_STD_OPTIONAL

#include <optional>

namespace djinterop
{
namespace stdx
{
using std::bad_optional_access;
using std::in_place;
using std::in_place_t;
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;
}  // namespace stdx
}  // namespace djinterop

#elif DJINTEROP_STD_EXPERIMENTAL_OPTIONAL
#include <experimental/optional>

namespace djinterop
{
namespace stdx
{
using std::experimental::bad_optional_access;
using std::experimental::in_place;
using std::experimental::in_place_t;
using std::experimental::make_optional;
using std::experimental::nullopt;
using std::experimental::nullopt_t;
using std::experimental::optional;
}  // namespace stdx
}  // namespace djinterop

#else
#error This library requires support for optional<T>, but none was found
#endif

#endif  // DJINTEROP_OPTIONAL_HPP
