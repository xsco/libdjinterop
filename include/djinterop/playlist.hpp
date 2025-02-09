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
    void add_track_back(track tr);

    /// Adds a track to the playlist, after another track.
    ///
    /// A track can be contained in arbitrarily many (including zero) playlists.
    void add_track_after(track tr, track after);

    /// Add a range of tracks to the back of the playlist.
    template <typename TInputIterator>
    void add_tracks_back(TInputIterator first, TInputIterator last)
    {
        for (auto iter = first; iter != last; ++iter)
            add_track_back(*iter);
    }

    /// Removes all tracks from the playlist.
    ///
    /// Note that the tracks stay in the database even if they're contained in
    /// zero playlists.
    void clear_tracks();

    /// Returns the database containing the playlist.
    database db() const;

    /// Returns the playlist's name.
    std::string name() const;

    /// Removes a track from the playlist.  If the track is in the playlist
    /// multiple times, the first instance will be removed.
    ///
    /// Note that the track stays in the database even if it's contained in zero
    /// playlists.
    void remove_track(track tr);

    /// Sets the playlist's name.
    void set_name(const std::string& name);

    /// Returns the playlist's contained tracks, in order.
    ///
    /// \return Returns a vector of tracks.
    std::vector<track> tracks() const;

    playlist(std::shared_ptr<playlist_impl> pimpl);

private:
    std::shared_ptr<playlist_impl> pimpl_;
};

}  // namespace djinterop

#endif  // DJINTEROP_PLAYLIST_HPP
