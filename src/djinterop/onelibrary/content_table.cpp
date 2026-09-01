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

#include "content_table.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <utility>

namespace djinterop::onelibrary
{
namespace
{
/// Every column the row structure needs, in the order it reads them back.
///
/// Nothing enforces that a lookup reference resolves, so each is joined
/// outwards.
constexpr const char* select_columns =
    "SELECT c.content_id, c.title, artist.name, composer.name, album.name, "
    "genre.name, label.name, \"key\".name, c.djComment, c.bpmx100, c.length, "
    "c.trackNo, c.releaseYear, c.rating, c.path, c.fileSize, "
    "c.bitrate, c.samplingRate "
    "FROM content AS c "
    "LEFT JOIN artist AS artist ON artist.artist_id = c.artist_id_artist "
    "LEFT JOIN artist AS composer "
    "ON composer.artist_id = c.artist_id_composer "
    "LEFT JOIN album ON album.album_id = c.album_id "
    "LEFT JOIN genre ON genre.genre_id = c.genre_id "
    "LEFT JOIN label ON label.label_id = c.label_id "
    // `key` is quoted throughout, as it is also a SQL keyword.
    "LEFT JOIN \"key\" ON \"key\".key_id = c.key_id ";

/// Treat a column that is present but empty as absent: rekordbox writes an
/// empty string for metadata a track does not carry.
std::optional<std::string> non_empty(std::optional<std::string> value)
{
    if (value.has_value() && value->empty())
        return std::nullopt;

    return value;
}

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

    const auto note = semitones_above_c(static_cast<char>(
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

content_table::content_table(std::shared_ptr<onelibrary_context> context) :
    context_{std::move(context)}
{
}

std::optional<content_row> content_table::get(int64_t id) const
{
    std::optional<content_row> result;

    // The parameter list has to match `select_columns` exactly.
    context_->db << (std::string{select_columns} + "WHERE c.content_id = ?")
                 << id >>
        [&](int64_t row_id, std::optional<std::string> title,
            std::optional<std::string> artist,
            std::optional<std::string> composer,
            std::optional<std::string> album, std::optional<std::string> genre,
            std::optional<std::string> label, std::optional<std::string> key,
            std::optional<std::string> comment, std::optional<int64_t> bpm_x100,
            std::optional<int64_t> length_seconds,
            std::optional<int64_t> track_number,
            std::optional<int64_t> release_year,
            std::optional<int64_t> rating_stars,
            std::optional<std::string> path, std::optional<int64_t> file_size,
            std::optional<int64_t> bitrate,
            std::optional<int64_t> sampling_rate)
    {
        content_row row;
        row.id = row_id;
        row.title = non_empty(std::move(title));
        row.artist = non_empty(std::move(artist));
        row.composer = non_empty(std::move(composer));
        row.album = non_empty(std::move(album));
        row.genre = non_empty(std::move(genre));
        row.label = non_empty(std::move(label));
        row.key = non_empty(std::move(key));
        row.comment = non_empty(std::move(comment));
        row.bpm_x100 = bpm_x100;
        row.length_seconds = length_seconds;
        row.track_number = track_number;
        row.release_year = release_year;
        row.rating_stars = rating_stars;
        row.path = non_empty(std::move(path));
        row.file_size = file_size;
        row.bitrate = bitrate;
        row.sampling_rate = sampling_rate;
        result = std::move(row);
    };

    return result;
}

std::vector<int64_t> content_table::all_ids() const
{
    return collect_ids(
        *context_, "SELECT content_id FROM content ORDER BY content_id");
}

std::vector<int64_t> content_table::ids_by_path(const std::string& path) const
{
    const auto qualified =
        !path.empty() && path.front() == '/' ? path : "/" + path;

    return collect_ids(
        *context_,
        "SELECT content_id FROM content WHERE path = ? ORDER BY content_id",
        qualified);
}

bool content_table::exists(int64_t id) const
{
    return any_row(
        *context_, "SELECT 1 FROM content WHERE content_id = ? LIMIT 1", id);
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

    if (row.length_seconds.has_value() && *row.length_seconds > 0)
        snapshot.duration =
            std::chrono::milliseconds{*row.length_seconds * 1000};

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
        if (row.length_seconds.has_value() && *row.length_seconds > 0)
            snapshot.sample_count = static_cast<unsigned long long>(
                *row.length_seconds * *row.sampling_rate);
    }

    if (row.key.has_value())
        snapshot.key = parse_musical_key(*row.key);

    // Beatgrids, waveforms, hot cues and loops are not in the database:
    // rekordbox leaves them in the ANLZ files that `analysisDataFilePath`
    // points at, and exports an empty `cue` table.
    return snapshot;
}

}  // namespace djinterop::onelibrary
