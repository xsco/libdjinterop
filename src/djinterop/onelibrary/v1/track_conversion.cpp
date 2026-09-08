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

#include "track_conversion.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
namespace djinterop::onelibrary::v1
{
namespace
{
/// The offset in semitones of a note letter above C, if it is one.
constexpr std::optional<int> semitones_above_c(char note)
{
    switch (note)
    {
        case 'C': return 0;
        case 'D': return 2;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 7;
        case 'A': return 9;
        case 'B': return 11;
        default: return std::nullopt;
    }
}

/// Major keys, indexed by semitones above C.
constexpr musical_key major_keys[12] = {
    musical_key::c_major,       musical_key::d_flat_major,
    musical_key::d_major,       musical_key::e_flat_major,
    musical_key::e_major,       musical_key::f_major,
    musical_key::f_sharp_major, musical_key::g_major,
    musical_key::a_flat_major,  musical_key::a_major,
    musical_key::b_flat_major,  musical_key::b_major};

/// Minor keys, indexed by semitones above C.
constexpr musical_key minor_keys[12] = {
    musical_key::c_minor,       musical_key::d_flat_minor,
    musical_key::d_minor,       musical_key::e_flat_minor,
    musical_key::e_minor,       musical_key::f_minor,
    musical_key::f_sharp_minor, musical_key::g_minor,
    musical_key::a_flat_minor,  musical_key::a_minor,
    musical_key::b_flat_minor,  musical_key::b_minor};

}  // anonymous namespace

std::optional<musical_key> parse_musical_key(const std::string& name)
{
    // Notation is a note letter, an optional accidental, and an optional `m`
    // for a minor key: `C`, `F#m`, `Bb`.  Both the ASCII and the typographic
    // accidentals are accepted.
    size_t position = 0;
    if (position >= name.size())
        return std::nullopt;

    const auto note = semitones_above_c(
        static_cast<char>(
            std::toupper(static_cast<unsigned char>(name[position]))));
    if (!note)
        return std::nullopt;

    auto semitone = *note;
    ++position;

    // Step over an accidental if one is next.  The ASCII and typographic
    // spellings mean the same thing and differ only in how many bytes they
    // occupy, which `strlen` rather than a literal count keeps right.
    const auto consume = [&](const char* accidental)
    {
        const auto length = std::strlen(accidental);
        if (name.compare(position, length, accidental) != 0)
            return false;

        position += length;
        return true;
    };

    if (consume("#") || consume("♯"))  // MUSIC SHARP SIGN
        semitone += 1;
    else if (consume("b") || consume("♭"))  // MUSIC FLAT SIGN
        semitone -= 1;

    semitone = ((semitone % 12) + 12) % 12;

    const auto remainder = name.substr(position);
    if (remainder.empty())
        return major_keys[semitone];

    if (remainder == "m" || remainder == "M")
        return minor_keys[semitone];

    // Anything else is a notation this library does not know, such as the
    // Camelot or Open Key wheels.
    return std::nullopt;
}

track_snapshot to_snapshot(const content_row& row)
{
    track_snapshot snapshot;

    snapshot.title = row.title;
    snapshot.artist = row.artist;
    snapshot.composer = row.composer;
    snapshot.album = row.album;
    snapshot.genre = row.genre;
    snapshot.publisher = row.label;
    snapshot.comment = row.comment;

    if (row.bpm_x100.has_value() && *row.bpm_x100 > 0)
        snapshot.bpm = static_cast<double>(*row.bpm_x100) / 100;

    if (row.length.has_value() && row.length->count() > 0)
        snapshot.duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(*row.length);

    if (row.track_number.has_value() && *row.track_number > 0)
        snapshot.track_number = static_cast<int>(*row.track_number);

    if (row.release_year.has_value() && *row.release_year > 0)
        snapshot.year = static_cast<int>(*row.release_year);

    // djinterop rates a track from zero to one hundred, where rekordbox uses
    // whole stars.
    if (row.rating_stars.has_value())
        snapshot.rating =
            static_cast<int>(std::clamp<int64_t>(*row.rating_stars, 0, 5) * 20);

    if (row.path.has_value())
    {
        // Paths are absolute within the device, whereas djinterop wants them
        // relative to the directory of the database.
        const auto& path = *row.path;
        snapshot.relative_path = path.front() == '/' ? path.substr(1) : path;
    }

    if (row.file_size.has_value() && *row.file_size > 0)
        snapshot.file_bytes = static_cast<unsigned long long>(*row.file_size);

    if (row.bitrate.has_value() && *row.bitrate > 0)
        snapshot.bitrate = static_cast<int>(*row.bitrate);

    if (row.sampling_rate.has_value() && *row.sampling_rate > 0)
    {
        snapshot.sample_rate = static_cast<double>(*row.sampling_rate);

        // The database records a duration in whole seconds and no sample
        // count, so the count can only be recovered to that precision.
        if (row.length.has_value() && row.length->count() > 0)
            snapshot.sample_count = static_cast<unsigned long long>(
                row.length->count() * *row.sampling_rate);
    }

    if (row.key.has_value())
        snapshot.key = parse_musical_key(*row.key);

    // Beatgrids, waveforms, hot cues and loops are not in the database:
    // rekordbox leaves them in the ANLZ files that `analysisDataFilePath`
    // points at, and exports an empty `cue` table.
    return snapshot;
}

}  // namespace djinterop::onelibrary::v1
