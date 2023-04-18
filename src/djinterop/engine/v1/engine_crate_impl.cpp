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

#include <sqlite_modern_cpp.h>

#include <djinterop/djinterop.hpp>

#include "engine_crate_impl.hpp"
#include "engine_database_impl.hpp"
#include "engine_storage.hpp"
#include "engine_track_impl.hpp"
#include "engine_transaction_guard_impl.hpp"

namespace djinterop::engine::v1
{
using djinterop::crate;
using djinterop::track;

// Note that crates in the Engine Library format may exist either at top/root
// level, or be sub-crates underneath another crate.  This information is
// encoded redundantly in multiple places in the EL database schema:
//
// * Crate (id, title, path)
//     The `path` field is a semicolon-delimited string of crate titles,
//     representing the path from the root to the current crate.  Note that
//     there is always an additional trailing semicolon in this field.  As such,
//     semicolon is a prohibited character in crate names.
//
// * CrateParentList (crateOriginId, crateParentId)
//     Every crate is specified as having precisely one immediate parent.  A
//     top-level crate is said to have itself as parent.  The crate id is
//     written to the `crateOriginId` field, and the parent (or itself) is
//     written to the `crateParentId` field.
//
// * CrateHierarchy (crateId, crateIdChild)
//     The denormalised/flattened inheritance hierarchy is written to this
//     table, whereby the id of every descendant (not child) of a crate is
//     written to the `crateIdChild` field.  Note that the reflexive
//     relationship is not written to this table.
namespace
{
void update_path(
    sqlite::database& music_db, crate cr, const std::string& parent_path)
{
    // update path
    std::string path = parent_path + cr.name() + ';';
    music_db << "UPDATE Crate SET path = ? WHERE id = ?" << path << cr.id();

    // recursive call in order to update the path of indirect descendants
    for (crate cr2 : cr.children())
    {
        update_path(music_db, cr2, path);
    }
}

void ensure_valid_name(const std::string& name)
{
    if (name == "")
    {
        throw djinterop::crate_invalid_name{
            "Crate names must be non-empty", name};
    }
    else if (name.find_first_of(';') != std::string::npos)
    {
        throw djinterop::crate_invalid_name{
            "Crate names must not contain semicolons", name};
    }
}

}  // namespace

engine_crate_impl::engine_crate_impl(std::shared_ptr<engine_storage> storage, int64_t id) :
    crate_impl{id}, storage_{std::move(storage)}
{
}

void engine_crate_impl::add_track(int64_t track_id)
{
    engine_transaction_guard_impl trans{storage_};

    storage_->db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << track_id;

    storage_->db
        << "INSERT INTO CrateTrackList (crateId, trackId) VALUES (?, ?)" << id()
        << track_id;

    trans.commit();
}

void engine_crate_impl::add_track(track tr)
{
    add_track(tr.id());
}

std::vector<crate> engine_crate_impl::children()
{
    std::vector<crate> results;
    storage_->db << "SELECT crateIdChild FROM CrateHierarchy WHERE crateId = ?"
                 << id() >>
        [&](int64_t crate_id_child) {
            results.emplace_back(
                std::make_shared<engine_crate_impl>(storage_, crate_id_child));
        };
    return results;
}

void engine_crate_impl::clear_tracks()
{
    storage_->db << "DELETE FROM CrateTrackList WHERE crateId = ?" << id();
}

crate engine_crate_impl::create_sub_crate(std::string name)
{
    ensure_valid_name(name);
    engine_transaction_guard_impl trans{storage_};

    std::string path;
    storage_->db << "SELECT path FROM Crate WHERE id = ?" << id() >>
        [&](std::string path_val) {
            if (path.empty())
            {
                path = std::move(path_val);
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one crate for the same id", id()};
            }
        };

    int64_t sub_id;
    if (storage_->version >= desktop_1_1_1)
    {
        // Newer schemas consider crates to be a kind of 'list', and so the
        // `Crate` table has been replaced with a VIEW onto `List`.  The main
        // difference is that `List` does not have an integer primary key, so
        // the new id will need to be determined in advance.
        storage_->db << "SELECT IFNULL(MAX(id), 0) + 1 FROM Crate" >> sub_id;
        storage_->db << "INSERT INTO Crate (id, title, path) VALUES (?, ?, ?)"
                     << sub_id << name.data() << (path + name + ";");
    }
    else
    {
        // Older schema versions have a dedicated table for crates that has
        // an integer primary key, which will be filled automatically.
        storage_->db << "INSERT INTO Crate (title, path) VALUES (?, ?)"
                     << name.data() << (path + name + ";");
        sub_id = storage_->db.last_insert_rowid();
    }

    storage_->db << "INSERT INTO CrateParentList (crateOriginId, "
                    "crateParentId) VALUES (?, ?)"
                 << sub_id << id();

    storage_->db << "INSERT INTO CrateHierarchy (crateId, crateIdChild) "
                    "SELECT crateId, ? FROM CrateHierarchy "
                    "WHERE crateIdChild = ? "
                    "UNION "
                    "SELECT ? AS crateId, ? AS crateIdChild"
                 << sub_id << id() << id() << sub_id;

    crate cr{std::make_shared<engine_crate_impl>(storage_, sub_id)};

    trans.commit();

    return cr;
}

database engine_crate_impl::db()
{
    return database{std::make_shared<engine_database_impl>(storage_)};
}

std::vector<crate> engine_crate_impl::descendants()
{
    std::vector<crate> results;
    storage_->db
            << "SELECT crateOriginId FROM CrateParentList WHERE crateParentId "
               "= ? AND crateOriginId <> crateParentId"
            << id() >>
        [&](int64_t descendant_id) {
            results.push_back(crate{
                std::make_shared<engine_crate_impl>(storage_, descendant_id)});
        };
    return results;
}

bool engine_crate_impl::is_valid()
{
    bool valid = false;
    storage_->db << "SELECT COUNT(*) FROM Crate WHERE id = ?" << id() >>
        [&](int count) {
            if (count == 1)
            {
                valid = true;
            }
            else if (count > 1)
            {
                throw crate_database_inconsistency{
                    "More than one crate with the same ID", id()};
            }
        };
    return valid;
}

std::string engine_crate_impl::name()
{
    stdx::optional<std::string> name;
    storage_->db << "SELECT title FROM Crate WHERE id = ?" << id() >>
        [&](std::string title) {
            if (!name)
            {
                name = std::move(title);
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one crate with the same ID", id()};
            }
        };
    if (!name)
    {
        throw crate_deleted{id()};
    }
    return *name;
}

stdx::optional<crate> engine_crate_impl::parent()
{
    stdx::optional<crate> parent;
    storage_->db
            << "SELECT crateParentId FROM CrateParentList WHERE crateOriginId "
               "= ? AND crateParentId <> crateOriginId"
            << id() >>
        [&](int64_t parent_id) {
            if (!parent)
            {
                parent =
                    crate{std::make_shared<engine_crate_impl>(storage_, parent_id)};
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one parent crate for the same crate", id()};
            }
        };
    return parent;
}

void engine_crate_impl::remove_track(track tr)
{
    storage_->db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();
}

void engine_crate_impl::set_name(std::string name)
{
    ensure_valid_name(name);
    engine_transaction_guard_impl trans{storage_};

    // obtain parent's `path`
    std::string parent_path;
    storage_->db
            << "SELECT path FROM Crate c JOIN CrateParentList cpl ON c.id = "
               "cpl.crateParentId WHERE cpl.crateOriginId = ? AND "
               "cpl.crateOriginId <> cpl.crateParentId"
            << id() >>
        [&](std::string path) {
            if (parent_path.empty())
            {
                parent_path = std::move(path);
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one parent crate for the same crate", id()};
            }
        };

    // update name and path
    std::string path = std::move(parent_path) + name.data() + ';';
    storage_->db << "UPDATE Crate SET title = ?, path = ? WHERE id = ?"
                 << name.data() << path << id();

    // call the lambda in order to update the path of direct children
    for (crate cr : children())
    {
        update_path(storage_->db, cr, path);
    }

    trans.commit();
}

void engine_crate_impl::set_parent(stdx::optional<crate> parent)
{
    if (parent && parent->id() == id())
    {
        throw crate_invalid_parent{"Cannot set crate parent to self"};
    }

    engine_transaction_guard_impl trans{storage_};

    storage_->db << "DELETE FROM CrateParentList WHERE crateOriginId = ?"
                 << id();

    storage_->db << "INSERT INTO CrateParentList (crateOriginId, "
                    "crateParentId) VALUES (?, ?)"
                 << id() << (parent ? parent->id() : id());

    storage_->db << "DELETE FROM CrateHierarchy WHERE crateIdChild = ?" << id();

    if (parent)
    {
        storage_->db
            << "INSERT INTO CrateHierarchy (crateId, crateIdChild) SELECT "
               "crateId, ? FROM CrateHierarchy WHERE crateIdChild = ? UNION "
               "SELECT ? AS crateId, ? AS crateIdChild"
            << id() << parent->id() << parent->id() << id();
    }

    trans.commit();
}

stdx::optional<crate> engine_crate_impl::sub_crate_by_name(const std::string& name)
{
    stdx::optional<crate> cr;
    storage_->db << "SELECT cr.id FROM Crate cr "
                    "JOIN CrateParentList cpl ON (cpl.crateOriginId = cr.id) "
                    "WHERE cr.title = ? "
                    "AND cpl.crateParentId = ? "
                    "ORDER BY cr.id"
                 << name.data() << id() >>
        [&](int64_t id) {
            cr = crate{std::make_shared<engine_crate_impl>(storage_, id)};
        };
    return cr;
}

std::vector<track> engine_crate_impl::tracks()
{
    std::vector<track> results;
    storage_->db << "SELECT trackId FROM CrateTrackList WHERE crateId = ?"
                 << id() >>
        [&](int64_t track_id) {
            results.emplace_back(
                std::make_shared<engine_track_impl>(storage_, track_id));
        };
    return results;
}

}  // namespace djinterop::engine::v1
