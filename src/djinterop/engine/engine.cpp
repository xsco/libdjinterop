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

#include <cmath>
#include <fstream>
#include <string>

#include <djinterop/djinterop.hpp>

#include "../util.hpp"
#include "schema/schema.hpp"
#include "track_utils.hpp"
#include "v1/engine_database_impl.hpp"
#include "v1/engine_transaction_guard_impl.hpp"

namespace djinterop::engine
{
database create_database(
    const std::string& directory, const engine_version& version)
{
    auto storage = std::make_shared<v1::engine_storage>(directory, version);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
}

database create_temporary_database(const engine_version& version)
{
    auto storage = std::make_shared<v1::engine_storage>(version);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
}

database create_database_from_scripts(
    const std::string& db_directory, const std::string& script_directory)
{
    std::string stmt;

    std::ifstream m_db_script{script_directory + "/m.db.sql"};
    sqlite::database m_db{db_directory + "/m.db"};
    while (std::getline(m_db_script, stmt))
    {
        m_db << stmt;
    }

    std::ifstream p_db_script{script_directory + "/p.db.sql"};
    sqlite::database p_db{db_directory + "/p.db"};
    while (std::getline(p_db_script, stmt))
    {
        p_db << stmt;
    }

    return load_database(db_directory);
}

database create_or_load_database(
    const std::string& directory, const engine_version& version,
    bool& created)
{
    try
    {
        created = false;
        return load_database(directory);
    }
    catch (database_not_found& e)
    {
        created = true;
        return create_database(directory, version);
    }
}

bool database_exists(const std::string& directory)
{
    try
    {
        load_database(directory);
    }
    catch (database_not_found& e)
    {
        return false;
    }

    return true;
}

database load_database(const std::string& directory)
{
    auto storage = std::make_shared<v1::engine_storage>(directory);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
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
            "Attempted to normalize a misplaced beatgrid"};
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

int64_t required_waveform_samples_per_entry(double sample_rate)
{
    return util::required_waveform_samples_per_entry(sample_rate);
}

std::string perfdata_db_path(const database& db)
{
    return db.directory() + "/p.db";
}

}  // namespace djinterop::enginelibrary
