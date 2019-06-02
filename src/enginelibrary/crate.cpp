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

#include <djinterop/enginelibrary/crate.hpp>

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include "sqlite_modern_cpp.h"

namespace djinterop {
namespace enginelibrary {

struct crate_row
{
    std::string title;
};

// Select a row from the Crate table
static crate_row select_crate_row(const database &db, int id)
{
    sqlite::database m_db{db.music_db_path()};
    crate_row row;
    int rows_found = 0;
    m_db
        << "SELECT title FROM Crate WHERE id = :1"
        << id
        >> [&](std::string title)
        {
            row = crate_row { title };
            ++rows_found;
        };

    if (rows_found == 0)
        throw nonexistent_crate{id};

    return row;
}

// Select a crate's parent id from the DB, if it has one
static int select_crate_parent_id(const database &db, int id)
{
    // The information about a crate hierarchy is stored in two different places
    // in the DB, so we will check both and ensure they are consistent
    sqlite::database m_db{db.music_db_path()};

    int parent_list_rows_found = 0;
    int parent_list_parent_id;
    m_db
        << "SELECT crateParentId FROM CrateParentList WHERE crateOriginId = :1"
        << id
        >> [&](int parent_id)
        {
            parent_list_parent_id = parent_id;
            ++parent_list_rows_found;
        };

    int hierarchy_rows_found = 0;
    int hierarchy_parent_id;
    m_db
        << "SELECT crateId FROM CrateHierarchy WHERE crateIdChild = :1"
        << id
        >> [&](int parent_id)
        {
            hierarchy_parent_id = parent_id;
            ++hierarchy_rows_found;
        };

    // There should always be entries in CrateParentList, and if a crate is at
    // the root level then it will be entered in this table with its parent id
    // equal to itself: clearly the Engine Library devs were not a fan of DRY!
    if (parent_list_rows_found == 0)
        throw crate_database_inconsistency{"No entry in CrateParentList", id};
    else if (parent_list_parent_id == id && hierarchy_rows_found > 0)
        throw crate_database_inconsistency{
            "Entry in CrateHierarchy for root Crate", id};
    else if (parent_list_parent_id != id && hierarchy_rows_found == 0)
        throw crate_database_inconsistency{
            "No entry in CrateHierarchy", id};
    else if (parent_list_parent_id != id &&
            parent_list_parent_id != hierarchy_parent_id)
        throw crate_database_inconsistency{
            "CrateParentList/CrateHierarchy specify different crate parents", id};

    // Internally we will use parent_id = 0 to signify a root-level crate
    return parent_list_parent_id != id ? parent_list_parent_id : 0;
}

static std::vector<int> select_child_crate_ids(const database &db, int id)
{
    // The information about a crate hierarchy is stored in two different places
    // in the DB, but we will only consider CrateHierarchy for this function
    sqlite::database m_db{db.music_db_path()};
    std::vector<int> child_crate_ids;
    m_db
        << "SELECT crateIdChild FROM CrateHierarchy WHERE crateId = :1"
        << id
        >> [&](int child_crate_id)
        {
            child_crate_ids.push_back(child_crate_id);
        };

    return child_crate_ids;
}

static std::unordered_set<int> select_track_ids(const database &db, int id)
{
    sqlite::database m_db{db.music_db_path()};
    std::unordered_set<int> track_ids;
    m_db
        << "SELECT trackId FROM CrateTrackList WHERE crateId = :1"
        << id
        >> [&](int track_id)
        {
            track_ids.insert(track_id);
        };

    return track_ids;
}

struct crate::impl
{
    impl(const database &db, int id) :
        id_{id},
        load_db_uuid_{db.uuid()},
        crate_row_{select_crate_row(db, id)},
        parent_id_{select_crate_parent_id(db, id)},
        child_crate_ids_{select_child_crate_ids(db, id)},
        track_ids_{select_track_ids(db, id)}
    {}

    impl(const crate::impl &other) :
        id_{0},
        load_db_uuid_{}, // Copy doesn't belong to a database (yet)
        crate_row_{other.crate_row_},
        parent_id_{other.parent_id_},
        child_crate_ids_{}, // Can't be the parent of anything else
        track_ids_{other.track_ids_}
    {}

    impl() :
        id_{0},
        parent_id_{0}
    {}
    
    int id_;
    std::string load_db_uuid_;
    crate_row crate_row_;
    int parent_id_;
    std::vector<int> child_crate_ids_;
    std::unordered_set<int> track_ids_;
};


crate::crate(const database &database, int id) : pimpl_{new impl{database, id}}
{}

crate::crate(const crate &other) : pimpl_{new impl{*other.pimpl_}}
{}

crate::crate() : pimpl_{new impl{}}
{}

crate::~crate() = default;

crate &crate::operator =(const crate &other)
{
    if (this != &other)
        pimpl_.reset(new impl{*other.pimpl_});
    return *this;
}

int crate::id() const
{
    return pimpl_->id_;
}

std::string crate::name() const
{
    return pimpl_->crate_row_.title;
}

bool crate::has_parent() const
{
    return pimpl_->parent_id_ != 0;
}

int crate::parent_id() const
{
    return pimpl_->parent_id_;
}

crate::crate_id_iterator crate::child_crates_begin() const
{
    return std::begin(pimpl_->child_crate_ids_);
}

crate::crate_id_iterator crate::child_crates_end() const
{
    return std::end(pimpl_->child_crate_ids_);
}

crate::track_id_iterator crate::tracks_begin() const
{
    return std::begin(pimpl_->track_ids_);
}

crate::track_id_iterator crate::tracks_end() const
{
    return std::end(pimpl_->track_ids_);
}

void crate::set_name(const std::string &name)
{
    pimpl_->crate_row_.title = name;
}

void crate::set_parent_id(int parent_crate_id)
{
    pimpl_->parent_id_ = parent_crate_id;
}

void crate::set_no_parent()
{
    pimpl_->parent_id_ = 0;
}

void crate::add_tracks(track_id_iterator begin, track_id_iterator end)
{
    pimpl_->track_ids_.insert(begin, end);
}

void crate::add_track(int track_id)
{
    pimpl_->track_ids_.insert(track_id);
}

void crate::set_tracks(track_id_iterator begin, track_id_iterator end)
{
    pimpl_->track_ids_.clear();
    pimpl_->track_ids_.insert(begin, end);
}

void crate::clear_tracks()
{
    pimpl_->track_ids_.clear();
}


void crate::save(const database &database)
{
    // Check mandatory fields
    if (pimpl_->crate_row_.title == "")
        throw std::invalid_argument{"Name must be populated"};

    // Work out if we are creating a new entry or not
    auto new_entry = id() == 0 || pimpl_->load_db_uuid_ != database.uuid();

    // Do all DB writing in a transaction
    sqlite::database m_db{database.music_db_path()};
    m_db << "BEGIN";

    // Calculate path for this crate by appending this crate's name to that of
    // its parent
    std::string parent_path;
    if (pimpl_->parent_id_ != 0)
    {
        m_db
            << "SELECT path FROM Crate WHERE id = ?"
            << pimpl_->parent_id_
            >> parent_path;
    }
    std::string path{parent_path + pimpl_->crate_row_.title + ";"};

    // Insert/update the Crate table
    if (new_entry)
    {
        // Insert a new entry in the Crate table
        m_db
            << "INSERT INTO Crate (title, path) VALUES (?, ?)"
            << pimpl_->crate_row_.title
            << path;
        pimpl_->id_ = m_db.last_insert_rowid();
    }
    else
    {
        // Update existing entry
        m_db
            << "UPDATE Crate SET title = ?, path = ? WHERE id = ?"
            << pimpl_->crate_row_.title
            << path
            << pimpl_->id_;
    }

    // Write hierarchy to parent list
    if (pimpl_->parent_id_ == 0)
    {
        // Write to CrateParentList
        m_db
            << "INSERT OR REPLACE INTO CrateParentList ("
               "  crateOriginId, crateParentId) "
               "VALUES (?, ?)"
            << pimpl_->id_
            << pimpl_->id_;
        // Remove anything existing from CrateHierarchy
        m_db
            << "DELETE FROM CrateHierarchy WHERE crateIdChild = ?"
            << pimpl_->id_;
    }
    else
    {
        // Write to both CrateParentList and CrateHierarchy
        m_db
            << "INSERT OR REPLACE INTO CrateParentList ("
               "  crateOriginId, crateParentId) "
               "VALUES (?, ?)"
            << pimpl_->id_
            << pimpl_->parent_id_;
        m_db
            << "INSERT OR REPLACE INTO CrateHierarchy ("
               "  crateId, crateIdChild) "
               "VALUES (?, ?)"
            << pimpl_->parent_id_
            << pimpl_->id_;
    }

    // Clear track list first, then add
    m_db << "DELETE FROM CrateTrackList WHERE crateId = ?" << pimpl_->id_;
    for (auto &track_id : pimpl_->track_ids_)
    {
        m_db
            << "INSERT INTO CrateTrackList (crateId, trackId) VALUES (?, ?)"
            << pimpl_->id_
            << track_id;
    }

    m_db << "COMMIT";
    pimpl_->load_db_uuid_ = database.uuid();
}

std::vector<int> all_crate_ids(const database &database)
{
    sqlite::database m_db{database.music_db_path()};
    std::vector<int> ids;
    m_db
        << "SELECT id FROM Crate ORDER BY id"
        >> [&ids](int id) { ids.push_back(id); };

    return ids;
}

std::vector<int> all_root_crate_ids(const database &database)
{
    sqlite::database m_db{database.music_db_path()};
    std::vector<int> ids;
    m_db
        <<
            "SELECT id "
            "FROM Crate c "
            "INNER JOIN CrateParentList cpl ON (cpl.crateOriginId = c.id) "
            "WHERE cpl.crateParentId = cpl.crateOriginId "
            "ORDER BY c.id"
        >> [&ids](int id) { ids.push_back(id); };

    return ids;
}

/**
 * \brief Try to find a crate by its (unique) name
 *
 * If the crate is found, its id will be written to the provided reference
 * variable.
 */
bool find_crate_by_name(
        const database &database,
        const std::string &name,
        int &crate_id)
{
    sqlite::database m_db{database.music_db_path()};
    bool found = false;
    m_db
        << "SELECT id FROM Crate WHERE title = ?"
        << name
        >> [&](int id) { crate_id = id; found = true; };

    return found;
}

} // enginelibrary
} // djinterop
