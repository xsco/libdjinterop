#pragma once

#include <string>

#include <sqlite_modern_cpp.h>

namespace djinterop
{
namespace enginelibrary
{
class el_storage
{
public:
    el_storage(std::string directory);

    std::string directory;
    sqlite::database db;
};

}  // namespace enginelibrary
}  // namespace djinterop