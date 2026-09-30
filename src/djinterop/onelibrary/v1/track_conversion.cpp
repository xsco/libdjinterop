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
#include <cctype>
#include <cstring>
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
    if (name.empty())
        return std::nullopt;

    const auto note = semitones_above_c(
        static_cast<char>(std::toupper(static_cast<unsigned char>(name[0]))));
    if (!note)
        return std::nullopt;

    auto semitone = *note;
    size_t position = 1;

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

std::optional<double> to_bpm(std::optional<int64_t> bpm_x100)
{
    if (!bpm_x100 || *bpm_x100 <= 0)
        return std::nullopt;

    return static_cast<double>(*bpm_x100) / 100;
}

std::optional<std::chrono::milliseconds> to_duration(
    std::optional<std::chrono::seconds> length)
{
    if (!length || length->count() <= 0)
        return std::nullopt;

    return std::chrono::duration_cast<std::chrono::milliseconds>(*length);
}

std::optional<int> to_positive_int(std::optional<int64_t> value)
{
    if (!value || *value <= 0)
        return std::nullopt;

    return static_cast<int>(*value);
}

std::optional<int> to_rating(std::optional<int64_t> stars)
{
    if (!stars)
        return std::nullopt;

    // djinterop rates a track from zero to one hundred, where rekordbox uses
    // whole stars.
    return static_cast<int>(std::clamp<int64_t>(*stars, 0, 5) * 20);
}

std::string to_relative_path(const std::string& path)
{
    return !path.empty() && path.front() == '/' ? path.substr(1) : path;
}

std::string to_device_path(const std::string& relative_path)
{
    return "/" + to_relative_path(relative_path);
}

std::optional<double> to_sample_rate(std::optional<int64_t> sampling_rate)
{
    if (!sampling_rate || *sampling_rate <= 0)
        return std::nullopt;

    return static_cast<double>(*sampling_rate);
}

std::optional<unsigned long long> to_sample_count(
    std::optional<std::chrono::seconds> length,
    std::optional<int64_t> sampling_rate)
{
    // The database records a duration in whole seconds and no sample count,
    // so the count can only be recovered to that precision.
    if (!length || length->count() <= 0 || !sampling_rate ||
        *sampling_rate <= 0)
        return std::nullopt;

    return static_cast<unsigned long long>(length->count() * *sampling_rate);
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
    snapshot.bpm = to_bpm(row.bpm_x100);
    snapshot.duration = to_duration(row.length);
    snapshot.track_number = to_positive_int(row.track_number);
    snapshot.year = to_positive_int(row.release_year);
    snapshot.rating = to_rating(row.rating_stars);
    snapshot.bitrate = to_positive_int(row.bitrate);
    snapshot.sample_rate = to_sample_rate(row.sampling_rate);
    snapshot.sample_count = to_sample_count(row.length, row.sampling_rate);

    if (row.path)
        snapshot.relative_path = to_relative_path(*row.path);

    if (row.file_size && *row.file_size > 0)
        snapshot.file_bytes = static_cast<unsigned long long>(*row.file_size);

    if (row.key)
        snapshot.key = parse_musical_key(*row.key);

    return snapshot;
}

}  // namespace djinterop::onelibrary::v1
