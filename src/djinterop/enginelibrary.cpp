#include <sys/stat.h>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#endif

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/schema.hpp>

namespace djinterop
{
namespace enginelibrary
{
database load_database(std::string directory)
{
    return database{std::make_shared<el_database_impl>(std::move(directory))};
}

database make_database(
    std::string directory, const semantic_version& default_version)
{
    if (!is_supported(default_version))
    {
        throw unsupported_database_version{"Unsupported database version",
                                           default_version};
    }

    auto music_db_path = directory + "/m.db";
    auto perfdata_db_path = directory + "/p.db";

    struct stat buf;
    bool music_db_exists = stat(music_db_path.c_str(), &buf) == 0;
    bool perfdata_db_exists = stat(perfdata_db_path.c_str(), &buf) == 0;

    switch (music_db_exists + perfdata_db_exists)
    {
        case 1:
            throw std::runtime_error{"Only one of m.db and p.db exist"};
            break;
        case 0:
        {
            // Ensure the target directory exists
            if (stat(directory.c_str(), &buf) != 0)
            {
                // Create the dir
#if defined(_WIN32)
                if (_mkdir(directory.c_str()) != 0)
#else
                if (mkdir(directory.c_str(), 0755) != 0)
#endif
                {
                    throw std::runtime_error{
                        "Failed to create directory to hold new database"};
                }
            }
            sqlite::database m_db{music_db_path};
            create_music_schema(m_db, default_version);
            verify_music_schema(m_db);
            sqlite::database p_db{perfdata_db_path};
            create_performance_schema(p_db, default_version);
            verify_performance_schema(p_db);
            break;
        }
        default:  // both exist, so we do nothing
            break;
    }

    return load_database(std::move(directory));
}

}  // namespace enginelibrary
}  // namespace djinterop
