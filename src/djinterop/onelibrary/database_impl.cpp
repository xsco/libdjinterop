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

#include "database_impl.hpp"

#include <array>
#include <set>
#include <utility>

#include <djinterop/exceptions.hpp>
#include <djinterop/track.hpp>

#include "content_table.hpp"
#include "crate_impl.hpp"
#include "playlist_impl.hpp"
#include "playlist_table.hpp"
#include "track_impl.hpp"

namespace djinterop::onelibrary
{
namespace
{
/// The tables that must be present for a database to be a OneLibrary one.
///
/// A real export has twenty-two; demanding the ones this library does not read
/// would reject a database that is merely older or newer.
constexpr std::array<const char*, 8> required_tables{
    "content",  "artist",           "album",   "genre", "label",
    "playlist", "playlist_content", "property"};

}  // anonymous namespace

database_impl::database_impl(std::shared_ptr<onelibrary_context> context) :
    djinterop::database_impl{
        {feature::supports_nested_crates, feature::supports_nested_playlists,
         feature::playlists_support_duplicate_tracks}},
    context_{std::move(context)}
{
}

std::string database_impl::directory()
{
    return context_->directory;
}

std::string database_impl::uuid()
{
    // The format has no identifier for the library as such, so the library
    // each track was exported from -- constant across one export -- is the
    // closest thing available.
    std::string result;
    context_->db << "SELECT masterDbId FROM content "
                    "WHERE masterDbId IS NOT NULL LIMIT 1" >>
        [&](std::optional<int64_t> master_db_id)
    {
        if (master_db_id)
            result = std::to_string(*master_db_id);
    };

    return result;
}

std::string database_impl::version_name()
{
    std::string version;
    context_->db << "SELECT dbVersion FROM property LIMIT 1" >>
        [&](std::optional<std::string> db_version)
    { version = db_version.value_or(std::string{}); };

    return version.empty() ? "OneLibrary" : "OneLibrary " + version;
}

void database_impl::verify()
{
    std::set<std::string> present;
    context_->db << "SELECT name FROM sqlite_master WHERE type = 'table'" >>
        [&](std::string name) { present.insert(std::move(name)); };

    for (const auto& table : required_tables)
        if (present.count(table) == 0)
            throw database_inconsistency{
                std::string{"The table `"} + table +
                "` is missing, so this is not a OneLibrary database"};
}

std::optional<djinterop::track> database_impl::track_by_id(int64_t id)
{
    if (!content_table{context_}.exists(id))
        return std::nullopt;

    return track{std::make_shared<track_impl>(context_, id)};
}

std::vector<djinterop::track> database_impl::tracks()
{
    std::vector<djinterop::track> results;
    for (const auto& id : content_table{context_}.all_ids())
        results.push_back(track{std::make_shared<track_impl>(context_, id)});

    return results;
}

std::vector<djinterop::track> database_impl::tracks_by_relative_path(
    const std::string& relative_path)
{
    std::vector<djinterop::track> results;
    for (const auto& id : content_table{context_}.ids_by_path(relative_path))
        results.push_back(track{std::make_shared<track_impl>(context_, id)});

    return results;
}

std::optional<djinterop::crate> database_impl::crate_by_id(int64_t id)
{
    if (!playlist_table{context_}.exists(id))
        return std::nullopt;

    return make_crate(context_, id);
}

std::vector<djinterop::crate> database_impl::root_crates()
{
    std::vector<djinterop::crate> results;
    for (const auto& id : playlist_table{context_}.root_ids())
        results.push_back(make_crate(context_, id));

    return results;
}

std::optional<djinterop::crate> database_impl::root_crate_by_name(
    const std::string& name)
{
    const auto id = playlist_table{context_}.find_root(name);
    if (!id)
        return std::nullopt;

    return make_crate(context_, *id);
}

std::vector<playlist> database_impl::root_playlists()
{
    std::vector<playlist> results;
    for (const auto& id : playlist_table{context_}.root_ids())
        results.push_back(make_playlist(context_, id));

    return results;
}

std::optional<djinterop::playlist> database_impl::root_playlist_by_name(
    const std::string& name)
{
    const auto id = playlist_table{context_}.find_root(name);
    if (!id)
        return std::nullopt;

    return make_playlist(context_, *id);
}

crate database_impl::create_root_crate(const std::string&)
{
    read_only();
}

crate database_impl::create_root_crate_after(const std::string&, const crate&)
{
    read_only();
}

playlist database_impl::create_root_playlist(const std::string&)
{
    read_only();
}

playlist database_impl::create_root_playlist_after(
    const std::string&, const djinterop::playlist_impl&)
{
    read_only();
}

track database_impl::create_track(const track_snapshot&)
{
    read_only();
}

void database_impl::remove_crate(djinterop::crate)
{
    read_only();
}

void database_impl::remove_playlist(const djinterop::playlist_impl&)
{
    read_only();
}

void database_impl::remove_track(djinterop::track)
{
    read_only();
}

}  // namespace djinterop::onelibrary
