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

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_crate_impl.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/enginelibrary/el_track_impl.hpp>
#include <djinterop/enginelibrary/el_transaction_guard_impl.hpp>
#include <djinterop/enginelibrary/schema.hpp>
#include <djinterop/impl/util.hpp>
#include <djinterop/transaction_guard.hpp>

namespace djinterop
{
namespace enginelibrary
{
using djinterop::crate;
using djinterop::track;

el_database_impl::el_database_impl(std::string directory)
    : storage_{std::make_shared<el_storage>(std::move(directory))}
{
    // TODO (haslersn): On construction, should we check that the database
    // version is supported? This would give more guarantees to a user that
    // obtains a database object.
}

el_database_impl::el_database_impl(std::shared_ptr<el_storage> storage)
    : storage_{std::move(storage)}
{
}

transaction_guard el_database_impl::begin_transaction()
{
    return transaction_guard{
        std::make_unique<el_transaction_guard_impl>(storage_)};
}

boost::optional<crate> el_database_impl::crate_by_id(int64_t id)
{
    boost::optional<crate> cr;
    storage_->db << "SELECT COUNT(*) FROM Crate WHERE id = ?" << id >>
        [&](int64_t count) {
            if (count == 1)
            {
                cr = crate{std::make_shared<el_crate_impl>(storage_, id)};
            }
            else if (count > 1)
            {
                throw crate_database_inconsistency{
                    "More than one crate with the same ID", id};
            }
        };
    return cr;
}

std::vector<crate> el_database_impl::crates()
{
    std::vector<crate> results;
    storage_->db << "SELECT id FROM Crate ORDER BY id" >> [&](int64_t id) {
        results.push_back(crate{std::make_shared<el_crate_impl>(storage_, id)});
    };
    return results;
}

std::vector<crate> el_database_impl::crates_by_name(boost::string_view name)
{
    std::vector<crate> results;
    storage_->db << "SELECT id FROM Crate WHERE title = ? ORDER BY id"
                 << name.data() >>
        [&](int64_t id) {
            results.push_back(
                crate{std::make_shared<el_crate_impl>(storage_, id)});
        };
    return results;
}

crate el_database_impl::create_crate(boost::string_view name)
{
    el_transaction_guard_impl trans{storage_};

    storage_->db << "INSERT INTO Crate (title, path) VALUES (?, ?)"
                 << name.data() << std::string{name} + ';';

    int64_t id = storage_->db.last_insert_rowid();

    storage_->db << "INSERT INTO CrateParentList (crateOriginId, "
                    "crateParentId) VALUES (?, ?)"
                 << id << id;

    crate cr{std::make_shared<el_crate_impl>(storage_, id)};

    trans.commit();

    return cr;
}

track el_database_impl::create_track(boost::string_view relative_path)
{
    // TODO (haslersn): Should it be allowed to create two tracks with the same
    // `relative_path`?

    auto filename = get_filename(relative_path);

    el_transaction_guard_impl trans{storage_};

    // Insert a new entry in the track table
    storage_->db << "INSERT INTO Track (path, filename, trackType, "
                    "isExternalTrack, idAlbumArt) VALUES (?,?,?,?,?)"
                 << relative_path.data()   //
                 << std::string{filename}  //
                 << 1                      // trackType
                 << 0                      // isExternalTrack
                 << 1;                     // idAlbumArt

    auto id = storage_->db.last_insert_rowid();

    if (version() >= version_1_7_1)
    {
        storage_->db << "UPDATE Track SET pdbImportKey = 0 WHERE id = ?" << id;
    }

    {
        auto extension = get_file_extension(filename);
        auto metadata_str_inserter =
            storage_->db
            << "REPLACE INTO MetaData (id, type, text) VALUES (?, ?, ?)";
        for (int64_t type : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 15, 16})
        {
            boost::optional<std::string> text;
            switch (type)
            {
                case 10:
                    // duration in MM:SS
                    // TODO (haslersn)
                    break;
                case 13:
                    // extension
                    if (extension)
                    {
                        text = extension->to_string();
                    }
                    break;
                case 15:
                case 16:
                    // Always 1 to our knowledge
                    text = "1";
                    break;
            }
            metadata_str_inserter << id << type << text;
            metadata_str_inserter++;
        }
    }

    {
        auto metadata_int_inserter = storage_->db
                                     << "REPLACE INTO MetaDataInteger (id, "
                                        "type, value) VALUES (?, ?, ?)";
        for (int64_t type = 1; type <= 11 /* 12 */; ++type)
        {
            boost::optional<int64_t> value;
            switch (type)
            {
                case 5: value = 0; break;
                case 11:
                    // case 12:
                    value = 1;
                    break;
            }
            metadata_int_inserter << id << type << value;
            metadata_int_inserter++;
        }
    }

    track tr{std::make_shared<el_track_impl>(storage_, id)};

    trans.commit();

    return tr;
}

std::string el_database_impl::directory()
{
    return storage_->directory;
}

bool el_database_impl::is_supported()
{
    return djinterop::enginelibrary::is_supported(version());
}

void el_database_impl::verify()
{
    // Verify music schema
    verify_music_schema(storage_->db);

    // Verify performance schema
    verify_performance_schema(storage_->db);
}

void el_database_impl::remove_crate(crate cr)
{
    storage_->db << "DELETE FROM Crate WHERE id = ?" << cr.id();
}

void el_database_impl::remove_track(track tr)
{
    storage_->db << "DELETE FROM Track WHERE id = ?" << tr.id();
    // All other references to the track should automatically be cleared by
    // "ON DELETE CASCADE"
}

std::vector<crate> el_database_impl::root_crates()
{
    std::vector<crate> results;
    storage_->db
            << "SELECT crateOriginId FROM CrateParentList WHERE crateParentId "
               "= crateOriginId ORDER BY crateOriginId" >>
        [&](int64_t id) {
            results.push_back(
                crate{std::make_shared<el_crate_impl>(storage_, id)});
        };
    return results;
}

boost::optional<track> el_database_impl::track_by_id(int64_t id)
{
    boost::optional<track> tr;
    storage_->db << "SELECT COUNT(*) FROM Track WHERE id = ?" << id >>
        [&](int64_t count) {
            if (count == 1)
            {
                tr = track{std::make_shared<el_track_impl>(storage_, id)};
            }
            else if (count > 1)
            {
                throw track_database_inconsistency{
                    "More than one track with the same ID", id};
            }
        };
    return tr;
}

std::vector<track> el_database_impl::tracks()
{
    std::vector<track> results;
    storage_->db << "SELECT id FROM Track ORDER BY id" >> [&](int64_t id) {
        results.push_back(track{std::make_shared<el_track_impl>(storage_, id)});
    };
    return results;
}

std::vector<track> el_database_impl::tracks_by_relative_path(
    boost::string_view relative_path)
{
    std::vector<track> results;
    storage_->db << "SELECT id FROM Track WHERE path = ? ORDER BY id"
                 << relative_path.data() >>
        [&](int64_t id) {
            results.push_back(
                track{std::make_shared<el_track_impl>(storage_, id)});
        };
    return results;
}

std::string el_database_impl::uuid()
{
    std::string uuid;
    storage_->db << "SELECT uuid FROM Information" >> uuid;
    return uuid;
}

semantic_version el_database_impl::version()
{
    semantic_version version;
    storage_->db << "SELECT schemaVersionMajor, schemaVersionMinor, "
                    "schemaVersionPatch FROM Information" >>
        std::tie(version.maj, version.min, version.pat);
    return version;
}

}  // namespace enginelibrary
}  // namespace djinterop
