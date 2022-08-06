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

#include <djinterop/engine/v2/information_table.hpp>

#include <cassert>
#include <string>
#include <utility>

#include <djinterop/optional.hpp>

#include "engine_library_context.hpp"

namespace djinterop::engine::v2
{

information_table::information_table(
    std::shared_ptr<engine_library_context> context) :
    context_{std::move(context)}
{
}

information_row information_table::get() const
{
    djinterop::stdx::optional<information_row> result;
    context_->db << "SELECT id, uuid, schemaVersionMajor, schemaVersionMinor, "
                    "schemaVersionPatch, currentPlayedIndiciator, "
                    "lastRekordBoxLibraryImportReadCounter FROM Information" >>
        [&](int64_t id, std::string uuid, int64_t maj, int64_t min, int64_t pat,
            int64_t current_played_indicator,
            int64_t last_rekordbox_library_import_read_counter)
    {
        assert(!result);  // Failure means >1 row in the Information table.

        result = information_row{
            id,
            std::move(uuid),
            maj,
            min,
            pat,
            current_played_indicator,
            last_rekordbox_library_import_read_counter};
    };

    assert(result);  // Failure means no rows in the Information table.
    return *result;
}

void information_table::update_current_played_indicator(
    int64_t played_indicator)
{
    // Note: the current played "indicator" column is misspelled in the schema.
    context_->db << "UPDATE Information SET currentPlayedIndiciator = ?"
                 << played_indicator;
}

}  // namespace djinterop::engine::v2
