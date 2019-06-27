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

#include <djinterop/enginelibrary.hpp>

#include <sys/stat.h>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#endif
#include <tuple>

#include "database_impl.hpp"
#include "util.hpp"

namespace djinterop
{
namespace enginelibrary
{
database::database(const std::string& directory)
    : pimpl_{std::make_shared<database_impl>(directory)}
{
}

database::database(const database& db) = default;

database::~database() = default;

database& database::operator=(const database& db) = default;

boost::optional<crate> database::crate_by_id(int64_t id) const
{
    boost::optional<crate> cr;
    pimpl_->music_db_ << "SELECT COUNT(*) FROM Crate WHERE id = ?" << id >>
        [&](int64_t count) {
            if (count == 1)
            {
                cr = crate{*this, id};
            }
            else if (count > 1)
            {
                throw crate_database_inconsistency{
                    "More than one crate with the same ID", id};
            }
        };
    return cr;
}

std::vector<crate> database::crates() const
{
    std::vector<crate> results;
    pimpl_->music_db_ << "SELECT id FROM Crate ORDER BY id" >> [&](int64_t id) {
        results.push_back(crate{*this, id});
    };
    return results;
}

std::vector<crate> database::crates_by_name(const std::string& name) const
{
    std::vector<crate> results;
    pimpl_->music_db_ << "SELECT id FROM Crate WHERE title = ? ORDER BY id"
                      << name >>
        [&](int64_t id) {
            results.push_back(crate{*this, id});
        };
    return results;
}

crate database::create_crate(const std::string& name) const
{
    pimpl_->music_db_ << "BEGIN";

    pimpl_->music_db_ << "INSERT INTO Crate (title, path) VALUES (?, ?)" << name
                      << std::string{name} + ';';

    int64_t id = pimpl_->music_db_.last_insert_rowid();

    pimpl_->music_db_ << "INSERT INTO CrateParentList (crateOriginId, "
                         "crateParentId) VALUES (?, ?)"
                      << id << id;

    pimpl_->music_db_ << "COMMIT";

    return crate{*this, id};
}

track database::create_track(const std::string& relative_path) const
{
    // TODO (haslersn): Should it be allowed to create two tracks with the same
    // `relative_path`?

    auto filename = get_filename(relative_path);

    pimpl_->music_db_ << "BEGIN";

    // Insert a new entry in the track table
    pimpl_->music_db_ << "INSERT INTO Track (path, filename, trackType, "
                         "isExternalTrack, idAlbumArt) VALUES (?,?,?,?,?)"
                      << relative_path          //
                      << std::string{filename}  //
                      << 1                      // trackType
                      << 0                      // isExternalTrack
                      << 1;                     // idAlbumArt

    auto id = pimpl_->music_db_.last_insert_rowid();

    if (version() >= version_1_7_1)
    {
        pimpl_->music_db_ << "UPDATE Track SET pdbImportKey = 0 WHERE id = ?"
                          << id;
    }

    {
        auto extension = get_file_extension(filename);
        auto metadata_str_inserter =
            pimpl_->music_db_
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
        auto metadata_int_inserter = pimpl_->music_db_
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

    pimpl_->music_db_ << "COMMIT";

    return track{*this, id};
}

std::string database::directory() const
{
    return pimpl_->directory_;
}

bool database::is_supported() const
{
    return djinterop::enginelibrary::is_supported(version());
}

void database::verify() const
{
    // Verify music schema
    verify_music_schema(pimpl_->music_db_);

    // Verify performance schema
    verify_performance_schema(pimpl_->perfdata_db_);
}

std::string database::music_db_path() const
{
    return pimpl_->music_db_path_;
}

std::string database::perfdata_db_path() const
{
    return pimpl_->perfdata_db_path_;
}

void database::remove_crate(crate cr) const
{
    pimpl_->music_db_ << "DELETE FROM Crate WHERE id = ?" << cr.id();
}

void database::remove_track(track tr) const
{
    pimpl_->music_db_ << "DELETE FROM Track WHERE id = ?" << tr.id();
    // All other references to the track should automatically be cleared by
    // "ON DELETE CASCADE"
}

std::vector<crate> database::root_crates() const
{
    std::vector<crate> results;
    pimpl_->music_db_
            << "SELECT crateOriginId FROM CrateParentList WHERE crateParentId "
               "= crateOriginId ORDER BY crateOriginId" >>
        [&](int64_t id) {
            results.push_back(crate{*this, id});
        };
    return results;
}

boost::optional<track> database::track_by_id(int64_t id) const
{
    boost::optional<track> tr;
    pimpl_->music_db_ << "SELECT COUNT(*) FROM Track WHERE id = ?" << id >>
        [&](int64_t count) {
            if (count == 1)
            {
                tr = track{*this, id};
            }
            else if (count > 1)
            {
                throw track_database_inconsistency{
                    "More than one track with the same ID", id};
            }
        };
    return tr;
}

std::vector<track> database::tracks() const
{
    std::vector<track> results;
    pimpl_->music_db_ << "SELECT id FROM Track ORDER BY id" >> [&](int64_t id) {
        results.push_back(track{*this, id});
    };
    return results;
}

std::vector<track> database::tracks_by_relative_path(
    const std::string& relative_path) const
{
    std::vector<track> results;
    pimpl_->music_db_ << "SELECT id FROM Track WHERE path = ? ORDER BY id"
                      << relative_path >>
        [&](int64_t id) {
            results.push_back(track{*this, id});
        };
    return results;
}

std::string database::uuid() const
{
    std::string uuid;
    pimpl_->music_db_ << "SELECT uuid FROM Information" >> uuid;
    return uuid;
}

schema_version database::version() const
{
    schema_version version;
    pimpl_->music_db_ << "SELECT schemaVersionMajor, schemaVersionMinor, "
                         "schemaVersionPatch FROM Information" >>
        std::tie(version.maj, version.min, version.pat);
    return version;
}

database make_database(
    const std::string& directory, const schema_version& default_version)
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

    return database{directory};
}

}  // namespace enginelibrary
}  // namespace djinterop
