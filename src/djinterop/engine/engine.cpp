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

#include <djinterop/engine/engine.hpp>

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>

#include <djinterop/engine/v2/engine_library.hpp>
#include <djinterop/engine/v3/engine_library.hpp>

#include "../util/filesystem.hpp"
#include "engine_library_context.hpp"
#include "engine_library_dir_utils.hpp"
#include "track_utils.hpp"
#include "v1/engine_database_impl.hpp"

namespace djinterop::engine
{
namespace
{
void hydrate_database(
    const std::string& db_path, const std::string& script_path)
{
    std::string stmt;

    std::ifstream script{script_path};
    sqlite::database m_db{db_path};
    while (std::getline(script, stmt))
    {
        m_db << stmt;
    }
}
}  // anonymous namespace

database create_database(
    const std::string& directory, const engine_schema& schema)
{
    if (schema >= engine_schema::schema_3_0_0)
    {
        auto library = v3::engine_library::create(directory, schema);
        return library.database();
    }

    if (schema >= engine_schema::schema_2_18_0)
    {
        auto library = v2::engine_library::create(directory, schema);
        return library.database();
    }

    auto storage = v1::engine_storage::create(directory, schema);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
}

database create_temporary_database(const engine_schema& schema)
{
    if (schema >= engine_schema::schema_3_0_0)
    {
        auto library = v3::engine_library::create_temporary(schema);
        return library.database();
    }

    if (schema >= engine_schema::schema_2_18_0)
    {
        auto library = v2::engine_library::create_temporary(schema);
        return library.database();
    }

    auto storage = v1::engine_storage::create_temporary(schema);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
}

database create_database_from_scripts(
    const std::string& db_directory, const std::string& script_directory,
    engine_schema& loaded_schema)
{
    if (!djinterop::util::path_exists(db_directory))
    {
        throw std::runtime_error{"DB directory does not exist"};
    }

    if (!djinterop::util::path_exists(script_directory))
    {
        throw std::runtime_error{"Script directory does not exist"};
    }

    auto v1_m_db_path = db_directory + "/m.db";
    auto v1_m_db_sql_path = script_directory + "/m.db.sql";
    auto v1_p_db_path = db_directory + "/p.db";
    auto v1_p_db_sql_path = script_directory + "/p.db.sql";
    auto database2_db_dir = db_directory + "/Database2";
    auto database2_script_dir = script_directory + "/Database2";
    auto v2_m_db_path = database2_db_dir + "/m.db";
    auto v2_m_db_sql_path = database2_script_dir + "/m.db.sql";

    if (djinterop::util::path_exists(v1_m_db_sql_path))
    {
        hydrate_database(v1_m_db_path, v1_m_db_sql_path);
    }

    if (djinterop::util::path_exists(v1_p_db_sql_path))
    {
        hydrate_database(v1_p_db_path, v1_p_db_sql_path);
    }

    if (djinterop::util::path_exists(v2_m_db_sql_path))
    {
        if (!djinterop::util::path_exists(database2_db_dir))
            djinterop::util::create_dir(database2_db_dir);

        hydrate_database(v2_m_db_path, v2_m_db_sql_path);
    }

    return load_database(db_directory, loaded_schema);
}

database create_or_load_database(
    const std::string& directory, const engine_schema& schema, bool& created,
    engine_schema& loaded_schema)
{
    try
    {
        created = false;
        return load_database(directory, loaded_schema);
    }
    catch ([[maybe_unused]] database_not_found& e)
    {
        created = true;
        return create_database(directory, schema);
    }
}

bool database_exists(const std::string& directory)
{
    try
    {
        load_database(directory);
    }
    catch ([[maybe_unused]] database_not_found& e)
    {
        return false;
    }

    return true;
}

database load_database(
    const std::string& directory, engine_schema& loaded_schema)
{
    const auto is_database2 = detect_is_database2(directory);

    if (!is_database2)
    {
        // Legacy-type directory structure is always schema 1.x.
        auto storage = std::make_shared<v1::engine_storage>(directory);
        loaded_schema = storage->schema;
        return database{std::make_shared<v1::engine_database_impl>(storage)};
    }

    // A Database2-type directory structure could be schema 2.x or higher.
    auto db = load_database2_sqlite_database(directory);
    const auto detected_schema = schema::detect_schema(db);
    auto context = std::make_shared<engine_library_context>(
        directory, true, detected_schema, db);

    if (detected_schema >= engine_schema::schema_3_0_0)
    {
        auto library = v3::engine_library{context};
        return library.database();
    }

    if (detected_schema >= engine_schema::schema_2_18_0)
    {
        auto library = v2::engine_library{context};
        return library.database();
    }

    throw database_inconsistency{
        "Found a Database2-type Engine Library with schema " +
        to_string(detected_schema) + ", which is not supported"};
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
            [sample_count](const beatgrid_marker& marker)
            { return marker.sample_offset > sample_count; });
        if (last_marker_iter != beatgrid.end())
        {
            beatgrid.erase(last_marker_iter + 1, beatgrid.end());
        }
    }

    {
        auto after_first_marker_iter = std::find_if(
            beatgrid.begin(), beatgrid.end(), [](const beatgrid_marker& marker)
            { return marker.sample_offset > 0; });
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

waveform_extents calculate_high_resolution_waveform_extents(
    unsigned long long sample_count, double sample_rate)
{
    return util::calculate_high_resolution_waveform_extents(
        sample_count, sample_rate);
}

waveform_extents calculate_overview_waveform_extents(
    unsigned long long sample_count, double sample_rate)
{
    return util::calculate_overview_waveform_extents(sample_count, sample_rate);
}

}  // namespace djinterop::engine
