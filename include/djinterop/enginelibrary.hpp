#pragma once

#if __cplusplus < 201103L && _MSVC_LANG < 201103L
#error This library needs at least a C++11 compliant compiler
#endif

#include <djinterop/semantic_version.hpp>

namespace djinterop
{
class database;

namespace enginelibrary
{
static constexpr semantic_version version_1_6_0{1, 6, 0};
static constexpr semantic_version version_1_7_1{1, 7, 1};
static constexpr semantic_version version_latest = version_1_7_1;

database load_database(std::string directory);

/// Creates a new, empty database in a given directory and using a specified
/// schema version
database make_database(
    std::string directory,
    const semantic_version& default_version = version_latest);

}  // namespace enginelibrary
}  // namespace djinterop
