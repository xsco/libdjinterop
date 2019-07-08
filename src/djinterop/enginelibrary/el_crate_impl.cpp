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
#include <djinterop/enginelibrary/el_crate_impl.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/enginelibrary/el_track_impl.hpp>
#include <djinterop/enginelibrary/el_transaction_guard_impl.hpp>

namespace djinterop
{
namespace enginelibrary
{
using djinterop::crate;
using djinterop::track;

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

}  // namespace

el_crate_impl::el_crate_impl(std::shared_ptr<el_storage> storage, int64_t id)
    : crate_impl{id}, storage_{std::move(storage)}
{
}

void el_crate_impl::add_track(track tr)
{
    el_transaction_guard_impl trans{storage_};

    storage_->db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();

    storage_->db
        << "INSERT INTO CrateTrackList (crateId, trackId) VALUES (?, ?)" << id()
        << tr.id();

    trans.commit();
}

std::vector<crate> el_crate_impl::children()
{
    std::vector<crate> results;
    storage_->db << "SELECT crateIdChild FROM CrateHierarchy WHERE crateId = ?"
                 << id() >>
        [&](int64_t crate_id_child) {
            results.emplace_back(
                std::make_shared<el_crate_impl>(storage_, crate_id_child));
        };
    return results;
}

void el_crate_impl::clear_tracks()
{
    storage_->db << "DELETE FROM CrateTrackList WHERE crateId = ?" << id();
}

database el_crate_impl::db()
{
    return database{std::make_shared<el_database_impl>(storage_)};
}

std::vector<crate> el_crate_impl::descendants()
{
    std::vector<crate> results;
    storage_->db
            << "SELECT crateOriginId FROM CrateParentList WHERE crateParentId "
               "= ? AND crateOriginId <> crateParentId"
            << id() >>
        [&](int64_t descendant_id) {
            results.push_back(crate{
                std::make_shared<el_crate_impl>(storage_, descendant_id)});
        };
    return results;
}

bool el_crate_impl::is_valid()
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

std::string el_crate_impl::name()
{
    boost::optional<std::string> name;
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

boost::optional<crate> el_crate_impl::parent()
{
    boost::optional<crate> parent;
    storage_->db
            << "SELECT crateParentId FROM CrateParentList WHERE crateOriginId "
               "= ? AND crateParentId <> crateOriginId"
            << id() >>
        [&](int64_t parent_id) {
            if (!parent)
            {
                parent =
                    crate{std::make_shared<el_crate_impl>(storage_, parent_id)};
            }
            else
            {
                throw crate_database_inconsistency{
                    "More than one parent crate for the same crate", id()};
            }
        };
    return parent;
}

void el_crate_impl::remove_track(track tr)
{
    storage_->db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();
}

void el_crate_impl::set_name(boost::string_view name)
{
    el_transaction_guard_impl trans{storage_};

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

void el_crate_impl::set_parent(boost::optional<crate> parent)
{
    el_transaction_guard_impl trans{storage_};

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

std::vector<track> el_crate_impl::tracks()
{
    std::vector<track> results;
    storage_->db << "SELECT trackId FROM CrateTrackList WHERE crateId = ?"
                 << id() >>
        [&](int64_t track_id) {
            results.emplace_back(
                std::make_shared<el_track_impl>(storage_, track_id));
        };
    return results;
}

}  // namespace enginelibrary
}  // namespace djinterop