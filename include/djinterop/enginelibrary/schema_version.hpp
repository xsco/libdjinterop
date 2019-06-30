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

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#ifndef DJINTEROP_ENGINELIBRARY_SCHEMA_VERSION_HPP
#define DJINTEROP_ENGINELIBRARY_SCHEMA_VERSION_HPP

#include <ostream>

namespace djinterop
{
namespace enginelibrary
{
struct schema_version
{
    int maj;
    int min;
    int pat;
};

static constexpr schema_version version_1_6_0{1, 6, 0};
static constexpr schema_version version_1_7_1{1, 7, 1};
static constexpr schema_version version_latest = version_1_7_1;

/**
 * The `database_inconsistency` exception is thrown when the schema of a
 * database does not match the expectations suggested by its reported version
 * number.
 */
class database_inconsistency : public std::logic_error
{
public:
    explicit database_inconsistency(const std::string &what_arg) noexcept
        : logic_error{what_arg}
    {
    }
    virtual ~database_inconsistency() = default;
};

/**
 * The `unsupported_database_version` exception is thrown when a database schema
 * version is encountered that is not yet supported by this version of the
 * library.
 */
class unsupported_database_version : public std::runtime_error
{
public:
    explicit unsupported_database_version(const schema_version version) noexcept
        : runtime_error{"Unsupported database version"}, version_{version}
    {
    }
    explicit unsupported_database_version(
        const std::string &what_arg, const schema_version version) noexcept
        : runtime_error{what_arg}, version_{version}
    {
    }
    virtual ~unsupported_database_version() = default;
    const schema_version version() const { return version_; }

private:
    schema_version version_;
};

inline std::ostream &operator<<(std::ostream &os, const schema_version &version)
{
    return os << version.maj << "." << version.min << "." << version.pat;
}

inline bool operator==(const schema_version &a, const schema_version &b)
{
    return a.maj == b.maj && a.min == b.min && a.pat == b.pat;
}

inline bool operator!=(const schema_version &a, const schema_version &b)
{
    return !(a.maj == b.maj && a.min == b.min && a.pat == b.pat);
}

inline bool operator>=(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat >= b.pat;
}

inline bool operator>(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat > b.pat;
}

inline bool operator<=(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat <= b.pat;
}

inline bool operator<(const schema_version &a, const schema_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat < b.pat;
}

}  // namespace enginelibrary
}  // namespace djinterop

#endif  // DJINTEROP_ENGINELIBRARY_DATABASE_HPP
