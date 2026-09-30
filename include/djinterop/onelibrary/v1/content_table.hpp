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
#ifndef DJINTEROP_ONELIBRARY_V1_CONTENT_TABLE_HPP
#define DJINTEROP_ONELIBRARY_V1_CONTENT_TABLE_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <djinterop/config.hpp>

namespace djinterop::onelibrary
{
struct onelibrary_context;

namespace v1
{
/// Special value for id to indicate that a given row is not a row of the
/// database.  Every row read from a device carries a real identifier, so this
/// is only ever the value a default-constructed row holds.
constexpr int64_t CONTENT_ROW_ID_NONE = 0;

/// Special value for the `color_id` column that indicates that a track is not
/// marked with a colour.
constexpr int64_t COLOR_ID_NONE = 0;

/// One row of the `content` table, with its lookup tables resolved.
///
/// The schema declares no constraints: nothing is `NOT NULL` and no foreign
/// key is enforced, so every field is optional and a lookup reference may
/// point at a row that is not there.
struct content_row
{
    int64_t id = CONTENT_ROW_ID_NONE;

    std::optional<std::string> title;
    std::optional<std::string> artist;
    std::optional<std::string> composer;
    std::optional<std::string> album;
    std::optional<std::string> genre;

    /// The record label, which djinterop calls the publisher.
    std::optional<std::string> label;

    /// The musical key in the notation rekordbox wrote, which follows the
    /// setting it exported under and may be Camelot, such as `8A`.
    std::optional<std::string> key;

    std::optional<std::string> comment;

    /// Tempo in hundredths of a beat per minute: 12400 is 124.00 BPM.
    std::optional<int64_t> bpm_x100;

    /// Duration, which the database records in whole seconds.
    ///
    /// The published description of the format calls this column
    /// milliseconds, which it is not: on a real export, a 320 kbps track of
    /// 6,517,615 bytes carries a `length` of 161, and 6517615 * 8 / 320000 is
    /// 163 seconds.  No other reading fits.
    std::optional<std::chrono::seconds> length;

    std::optional<int64_t> track_number;
    std::optional<int64_t> release_year;

    /// Rating in whole stars, from zero to five.
    ///
    /// This is not the 0-255 encoding the rest of the rekordbox ecosystem
    /// uses, whose only values are 0, 51, 102, 153, 204 and 255.  A real
    /// export holds a 5, which that encoding cannot express.
    std::optional<int64_t> rating_stars;

    /// POSIX path, absolute within the device and so beginning with a
    /// separator, such as `/Contents/Artist/Album/Track.mp3`.
    std::optional<std::string> path;

    /// Size of the file, in bytes.
    std::optional<int64_t> file_size;

    /// Bitrate of the file, in kilobits per second.
    std::optional<int64_t> bitrate;

    /// Sampling rate of the file, in hertz.
    std::optional<int64_t> sampling_rate;

    /// Path of the ANLZ analysis data, as the device records it.
    ///
    /// rekordbox keeps the beatgrid, cues, loops and waveforms in files beside
    /// the music rather than in the database.  This names the `.DAT` one; its
    /// siblings differ only in extension, `.EXT` holding the colour waveform
    /// and the cues beyond the first three, and `.2EX` the waveform a touch
    /// display draws.
    std::optional<std::string> analysis_path;

    /// The colour the track is marked with, or `COLOR_ID_NONE` for none.
    ///
    /// The eight colours rekordbox offers, numbered as the `export.pdb`
    /// library beside the database numbers them: one for pink, two red, three
    /// orange, four yellow, five green, six aqua, seven blue, eight purple.
    std::optional<int64_t> color_id;
};

/// Read access to the `content` table and the lookup tables it references.
///
/// A whole row can be read at once, or any one column on its own, which
/// avoids the joins that resolving every lookup table costs.
class DJINTEROP_PUBLIC content_table
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

    /// Fetch one column of one row.
    ///
    /// Each returns no value if the row is not there or the column is unset.
    /// Text that is set but empty counts as unset, which is how rekordbox
    /// writes metadata a track does not carry.
    /// @{
    [[nodiscard]] std::optional<std::string> get_title(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_artist(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_composer(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_album(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_genre(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_label(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_key(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_comment(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_bpm_x100(int64_t id) const;
    [[nodiscard]] std::optional<std::chrono::seconds> get_length(
        int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_track_number(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_release_year(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_rating_stars(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_path(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_file_size(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_bitrate(int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_sampling_rate(int64_t id) const;
    [[nodiscard]] std::optional<std::string> get_analysis_path(
        int64_t id) const;
    [[nodiscard]] std::optional<int64_t> get_color_id(int64_t id) const;
    /// @}

private:
    std::shared_ptr<onelibrary_context> context_;
};

}  // namespace v1
}  // namespace djinterop::onelibrary

#endif  // DJINTEROP_ONELIBRARY_V1_CONTENT_TABLE_HPP
