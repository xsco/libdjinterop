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

#if __has_include(<optional>)

#include <optional>

// Use a consistent namespace to contain experimental features, even though in
// this compilation path they are already 'standard'.
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

#elif __has_include(<experimental/optional>)

#include <experimental/optional>

// Use a consistent namespace to contain experimental features.
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

#error "Must have either <optional> or <experimental/optional>"

#endif

