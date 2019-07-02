#include <sqlite_modern_cpp.h>

#include <djinterop/djinterop.hpp>
#include <djinterop/enginelibrary/el_crate_impl.hpp>
#include <djinterop/enginelibrary/el_database_impl.hpp>
#include <djinterop/enginelibrary/el_storage.hpp>
#include <djinterop/enginelibrary/el_track_impl.hpp>

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
    storage_->music_db << "BEGIN";

    storage_->music_db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();

    storage_->music_db
        << "INSERT INTO CrateTrackList (crateId, trackId) VALUES (?, ?)" << id()
        << tr.id();

    storage_->music_db << "COMMIT";
}

std::vector<crate> el_crate_impl::children()
{
    std::vector<crate> results;
    storage_->music_db
            << "SELECT crateIdChild FROM CrateHierarchy WHERE crateId = ?"
            << id() >>
        [&](int64_t crate_id_child) {
            results.emplace_back(
                std::make_shared<el_crate_impl>(storage_, crate_id_child));
        };
    return results;
}

void el_crate_impl::clear_tracks()
{
    storage_->music_db << "DELETE FROM CrateTrackList WHERE crateId = ?"
                       << id();
}

database el_crate_impl::db()
{
    return database{std::make_shared<el_database_impl>(storage_)};
}

std::vector<crate> el_crate_impl::descendants()
{
    std::vector<crate> results;
    storage_->music_db
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
    storage_->music_db << "SELECT COUNT(*) FROM Crate WHERE id = ?" << id() >>
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
    storage_->music_db << "SELECT title FROM Crate WHERE id = ?" << id() >>
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
    storage_->music_db
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
    storage_->music_db
        << "DELETE FROM CrateTrackList WHERE crateId = ? AND trackId = ?"
        << id() << tr.id();
}

void el_crate_impl::set_name(boost::string_view name)
{
    storage_->music_db << "BEGIN";

    // obtain parent's `path`
    std::string parent_path;
    storage_->music_db
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
    storage_->music_db << "UPDATE Crate SET title = ?, path = ? WHERE id = ?"
                       << name.data() << path << id();

    // call the lambda in order to update the path of direct children
    for (crate cr : children())
    {
        update_path(storage_->music_db, cr, path);
    }

    storage_->music_db << "COMMIT";
}

void el_crate_impl::set_parent(boost::optional<crate> parent)
{
    storage_->music_db << "BEGIN";

    storage_->music_db << "DELETE FROM CrateParentList WHERE crateOriginId = ?"
                       << id();

    storage_->music_db << "INSERT INTO CrateParentList (crateOriginId, "
                          "crateParentId) VALUES (?, ?)"
                       << id() << (parent ? parent->id() : id());

    storage_->music_db << "DELETE FROM CrateHierarchy WHERE crateIdChild = ?"
                       << id();

    if (parent)
    {
        storage_->music_db
            << "INSERT INTO CrateHierarchy (crateId, crateIdChild) SELECT "
               "crateId, ? FROM CrateHierarchy WHERE crateIdChild = ? UNION "
               "SELECT ? AS crateId, ? AS crateIdChild"
            << id() << parent->id() << parent->id() << id();
    }

    storage_->music_db << "COMMIT";
}

std::vector<track> el_crate_impl::tracks()
{
    std::vector<track> results;
    storage_->music_db << "SELECT trackId FROM CrateTrackList WHERE crateId = ?"
                       << id() >>
        [&](int64_t track_id) {
            results.emplace_back(
                std::make_shared<el_track_impl>(storage_, track_id));
        };
    return results;
}

}  // namespace enginelibrary
}  // namespace djinterop