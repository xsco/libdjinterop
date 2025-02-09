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

#pragma once
#ifndef DJINTEROP_PLAYLIST_HPP
#define DJINTEROP_PLAYLIST_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop
{
class database;
class playlist_impl;
class track;

/// A `playlist` object is a handle to a playlist stored in a database.
///
/// `playlist` objects can be copied and assigned cheaply, resulting in multiple
/// handles to the same actual playlist.
///
/// The read/write operations provided by this class directly access the
/// database.
///
/// A `playlist` object becomes invalid if the playlist gets deleted by
/// `database::remove_playlist()`. After that, you must not call any methods on
/// the `playlist` object, except for destructing it, or assigning to it.
class DJINTEROP_PUBLIC playlist
{
public:
    /// Copy constructor
    playlist(const playlist& other) noexcept;

    /// Destructor
    ~playlist();

    /// Copy assignment operator
    playlist& operator=(const playlist& other) noexcept;

    /// Adds a track to the end of the playlist.
    ///
    /// A track can be contained in arbitrarily many (including zero) playlists.
    void add_track_back(const track& tr);

    /// Adds a track to the playlist, after another track.
    ///
    /// A track can be contained in arbitrarily many (including zero) playlists.
    void add_track_after(const track& tr, const track& after);

    /// Add a range of tracks to the back of the playlist.
    template <typename TInputIterator>
    void add_tracks_back(TInputIterator first, TInputIterator last)
    {
        for (auto iter = first; iter != last; ++iter)
            add_track_back(*iter);
    }

    /// Returns the (direct) children of this playlist.
    std::vector<playlist> children() const;

    /// Removes all tracks from the playlist.
    ///
    /// Note that the tracks stay in the database even if they're contained in
    /// zero playlists.
    void clear_tracks();

    /// Create a new playlist as a child of this one.
    playlist create_sub_playlist(const std::string& name);

    /// Create a new playlist as a child of this one, after the given playlist
    /// in order.
    playlist create_sub_playlist_after(
        const std::string& name, const playlist& after);

    /// Returns the database containing the playlist.
    database db() const;

    /// Returns the playlist's name.
    std::string name() const;

    /// Returns the parent playlist, if this playlist has one.
    ///
    /// If the playlist doesn't have a parent, then `std::nullopt` is
    /// returned.
    std::optional<playlist> parent() const;

    /// Removes a track from the playlist.  If the track is in the playlist
    /// multiple times, the first instance will be removed.
    ///
    /// Note that the track stays in the database even if it's contained in zero
    /// playlists.
    void remove_track(const track& tr);

    /// Sets the playlist's name.
    void set_name(const std::string& name);

    /// Sets this playlist's parent.
    ///
    /// If `std::nullopt` is given, then this playlist will have no parent.
    /// That is, it becomes a root playlist.
    void set_parent(const std::optional<playlist>& parent) const;

    /// Gets the sub-playlist of this one with a given name.
    ///
    /// Note that descendants of sub-playlists are not found by this method, i.e.
    /// the search does not recurse into the immediate sub-playlists of this playlist.
    ///
    /// If no such playlist is found, then `std::nullopt` is returned.
    std::optional<playlist> sub_playlist_by_name(const std::string& name) const;

    /// Returns the playlist's contained tracks, in order.
    ///
    /// \return Returns a vector of tracks.
    std::vector<track> tracks() const;

    /// Equality operator.  Playlists belonging to different databases can never
    /// be equal.
    bool operator==(const playlist& other) const;

    playlist(std::shared_ptr<playlist_impl> pimpl);

    friend database;

private:
    std::shared_ptr<playlist_impl> pimpl_;
};

}  // namespace djinterop

#endif  // DJINTEROP_PLAYLIST_HPP
