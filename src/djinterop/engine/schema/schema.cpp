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

#include <optional>
#include <string>

#include <djinterop/exceptions.hpp>

#include "schema.hpp"
#include "schema_1_11_1.hpp"
#include "schema_1_13_0.hpp"
#include "schema_1_13_1.hpp"
#include "schema_1_13_2.hpp"
#include "schema_1_15_0.hpp"
#include "schema_1_17_0.hpp"
#include "schema_1_18_0_desktop.hpp"
#include "schema_1_18_0_os.hpp"
#include "schema_1_6_0.hpp"
#include "schema_1_7_1.hpp"
#include "schema_1_9_1.hpp"
#include "schema_2_18_0.hpp"
#include "schema_2_20_1.hpp"
#include "schema_2_20_2.hpp"
#include "schema_2_20_3.hpp"
#include "schema_2_21_0.hpp"
#include "schema_2_21_1.hpp"
#include "schema_2_21_2.hpp"
#include "schema_3_0_0.hpp"
#include "schema_3_0_1.hpp"

namespace djinterop::engine::schema
{
namespace
{
std::optional<std::string> get_column_type(
    sqlite::database& db, const std::string& table_name,
    const std::string& column_name)
{
    std::optional<std::string> column_type;

    db << "PRAGMA table_info('" + table_name + "')" >>
        [&]([[maybe_unused]] int col_id, const std::string& col_name,
            const std::string& col_type, [[maybe_unused]] int nullable,
            [[maybe_unused]] const std::string& default_value,
            [[maybe_unused]] int part_of_pk)
    {
        if (col_name == column_name)
        {
            column_type = col_type;
        }
    };

    return column_type;
}
}  // namespace

std::unique_ptr<schema_creator_validator> make_schema_creator_validator(
    const engine_schema& schema)
{
    switch (schema)
    {
        case engine_schema::schema_1_6_0:
            return std::make_unique<schema_1_6_0>();
        case engine_schema::schema_1_7_1:
            return std::make_unique<schema_1_7_1>();
        case engine_schema::schema_1_9_1:
            return std::make_unique<schema_1_9_1>();
        case engine_schema::schema_1_11_1:
            return std::make_unique<schema_1_11_1>();
        case engine_schema::schema_1_13_0:
            return std::make_unique<schema_1_13_0>();
        case engine_schema::schema_1_13_1:
            return std::make_unique<schema_1_13_1>();
        case engine_schema::schema_1_13_2:
            return std::make_unique<schema_1_13_2>();
        case engine_schema::schema_1_15_0:
            return std::make_unique<schema_1_15_0>();
        case engine_schema::schema_1_17_0:
            return std::make_unique<schema_1_17_0>();
        case engine_schema::schema_1_18_0_desktop:
            return std::make_unique<schema_1_18_0_desktop>();
        case engine_schema::schema_1_18_0_os:
            return std::make_unique<schema_1_18_0_os>();
        case engine_schema::schema_2_18_0:
            return std::make_unique<schema_2_18_0>();
        case engine_schema::schema_2_20_1:
            return std::make_unique<schema_2_20_1>();
        case engine_schema::schema_2_20_2:
            return std::make_unique<schema_2_20_2>();
        case engine_schema::schema_2_20_3:
            return std::make_unique<schema_2_20_3>();
        case engine_schema::schema_2_21_0:
            return std::make_unique<schema_2_21_0>();
        case engine_schema::schema_2_21_1:
            return std::make_unique<schema_2_21_1>();
        case engine_schema::schema_2_21_2:
            return std::make_unique<schema_2_21_2>();
        case engine_schema::schema_3_0_0:
            return std::make_unique<schema_3_0_0>();
        case engine_schema::schema_3_0_1:
            return std::make_unique<schema_3_0_1>();
    }

    throw unsupported_operation{
        "Engine schema not implemented for version " + to_string(schema)};
}

engine_schema detect_schema(
    sqlite::database& db, const std::string& db_schema_name)
{
    const auto master_table = db_schema_name.empty()
                                  ? "sqlite_master"
                                  : db_schema_name + ".sqlite_master";
    const auto information_table = db_schema_name.empty()
                                       ? "Information"
                                       : db_schema_name + ".Information";

    // Check that the `Information` table has been created.
    std::string sql = "SELECT COUNT(*) AS rows FROM " + master_table +
                      " WHERE name = 'Information'";
    int32_t table_count;
    db << sql >> table_count;

    if (table_count != 1)
    {
        throw database_inconsistency{
            "Did not find an `" + information_table +
            "` table in the music database"};
    }

    semantic_version version{};
    db << "SELECT schemaVersionMajor, schemaVersionMinor, "
          "schemaVersionPatch FROM " +
                information_table >>
        std::tie(version.maj, version.min, version.pat);

    auto make_err_message = [](const semantic_version& v)
    { return "Unsupported schema: " + to_string(v); };

#define REQUIRE_PATCH_VERSION(v, expected)               \
    if (v.pat != expected)                               \
    {                                                    \
        throw unsupported_database{make_err_message(v)}; \
    }

    switch (version.maj)
    {
        case 1:
            switch (version.min)
            {
                case 6:
                    REQUIRE_PATCH_VERSION(version, 0);
                    return engine_schema::schema_1_6_0;
                case 7:
                    REQUIRE_PATCH_VERSION(version, 1);
                    return engine_schema::schema_1_7_1;
                case 9:
                    REQUIRE_PATCH_VERSION(version, 1);
                    return engine_schema::schema_1_9_1;
                case 11:
                    REQUIRE_PATCH_VERSION(version, 1);
                    return engine_schema::schema_1_11_1;
                case 13:
                    switch (version.pat)
                    {
                        case 0: return engine_schema::schema_1_13_0;
                        case 1: return engine_schema::schema_1_13_1;
                        case 2: return engine_schema::schema_1_13_2;
                        default:
                            throw unsupported_database{
                                make_err_message(version)};
                    }
                case 15:
                    REQUIRE_PATCH_VERSION(version, 0);
                    return engine_schema::schema_1_15_0;
                case 17:
                    REQUIRE_PATCH_VERSION(version, 0);
                    return engine_schema::schema_1_17_0;
                case 18:
                {
                    REQUIRE_PATCH_VERSION(version, 0);
                    // Schema version 1.18.0 has different variants, meaning
                    // that the version number alone is insufficient.  Detect
                    // the variant by looking at other parts of the schema.
                    const auto has_numeric_bools =
                        get_column_type(db, "Track", "isExternalTrack") ==
                        std::string{"NUMERIC"};
                    return has_numeric_bools
                               ? engine_schema::schema_1_18_0_desktop
                               : engine_schema::schema_1_18_0_os;
                }
                default: throw unsupported_database{make_err_message(version)};
            }
        case 2:
            switch (version.min)
            {
                case 18:
                    REQUIRE_PATCH_VERSION(version, 0);
                    return engine_schema::schema_2_18_0;
                case 20:
                    switch (version.pat)
                    {
                        case 1: return engine_schema::schema_2_20_1;
                        case 2: return engine_schema::schema_2_20_2;
                        case 3: return engine_schema::schema_2_20_3;
                        default:
                            throw unsupported_database{
                                make_err_message(version)};
                    }
                case 21:
                    switch (version.pat)
                    {
                        case 0: return engine_schema::schema_2_21_0;
                        case 1: return engine_schema::schema_2_21_1;
                        case 2: return engine_schema::schema_2_21_2;
                        default:
                            throw unsupported_database{
                                make_err_message(version)};
                    }
                default: throw unsupported_database{make_err_message(version)};
            }
        case 3:
            switch (version.min)
            {
                case 0: 
                    switch (version.pat)
                    {
                        case 0: return engine_schema::schema_3_0_0;
                        case 1: return engine_schema::schema_3_0_1;
                    }
                default: throw unsupported_database{make_err_message(version)};
            }
        default: throw unsupported_database{make_err_message(version)};
    }

#undef REQUIRE_PATCH_VERSION
}

}  // namespace djinterop::engine::schema
