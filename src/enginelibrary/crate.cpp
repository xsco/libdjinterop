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

#include <string>
#include <vector>

#include "database_impl.hpp"

namespace djinterop
{
namespace enginelibrary
{
struct crate_row
{
    std::string title;
};

struct crate::impl
{
    impl(database db, int64_t id) : db_{std::move(db)}, id_{id} {}

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

    database db_;
    int64_t id_;
};

crate::crate(const crate& other) noexcept = default;

crate::~crate() = default;

crate& crate::operator=(const crate& other) noexcept = default;

void crate::add_track(track tr) const
{
    db().pimpl_->music_db_ << "BEGIN";

    db().pimpl_->music_db_
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();

    db().pimpl_->music_db_
        << "INSERT INTO CrateTrackList (crateId, trackId) VALUES (?, ?)" << id()
        << tr.id();

    db().pimpl_->music_db_ << "COMMIT";
}

std::vector<crate> crate::children() const
{
    std::vector<crate> results;
    db().pimpl_->music_db_
            << "SELECT crateIdChild FROM CrateHierarchy WHERE crateId = ?"
            << id() >>
        [&](int64_t crate_id_child) {
            results.push_back(crate{db(), crate_id_child});
        };
    return results;
}

void crate::clear_tracks() const
{
    db().pimpl_->music_db_ << "DELETE FROM CrateTrackList WHERE crateId = ?"
                           << id();
}

std::vector<crate> crate::descendants() const
{
    std::vector<crate> results;
    db().pimpl_->music_db_
            << "SELECT crateOriginId FROM CrateParentList WHERE crateParentId "
               "= ? AND crateOriginId <> crateParentId"
            << id() >>
        [&](int64_t descendant_id) {
            results.push_back(crate{db(), descendant_id});
        };
    return results;
}

database crate::db() const
{
    return pimpl_->db_;
}

int64_t crate::id() const
{
    return pimpl_->id_;
}

bool crate::is_valid() const
{
    bool valid = false;
    db().pimpl_->music_db_ << "SELECT COUNT(*) FROM Crate WHERE id = ?"
                           << id() >>
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

std::string crate::name() const
{
    boost::optional<std::string> name;
    db().pimpl_->music_db_ << "SELECT title FROM Crate WHERE id = ?" << id() >>
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

boost::optional<crate> crate::parent() const
{
    boost::optional<crate> parent;
    db().pimpl_->music_db_
            << "SELECT crateParentId FROM CrateParentList WHERE crateOriginId "
               "= ? AND crateParentId <> crateOriginId"
            << id() >>
        [&](int64_t parent_id) {
            if (!parent)
            {
                parent = crate{db(), parent_id};
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one parent crate for the same crate", id()};
            }
        };
    return parent;
}

void crate::remove_track(track tr) const
{
    db().pimpl_->music_db_
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();
}

void crate::set_name(const std::string& name) const
{
    db().pimpl_->music_db_ << "BEGIN";

    // obtain parent's `path`
    std::string parent_path;
    db().pimpl_->music_db_
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
    std::string path = std::move(parent_path) + name + ';';
    db().pimpl_->music_db_
        << "UPDATE Crate SET title = ?, path = ? WHERE id = ?" << name << path
        << id();

    // call the lambda in order to update the path of direct children
    for (crate cr : children())
    {
        pimpl_->update_path(db().pimpl_->music_db_, cr, path);
    }

    db().pimpl_->music_db_ << "COMMIT";
}

void crate::set_parent(boost::optional<crate> parent) const
{
    db().pimpl_->music_db_ << "BEGIN";

    db().pimpl_->music_db_
        << "DELETE FROM CrateParentList WHERE crateOriginId = ?" << id();

    db().pimpl_->music_db_ << "INSERT INTO CrateParentList (crateOriginId, "
                              "crateParentId) VALUES (?, ?)"
                           << id() << (parent ? parent->id() : id());

    db().pimpl_->music_db_
        << "DELETE FROM CrateHierarchy WHERE crateIdChild = ?" << id();

    if (parent)
    {
        db().pimpl_->music_db_
            << "INSERT INTO CrateHierarchy (crateId, crateIdChild) SELECT "
               "crateId, ? FROM CrateHierarchy WHERE crateIdChild = ? UNION "
               "SELECT ? AS crateId, ? AS crateIdChild"
            << id() << parent->id() << parent->id() << id();
    }

    db().pimpl_->music_db_ << "COMMIT";
}

std::vector<track> crate::tracks() const
{
    std::vector<track> results;
    db().pimpl_->music_db_
            << "SELECT trackId FROM CrateTrackList WHERE crateId = ?" << id() >>
        [&](int64_t track_id) {
            results.emplace_back(track{db(), track_id});
        };
    return results;
}

crate::crate(database db, int64_t id) : pimpl_{std::make_shared<impl>(db, id)}
{
}

}  // namespace enginelibrary
}  // namespace djinterop
