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
          music_db_path{directory + "/m.db"},
          perfdata_db_path{directory + "/p.db"},
          music_db{music_db_path},
          perfdata_db{perfdata_db_path}
    {
        // TODO (haslersn): Should we check that directory is an absolute path?
    }

    std::string directory;
    std::string music_db_path;
    std::string perfdata_db_path;
    sqlite::database music_db;
    sqlite::database perfdata_db;
};

}  // namespace enginelibrary
}  // namespace djinterop