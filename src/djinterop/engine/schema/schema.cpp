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

namespace djinterop::engine::schema
{
std::unique_ptr<schema_creator_validator> make_schema_creator_validator(
    const engine_version& version)
{
    if (version == os_1_0_0)
        return std::make_unique<schema_1_6_0>();
    else if (version == os_1_0_3)
        return std::make_unique<schema_1_7_1>();
    else if (version == desktop_1_1_1)
        return std::make_unique<schema_1_9_1>();
    else if (version == os_1_2_0)
        return std::make_unique<schema_1_11_1>();
    else if (version == os_1_2_2)
        return std::make_unique<schema_1_13_0>();
    else if (version == desktop_1_2_2)
        return std::make_unique<schema_1_13_1>();
    else if (version == os_1_3_1)
        return std::make_unique<schema_1_13_2>();
    else if (version == os_1_4_0)
        return std::make_unique<schema_1_15_0>();
    else if (version == os_1_5_1)
        return std::make_unique<schema_1_17_0>();
    else if (version == desktop_1_5_1)
        return std::make_unique<schema_1_18_0_desktop>();
    else if (version == os_1_6_0)
        return std::make_unique<schema_1_18_0_os>();
    else if (version == os_2_0_0)
        return std::make_unique<schema_2_18_0>();
    else if (version == desktop_2_0_0)
        return std::make_unique<schema_2_18_0>();
    else if (version == desktop_2_2_0)
        return std::make_unique<schema_2_20_1>();
    else if (version == os_2_2_0)
        return std::make_unique<schema_2_20_1>();
    else if (version == desktop_2_4_0)
        return std::make_unique<schema_2_20_2>();
    else if (version == os_2_4_0)
        return std::make_unique<schema_2_20_2>();
    else if (version == desktop_3_0_0)
        return std::make_unique<schema_2_20_3>();
    else if (version == os_3_0_0)
        return std::make_unique<schema_2_20_3>();
    else if (version == desktop_3_1_0)
        return std::make_unique<schema_2_21_0>();

    throw unsupported_engine_database{version.schema_version};
}

}  // namespace djinterop::engine::schema
