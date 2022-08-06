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
#include <string>

#include <djinterop/engine/v2/engine_library.hpp>

#include "../util.hpp"
#include "schema/schema.hpp"
#include "track_utils.hpp"
#include "v1/engine_database_impl.hpp"
#include "v1/engine_transaction_guard_impl.hpp"

namespace djinterop::engine
{
namespace
{
inline djinterop::stdx::optional<std::string> get_column_type(
    sqlite::database& db, const std::string& table_name,
    const std::string& column_name)
{
    djinterop::stdx::optional<std::string> column_type;

    db << "PRAGMA table_info('" + table_name + "')" >>
        [&](int col_id, std::string col_name, std::string col_type,
            int nullable, std::string default_value, int part_of_pk)
    {
        if (col_name == column_name)
        {
            column_type = col_type;
        }
    };

    return column_type;
}

engine_version detect_version(const std::string& directory)
{
    if (!path_exists(directory))
    {
        throw database_not_found{directory};
    }

    // Assume that all versions of engine libraries have a database called
    // "m.db", containing a table called "Information", containing the schema
    // version in some of its columns.
    sqlite::database db{directory + "/m.db"};

    // Check that the `Information` table has been created.
    std::string sql =
        "SELECT COUNT(*) AS rows "
        "FROM sqlite_master "
        "WHERE name = 'Information'";
    int32_t table_count;
    db << sql >> table_count;
    if (table_count != 1)
    {
        throw database_inconsistency{
            "Did not find an `Information` table in the database"};
    }

    semantic_version schema_version;
    db << "SELECT schemaVersionMajor, schemaVersionMinor, "
          "schemaVersionPatch FROM Information" >>
        std::tie(schema_version.maj, schema_version.min, schema_version.pat);

    // Some schema versions have different variants, meaning that the version
    // number alone is insufficient.  Detect the variant where required.
    if (schema_version.maj == 1 && schema_version.min == 18 &&
        schema_version.pat == 0)
    {
        auto has_numeric_bools =
            get_column_type(db, "Track", "isExternalTrack") ==
            std::string{"NUMERIC"};
        return has_numeric_bools ? djinterop::engine::desktop_1_5_1
                                 : djinterop::engine::os_1_6_0;
    }

    for (auto&& candidate_version : all_versions)
    {
        if (schema_version == candidate_version.schema_version)
        {
            return candidate_version;
        }
    }

    throw unsupported_engine_database{schema_version};
}

} // anonymous namespace

database create_database(
    const std::string& directory, const engine_version& version)
{
    if (version.version.maj >= 2)
    {
        auto library = v2::engine_library::create(directory, version);
        return library.make_database();
    }

    auto storage = v1::engine_storage::create(directory, version);
    return database{std::make_shared<v1::engine_database_impl>(storage)};
}

database create_temporary_database(const engine_version& version)
{
    if (version.version.maj >= 2)
    {
        auto library = v2::engine_library::create_temporary(version);
        return library.make_database();
    }

    auto storage = v1::engine_storage::create_temporary(version);
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

    // Note: there may not be a separate performance database, if the database
    // is >= version 2.
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
    auto version = detect_version(directory);
    if (version.version.maj >= 2)
    {
        // TODO(mr-smidge) inefficiency as construction reads the version again.
        auto library = v2::engine_library{directory};
        return library.make_database();
    }

    auto storage = std::make_shared<v1::engine_storage>(directory, version);
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
