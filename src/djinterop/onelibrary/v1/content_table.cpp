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

#include <djinterop/onelibrary/v1/content_table.hpp>

#include <string>
#include <utility>

#include "../../util/sqlite_query.hpp"
#include "../onelibrary_context.hpp"

namespace djinterop::onelibrary::v1
{
namespace
{
/// How each lookup table is joined to `content`.
///
/// Nothing enforces that a reference resolves, so each is joined outwards.
/// `key` is quoted throughout, as it is also a SQL keyword.
constexpr const char* artist_join =
    "artist ON artist.artist_id = c.artist_id_artist";
constexpr const char* composer_join =
    "artist AS composer ON composer.artist_id = c.artist_id_composer";
constexpr const char* album_join = "album ON album.album_id = c.album_id";
constexpr const char* genre_join = "genre ON genre.genre_id = c.genre_id";
constexpr const char* label_join = "label ON label.label_id = c.label_id";
constexpr const char* key_join = "\"key\" ON \"key\".key_id = c.key_id";

/// Every column the row structure needs, in the order it reads them back.
constexpr const char* row_columns =
    "c.content_id, c.title, artist.name, composer.name, album.name, "
    "genre.name, label.name, \"key\".name, c.djComment, c.bpmx100, c.length, "
    "c.trackNo, c.releaseYear, c.rating, c.path, c.fileSize, "
    "c.bitrate, c.samplingRate, c.analysisDataFilePath, c.color_id";

std::string row_query()
{
    return std::string{"SELECT "} + row_columns + " FROM content AS c " +
           "LEFT JOIN " + artist_join + " LEFT JOIN " + composer_join +
           " LEFT JOIN " + album_join + " LEFT JOIN " + genre_join +
           " LEFT JOIN " + label_join + " LEFT JOIN " + key_join +
           " WHERE c.content_id = ?";
}

/// A query for one column of `content`.
std::string content_column(const std::string& expression)
{
    return "SELECT " + expression + " FROM content AS c WHERE c.content_id = ?";
}

/// A query for the name a lookup table holds.
std::string lookup_column(const std::string& alias, const std::string& join)
{
    return "SELECT " + alias + ".name FROM content AS c LEFT JOIN " + join +
           " WHERE c.content_id = ?";
}

/// Read one column, if the row is there.
template <typename T>
std::optional<T> read_column(
    onelibrary_context& context, const std::string& sql, int64_t id)
{
    std::optional<T> result;
    context.db << sql << id >> [&](std::optional<T> value)
    { result = std::move(value); };

    return result;
}

/// Treat a column that is present but empty as absent: rekordbox writes an
/// empty string for metadata a track does not carry.
std::optional<std::string> non_empty(std::optional<std::string> value)
{
    if (value.has_value() && value->empty())
        return std::nullopt;

    return value;
}

/// Read one column of text, which is absent when it is empty.
std::optional<std::string> read_text(
    onelibrary_context& context, const std::string& sql, int64_t id)
{
    return non_empty(read_column<std::string>(context, sql, id));
}

}  // anonymous namespace

content_table::content_table(std::shared_ptr<onelibrary_context> context) :
    context_{std::move(context)}
{
}

std::optional<content_row> content_table::get(int64_t id) const
{
    std::optional<content_row> result;

    // The parameter list has to match `row_columns` exactly.
    context_->db << row_query() << id >>
        [&](int64_t row_id, std::optional<std::string> title,
            std::optional<std::string> artist,
            std::optional<std::string> composer,
            std::optional<std::string> album, std::optional<std::string> genre,
            std::optional<std::string> label, std::optional<std::string> key,
            std::optional<std::string> comment, std::optional<int64_t> bpm_x100,
            std::optional<int64_t> length, std::optional<int64_t> track_number,
            std::optional<int64_t> release_year,
            std::optional<int64_t> rating_stars,
            std::optional<std::string> path, std::optional<int64_t> file_size,
            std::optional<int64_t> bitrate,
            std::optional<int64_t> sampling_rate,
            std::optional<std::string> analysis_path,
            std::optional<int64_t> color_id)
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
        if (length)
            row.length = std::chrono::seconds{*length};
        row.track_number = track_number;
        row.release_year = release_year;
        row.rating_stars = rating_stars;
        row.path = non_empty(std::move(path));
        row.file_size = file_size;
        row.bitrate = bitrate;
        row.sampling_rate = sampling_rate;
        row.analysis_path = non_empty(std::move(analysis_path));
        row.color_id = color_id;
        result = std::move(row);
    };

    return result;
}

std::vector<int64_t> content_table::all_ids() const
{
    return util::collect_ids(
        context_->db, "SELECT content_id FROM content ORDER BY content_id");
}

std::vector<int64_t> content_table::ids_by_path(const std::string& path) const
{
    const auto qualified =
        !path.empty() && path.front() == '/' ? path : "/" + path;

    return util::collect_ids(
        context_->db,
        "SELECT content_id FROM content WHERE path = ? ORDER BY content_id",
        qualified);
}

bool content_table::exists(int64_t id) const
{
    return util::any_row(
        context_->db, "SELECT 1 FROM content WHERE content_id = ? LIMIT 1", id);
}

std::optional<std::string> content_table::get_title(int64_t id) const
{
    return read_text(*context_, content_column("c.title"), id);
}

std::optional<std::string> content_table::get_artist(int64_t id) const
{
    return read_text(*context_, lookup_column("artist", artist_join), id);
}

std::optional<std::string> content_table::get_composer(int64_t id) const
{
    return read_text(*context_, lookup_column("composer", composer_join), id);
}

std::optional<std::string> content_table::get_album(int64_t id) const
{
    return read_text(*context_, lookup_column("album", album_join), id);
}

std::optional<std::string> content_table::get_genre(int64_t id) const
{
    return read_text(*context_, lookup_column("genre", genre_join), id);
}

std::optional<std::string> content_table::get_label(int64_t id) const
{
    return read_text(*context_, lookup_column("label", label_join), id);
}

std::optional<std::string> content_table::get_key(int64_t id) const
{
    return read_text(*context_, lookup_column("\"key\"", key_join), id);
}

std::optional<std::string> content_table::get_comment(int64_t id) const
{
    return read_text(*context_, content_column("c.djComment"), id);
}

std::optional<int64_t> content_table::get_bpm_x100(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.bpmx100"), id);
}

std::optional<std::chrono::seconds> content_table::get_length(int64_t id) const
{
    const auto seconds =
        read_column<int64_t>(*context_, content_column("c.length"), id);
    if (!seconds)
        return std::nullopt;

    return std::chrono::seconds{*seconds};
}

std::optional<int64_t> content_table::get_track_number(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.trackNo"), id);
}

std::optional<int64_t> content_table::get_release_year(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.releaseYear"), id);
}

std::optional<int64_t> content_table::get_rating_stars(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.rating"), id);
}

std::optional<std::string> content_table::get_path(int64_t id) const
{
    return read_text(*context_, content_column("c.path"), id);
}

std::optional<int64_t> content_table::get_file_size(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.fileSize"), id);
}

std::optional<int64_t> content_table::get_bitrate(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.bitrate"), id);
}

std::optional<int64_t> content_table::get_sampling_rate(int64_t id) const
{
    return read_column<int64_t>(
        *context_, content_column("c.samplingRate"), id);
}

std::optional<std::string> content_table::get_analysis_path(int64_t id) const
{
    return read_text(*context_, content_column("c.analysisDataFilePath"), id);
}

std::optional<int64_t> content_table::get_color_id(int64_t id) const
{
    return read_column<int64_t>(*context_, content_column("c.color_id"), id);
}

}  // namespace djinterop::onelibrary::v1
