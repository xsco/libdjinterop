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

#include <sys/stat.h>
#include <cmath>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#endif

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_transaction_guard_impl.hpp>
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

    // Ensure the target directory exists
    struct stat buf;
    if (stat(directory.c_str(), &buf) != 0)
    {
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

    // We have to find out whether the engine library exists. Naively, we'd do
    // this by checking if the m.db and p.db files exist. However, if a previous
    // attempt to create the engine library failed after creating the files and
    // before creating the schemata, then the files exist but the enginelibrary
    // doesn't exist.
    //
    // Therefore, we create the two files (if not already present) and then
    // check if they contain zero tables. If so, then we create the schemata.

    auto storage = std::make_shared<el_storage>(std::move(directory));
    el_transaction_guard_impl trans{storage};
    int32_t table_count;
    storage->db << "SELECT SUM(rows) FROM ("
                   "SELECT COUNT(*) AS rows FROM music.sqlite_master WHERE "
                   "type = 'table' UNION ALL SELECT COUNT(*) AS rows FROM "
                   "perfdata.sqlite_master WHERE type = 'table'"
                   ")" >>
        table_count;
    if (table_count == 0)
    {
        // If they contain zero tables, then we create the schemata.
        create_music_schema(storage->db, default_version);
        verify_music_schema(storage->db);
        create_performance_schema(storage->db, default_version);
        verify_performance_schema(storage->db);
    }
    database db{std::make_shared<el_database_impl>(storage)};
    trans.commit();
    return db;
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
