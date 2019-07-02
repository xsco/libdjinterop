#pragma once

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <iomanip>

namespace djinterop
{
struct semantic_version
{
    int maj;
    int min;
    int pat;
};

inline std::ostream &operator<<(
    std::ostream &os, const semantic_version &version)
{
    return os << version.maj << "." << version.min << "." << version.pat;
}

inline bool operator==(const semantic_version &a, const semantic_version &b)
{
    return a.maj == b.maj && a.min == b.min && a.pat == b.pat;
}

inline bool operator!=(const semantic_version &a, const semantic_version &b)
{
    return !(a.maj == b.maj && a.min == b.min && a.pat == b.pat);
}

inline bool operator>=(const semantic_version &a, const semantic_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat >= b.pat;
}

inline bool operator>(const semantic_version &a, const semantic_version &b)
{
    if (a.maj != b.maj)
        return a.maj > b.maj;
    if (a.min != b.min)
        return a.min > b.min;
    return a.pat > b.pat;
}

inline bool operator<=(const semantic_version &a, const semantic_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat <= b.pat;
}

inline bool operator<(const semantic_version &a, const semantic_version &b)
{
    if (a.maj != b.maj)
        return a.maj < b.maj;
    if (a.min != b.min)
        return a.min < b.min;
    return a.pat < b.pat;
}

}  // namespace djinterop
