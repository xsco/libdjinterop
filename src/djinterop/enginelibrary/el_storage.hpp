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
    el_storage(std::string directory)
        : directory{directory},
          music_db{directory + "/m.db"},
          perfdata_db{directory + "/p.db"}
    {
        // TODO (haslersn): Should we check that directory is an absolute path?
    }

    std::string directory;
    sqlite::database music_db;
    sqlite::database perfdata_db;
};

}  // namespace enginelibrary
}  // namespace djinterop