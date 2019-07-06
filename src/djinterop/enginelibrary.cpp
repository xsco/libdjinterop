#include <sys/stat.h>
#include <cmath>
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

std::string music_db_path(const database& db)
{
    return db.directory() + "/m.db";
}

std::vector<beatgrid_marker> normalize_beatgrid(
    std::vector<beatgrid_marker> beatgrid, int64_t sample_count)
{
    if (beatgrid.empty())
    {
        return beatgrid;  // Named RVO
    }

    {
        auto last_marker_iter = std::find_if(
            beatgrid.begin(), beatgrid.end(),
            [sample_count](const beatgrid_marker& marker) {
                return marker.sample_offset > sample_count;
            });
        if (last_marker_iter != beatgrid.end())
        {
            beatgrid.erase(last_marker_iter + 1, beatgrid.end());
        }
    }

    {
        auto after_first_marker_iter = std::find_if(
            beatgrid.begin(), beatgrid.end(),
            [](const beatgrid_marker& marker) {
                return marker.sample_offset > 0;
            });
        if (after_first_marker_iter != beatgrid.begin())
        {
            beatgrid.erase(beatgrid.begin(), after_first_marker_iter - 1);
        }
    }

    if (beatgrid.size() < 2)
    {
        throw std::invalid_argument{
            "Attempted to normalize a misplaced beadgrid"};
    }

    {
        double samples_per_beat =
            (beatgrid[1].sample_offset - beatgrid[0].sample_offset) /
            (beatgrid[1].index - beatgrid[0].index);
        beatgrid[0].sample_offset -= (4 + beatgrid[0].index) * samples_per_beat;
        beatgrid[0].index = -4;
    }

    {
        int32_t last = static_cast<int32_t>(beatgrid.size() - 1);
        double samples_per_beat =
            (beatgrid[last].sample_offset - beatgrid[last - 1].sample_offset) /
            (beatgrid[last].index - beatgrid[last - 1].index);
        int32_t index_adjustment = static_cast<int32_t>(std::ceil(
            (sample_count - beatgrid[last].sample_offset) / samples_per_beat));
        beatgrid[last].sample_offset += index_adjustment * samples_per_beat;
        beatgrid[last].index += index_adjustment;
    }

    return beatgrid;  // Named RVO
}

std::string perfdata_db_path(const database& db)
{
    return db.directory() + "/p.db";
}

}  // namespace enginelibrary
}  // namespace djinterop
