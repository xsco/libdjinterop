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
#ifndef DJINTEROP_ONELIBRARY_ONELIBRARY_HPP
#define DJINTEROP_ONELIBRARY_ONELIBRARY_HPP

#include <memory>
#include <optional>
#include <string>

#include <djinterop/config.hpp>
#include <djinterop/database.hpp>

/// Support for the AlphaTheta OneLibrary device format, also documented as
/// Device Library Plus.
///
/// OneLibrary succeeds the DeviceSQL `export.pdb` library that rekordbox wrote
/// to USB media; a device usually carries both, and a player that understands
/// OneLibrary prefers it.  The library is one SQLite database encrypted with
/// SQLCipher 4, whose page format is implemented in `util/crypto`, so neither
/// SQLCipher nor OpenSSL is needed to read one.
///
/// Support here is currently **read-only**.  Everything that changes a
/// database throws `djinterop::unsupported_operation`.
namespace djinterop::onelibrary
{
/// Location of the library database within a device, relative to its root.
constexpr const char* database_relative_path =
    "PIONEER/rekordbox/exportLibrary.db";

/// The passphrase with which rekordbox encrypts every `exportLibrary.db`.
///
/// It is the same on every installation, and depends on neither licence nor
/// machine, so any player can read any device.  Pass a different one to
/// `load_database` if a future release of rekordbox changes it.
constexpr const char* default_passphrase =
    "r8gddnr4k847830ar6cqzbkk0el6qytmb3trbbx805jm74vez64i5o8fnrqryqls";

/// Test whether a OneLibrary database exists in a given location.
///
/// \param path Either the root directory of a device, or the database file.
/// \return Returns true if a database is present.
bool DJINTEROP_PUBLIC database_exists(const std::string& path);

/// Load a OneLibrary database.
///
/// \param path Either the root directory of a device, such as the mount point
///             of a USB drive, or the `exportLibrary.db` file itself.
/// \param passphrase Passphrase with which the database is encrypted.
/// \return Returns the loaded database.
/// \throws database_not_found If no database is present at the given path.
/// \throws unsupported_database If the passphrase does not open the database,
///                              or the SQLite in use is older than 3.36, which
///                              is the oldest that can read one.
/// \throws database_inconsistency If the database does not hold the tables
///                                that a OneLibrary database is expected to.
database DJINTEROP_PUBLIC load_database(
    const std::string& path,
    const std::string& passphrase = default_passphrase);

/// State shared by everything belonging to one loaded database.
struct onelibrary_context;

/// A loaded OneLibrary device, with access to the parts of the format that
/// the format-agnostic `database` interface has nowhere to put.
///
/// A device is loaded once and read many times: decryption derives a key,
/// which is deliberately expensive, and then holds the whole database in
/// memory, so a caller that wants both the unified interface and the extras
/// below should load a `library` and take `db()` from it rather than also
/// calling `load_database`.
class DJINTEROP_PUBLIC library
{
public:
    /// Load the database on a device.
    ///
    /// Arguments and exceptions are those of `load_database`.
    explicit library(
        const std::string& path,
        const std::string& passphrase = default_passphrase);

    /// The device, through the format-agnostic interface.
    [[nodiscard]] database db() const;

    /// Root directory of the device, to which every path is relative.
    ///
    /// This is the directory that was loaded, and not the directory the
    /// database file itself sits in.
    [[nodiscard]] const std::string& directory() const;

    /// Path of the ANLZ analysis data for a track, relative to `directory()`.
    ///
    /// rekordbox keeps the beatgrid, cues, loops and waveforms out of the
    /// database and in a set of files beside the music, which this names: the
    /// `.DAT` file given here, and the siblings that differ from it only in
    /// extension.  `.EXT` holds the colour waveform and the cues beyond the
    /// first three, and `.2EX` the waveform that a touch display draws.
    ///
    /// \param track_id Identifier of the track, as `track::id()` gives it.
    /// \return Returns the path, or no value if the track is not there or
    ///         carries no analysis data.
    [[nodiscard]] std::optional<std::string> analysis_path(
        int64_t track_id) const;

    /// The musical key of a track, in the notation the device records.
    ///
    /// `track::key()` gives the key as one of the twenty-four this library
    /// knows, which loses both the notation rekordbox wrote and any key whose
    /// notation is not recognised.  This gives back what the device holds,
    /// such as `F#m` under the classic setting or `8A` under the Camelot one,
    /// for a caller that would rather read it itself.
    ///
    /// \param track_id Identifier of the track, as `track::id()` gives it.
    /// \return Returns the notation, or no value if the track is not there or
    ///         carries no key.
    [[nodiscard]] std::optional<std::string> key_name(int64_t track_id) const;

    /// The colour a track is marked with, as rekordbox enumerates them.
    ///
    /// The colours are the eight that rekordbox offers, numbered as the
    /// `export.pdb` library beside the database numbers them: one for pink,
    /// two red, three orange, four yellow, five green, six aqua, seven blue
    /// and eight purple.
    ///
    /// \param track_id Identifier of the track, as `track::id()` gives it.
    /// \return Returns the colour, or no value if the track is not there or
    ///         is not marked with one.
    [[nodiscard]] std::optional<int> color_id(int64_t track_id) const;

private:
    std::shared_ptr<onelibrary_context> context_;
};

}  // namespace djinterop::onelibrary

#endif  // DJINTEROP_ONELIBRARY_ONELIBRARY_HPP
