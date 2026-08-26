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

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/track_snapshot.hpp>

#include "onelibrary_context.hpp"

namespace djinterop::onelibrary
{
/// One row of the `content` table, with its lookup tables resolved.
///
/// The schema declares no constraints at all: nothing is `NOT NULL` and no
/// foreign key is enforced, so every field is optional and a lookup reference
/// may point at a row that is not there.
struct content_row
{
    int64_t id = 0;

    std::optional<std::string> title;
    std::optional<std::string> artist;
    std::optional<std::string> composer;
    std::optional<std::string> album;
    std::optional<std::string> genre;

    /// The record label, which djinterop calls the publisher.
    std::optional<std::string> label;

    /// The musical key, in the notation rekordbox writes, such as `F#m`.
    ///
    /// Which notation a device carries follows the setting rekordbox exported
    /// it under, so it may equally be Camelot, such as `8A`.
    std::optional<std::string> key;

    /// Free-text comment the DJ attached to the track.
    std::optional<std::string> comment;

    /// Tempo in hundredths of a beat per minute: 12400 is 124.00 BPM.
    std::optional<int64_t> bpm_x100;

    /// Duration in whole seconds.
    ///
    /// The published description of the format calls this column
    /// milliseconds, which it is not: on an export written by rekordbox, a
    /// 320 kbps track of 6,517,615 bytes carries a `length` of 161, and
    /// 6517615 * 8 / 320000 is 163 seconds.  Every track on that device
    /// agrees to within a second, and none agrees on any other reading.
    std::optional<int64_t> length_seconds;

    std::optional<int64_t> track_number;
    std::optional<int64_t> release_year;

    /// Rating from zero to five stars, and not the 0-255 encoding that the
    /// rest of the rekordbox ecosystem uses: an export written by rekordbox
    /// holds only 0 and 5, and 5 is not a value that encoding can take.
    std::optional<int64_t> rating_stars;

    /// Device-relative POSIX path, such as
    /// `/Contents/Artist/Album/Track.mp3`.
    std::optional<std::string> path;

    std::optional<int64_t> file_size;
    std::optional<int64_t> bitrate;
    std::optional<int64_t> sampling_rate;
};

/// Read access to the `content` table and the lookup tables it references.
class content_table
{
public:
    explicit content_table(std::shared_ptr<onelibrary_context> context);

    /// Fetch one row by its identifier.
    [[nodiscard]] std::optional<content_row> get(int64_t id) const;

    /// Fetch the identifiers of every row, ordered.
    [[nodiscard]] std::vector<int64_t> all_ids() const;

    /// Fetch the identifiers of rows whose path matches, ordered.
    ///
    /// Paths in the database are absolute within the device and begin with a
    /// separator; a path given without one is matched as though it had one.
    [[nodiscard]] std::vector<int64_t> ids_by_path(
        const std::string& path) const;

    /// Test whether a row exists.
    [[nodiscard]] bool exists(int64_t id) const;

private:
    std::shared_ptr<onelibrary_context> context_;
};

/// Build a track snapshot from a content row.
[[nodiscard]] track_snapshot to_snapshot(const content_row& row);

/// Interpret the key notation that rekordbox writes, such as `F#m` or `Bb`.
///
/// Returns no value for a notation that is not recognised, rather than
/// guessing.
[[nodiscard]] std::optional<musical_key> parse_musical_key(
    const std::string& name);

}  // namespace djinterop::onelibrary
