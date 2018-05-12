/*
    This file is part of libengineprime.

    libengineprime is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libengineprime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libengineprime.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engineprime/track.hpp>

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <boost/optional.hpp>

#define _MODERN_SQLITE_BOOST_OPTIONAL_SUPPORT
#include "sqlite_modern_cpp.h"

namespace engineprime {

struct track_row
{
    int play_order;
    int length;
    int length_calculated;
    int bpm;
    int year;
    std::string path;
    std::string filename;
    int bitrate;
    double bpm_analysed;
    int track_type;
    double is_external_track;
    boost::optional<std::string> uuid_of_external_database;
    boost::optional<int> id_track_in_external_database;
    int id_album_art;
    int pdb_import_key; // schema versions >= 1.0.3
};

struct metadata_types
{
    static const int title          = 1;
    static const int artist         = 2;
    static const int album          = 3;
    static const int genre          = 4;
    static const int comment        = 5;
    static const int publisher      = 6;
    static const int composer       = 7;
    static const int duration_mm_ss = 10;
    static const int ever_played    = 12;
    static const int file_extension = 13;
};

struct metadata_row
{
    bool persist_nulls = true;
    int id;
    int type;
    boost::optional<std::string> text;
};

typedef std::vector<metadata_row> str_metadata_vec;

struct metadata_integer_types
{
    static const int last_played_ts   = 1;
    static const int last_modified_ts = 2;
    static const int last_accessed_ts = 3; // Note truncated to date on VFAT (see FAT "ACCDATE")
    static const int musical_key      = 4;
    static const int hash             = 10;
};

struct metadata_integer_row
{
    bool persist_nulls = true;
    int id;
    int type;
    boost::optional<int> value;
};

typedef std::vector<metadata_integer_row> int_metadata_vec;


// Select a row from the Track table
track_row select_track_row(const database &db, int id)
{
    sqlite::database m_db{db.music_db_path()};
    track_row row;
    int rows_found = 0;
    if (db.version() >= version_firmware_1_0_3)
    {
        m_db
            << "SELECT id, playOrder, length, lengthCalculated, bpm, year, "
               "path, filename, bitrate, bpmAnalyzed, trackType, "
               "isExternalTrack, uuidOfExternalDatabase, "
               "idTrackInExternalDatabase, idAlbumArt, pdbImportKey "
               "FROM Track WHERE id = :1"
            << id
            >> [&](int id, int play_order, int length, int length_calculated,
                  int bpm, int year, std::string path, std::string filename,
                  int bitrate, double bpm_analysed, int track_type,
                  double is_external_track, std::string uuid_of_external_database,
                  int id_track_in_external_database, int id_album_art,
                  int pdb_import_key)
            {
                row = track_row{
                    play_order, length, length_calculated, bpm, year, path,
                    filename, bitrate, bpm_analysed, track_type, is_external_track,
                    uuid_of_external_database, id_track_in_external_database,
                    id_album_art, pdb_import_key};
                ++rows_found;
            };
    }
    else
    {
        m_db
            << "SELECT id, playOrder, length, lengthCalculated, bpm, year, "
               "path, filename, bitrate, bpmAnalyzed, trackType, "
               "isExternalTrack, uuidOfExternalDatabase, "
               "idTrackInExternalDatabase, idAlbumArt "
               "FROM Track WHERE id = :1"
            << id
            >> [&](int id, int play_order, int length, int length_calculated,
                  int bpm, int year, std::string path, std::string filename,
                  int bitrate, double bpm_analysed, int track_type,
                  double is_external_track, std::string uuid_of_external_database,
                  int id_track_in_external_database, int id_album_art)
            {
                row = track_row{
                    play_order, length, length_calculated, bpm, year, path,
                    filename, bitrate, bpm_analysed, track_type, is_external_track,
                    uuid_of_external_database, id_track_in_external_database,
                    id_album_art, 0};
                ++rows_found;
            };
    }

    if (rows_found == 0)
        throw nonexistent_track{id};

    return row;
}

str_metadata_vec select_metadata_rows(const std::string &music_db_path, int id)
{
    sqlite::database m_db{music_db_path};
    str_metadata_vec results{17};
    m_db
        << "SELECT id, type, text FROM Metadata WHERE id = ?"
        << id
        >> [&results](int id, int type, boost::optional<std::string> text)
        {
            if (type > 16)
                // Some new metadata that we don't know about yet!
                return;

            results[type].id = id;
            results[type].type = type;
            results[type].text = text;
        };

    return results;
}

int_metadata_vec select_int_metadata_rows(
        const std::string &music_db_path, int id)
{
    sqlite::database m_db{music_db_path};
    int_metadata_vec results{12};
    m_db
        << "SELECT id, type, value FROM MetadataInteger WHERE id = ?"
        << id
        >> [&results](int id, int type, boost::optional<int> value)
        {
            if (type > 11)
                // Some new metadata that we don't know about yet!
                return;

            results[type].id    = id;
            results[type].type  = type;
            results[type].value = value;
        };

    return results;
}

struct track::impl
{
    impl(const database &db, int id) :
        id_{id},
        load_db_uuid_{db.uuid()},
        track_row_{select_track_row(db, id)},
        str_metadata_vec_{select_metadata_rows(db.music_db_path(), id)},
        int_metadata_vec_{select_int_metadata_rows(db.music_db_path(), id)}
    {}

    impl(const track::impl &other) :
        id_{0},
        load_db_uuid_{},
        track_row_{other.track_row_},
        str_metadata_vec_{other.str_metadata_vec_},
        int_metadata_vec_{other.int_metadata_vec_}
    {}

    impl() :
        id_{0},
        str_metadata_vec_{17},
        int_metadata_vec_{12}
    {
        // Set defaults for all otherwise-uninitialised fields
        track_row_.play_order = 0;
        track_row_.length = 0;
        track_row_.length_calculated = 0;
        track_row_.bpm = 0;
        track_row_.year = 0;
        track_row_.bitrate = 0;
        track_row_.bpm_analysed = 0;
        track_row_.track_type = 1; // All tracks appear to be of type 1
        track_row_.is_external_track = 0;
        track_row_.id_album_art = 1; // 1 is the magic "no album art" value
        track_row_.pdb_import_key = 0;

        // Set defaults for all metadata
        int i = 0;
        for (auto &sm : str_metadata_vec_)
        {
            sm.id = 0;
            sm.type = i++;
        }
        i = 0;
        for (auto &im : int_metadata_vec_)
        {
            im.id = 0;
            im.type = i++;
        }

        // Metadata type-specific defaults and overrides
        str_metadata_vec_[0].persist_nulls = false;
        str_metadata_vec_[11].persist_nulls = false;
        str_metadata_vec_[14].persist_nulls = false;
        str_metadata_vec_[15].text = "1";
        str_metadata_vec_[16].text = "1";
        int_metadata_vec_[0].persist_nulls = false;
        int_metadata_vec_[5].value = 0;
        int_metadata_vec_[11].value = 1;
    }
    
    int id_;
    std::string load_db_uuid_;

    track_row track_row_;
    str_metadata_vec str_metadata_vec_;
    int_metadata_vec int_metadata_vec_;
};


track::track(const database &database, int id) : pimpl_{new impl{database, id}}
{}

track::track(const track &other) : pimpl_{new impl{*other.pimpl_}}
{}

track::track() : pimpl_{new impl{}}
{}

track::~track() = default;

track &track::operator =(const track &other)
{
    if (this != &other)
        pimpl_.reset(new impl{*other.pimpl_});
    return *this;
}

int track::id() const
{
    return pimpl_->id_;
}

int track::track_number() const
{
    return pimpl_->track_row_.play_order;
}

std::chrono::seconds track::duration() const
{
    return std::chrono::seconds{pimpl_->track_row_.length};
}

int track::bpm() const
{
    return pimpl_->track_row_.bpm;
}

int track::year() const
{
    return pimpl_->track_row_.year;
}

std::string track::title() const
{
    return pimpl_->str_metadata_vec_[metadata_types::title].text.value_or("");
}

std::string track::artist() const
{
    return pimpl_->str_metadata_vec_[metadata_types::artist].text.value_or("");
}

std::string track::album() const
{
    return pimpl_->str_metadata_vec_[metadata_types::album].text.value_or("");
}

std::string track::genre() const
{
    return pimpl_->str_metadata_vec_[metadata_types::genre].text.value_or("");
}

std::string track::comment() const
{
    return pimpl_->str_metadata_vec_[metadata_types::comment].text.value_or("");
}

std::string track::publisher() const
{
    return pimpl_->str_metadata_vec_[metadata_types::publisher].text.value_or("");
}

std::string track::composer() const
{
    return pimpl_->str_metadata_vec_[metadata_types::composer].text.value_or("");
}

musical_key track::key() const
{
    auto key_int_value = pimpl_->int_metadata_vec_[
        metadata_integer_types::musical_key].value.value_or(1);
    return (musical_key)key_int_value;
}

std::string track::path() const
{
    return pimpl_->track_row_.path;
}

std::string track::filename() const
{
    return pimpl_->track_row_.filename;
}

std::string track::file_extension() const
{
    return pimpl_->str_metadata_vec_[metadata_types::file_extension]
        .text
        .value_or("");
}

std::chrono::system_clock::time_point track::last_modified_at() const
{
    return std::chrono::system_clock::time_point{
        std::chrono::seconds{
            pimpl_->int_metadata_vec_[metadata_integer_types::last_modified_ts]
                .value
                .value_or(0)}};
}

int track::bitrate() const
{
    return pimpl_->track_row_.bitrate;
}

bool track::ever_played() const
{
    auto &entry = pimpl_->str_metadata_vec_[metadata_types::ever_played].text;
    return entry && entry.value() == "1";
}

std::chrono::system_clock::time_point track::last_played_at() const
{
    return std::chrono::system_clock::time_point{
        std::chrono::seconds{
            pimpl_->int_metadata_vec_[metadata_integer_types::last_played_ts]
                .value
                .value_or(0)}};
}
std::chrono::system_clock::time_point track::last_accessed_at() const
{
    return std::chrono::system_clock::time_point{
        std::chrono::seconds{
            pimpl_->int_metadata_vec_[metadata_integer_types::last_accessed_ts]
                .value
                .value_or(0)}};
}
bool track::imported() const
{
    return pimpl_->track_row_.is_external_track != 0;
}

std::string track::external_database_uuid() const
{
    return pimpl_->track_row_.uuid_of_external_database.value_or("");
}

int track::track_id_in_external_database() const
{
    return pimpl_->track_row_.id_track_in_external_database.value_or(0);
}

int track::album_art_id() const
{
    return pimpl_->track_row_.id_album_art;
}

bool track::has_title() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::title].text;
}

bool track::has_artist() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::artist].text;
}

bool track::has_album() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::album].text;
}

bool track::has_genre() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::genre].text;
}

bool track::has_comment() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::comment].text;
}

bool track::has_publisher() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::publisher].text;
}

bool track::has_composer() const
{
    return (bool)pimpl_->str_metadata_vec_[metadata_types::composer].text;
}

bool track::has_key() const
{
    return (bool)pimpl_->int_metadata_vec_[metadata_integer_types::musical_key].value;
}

void track::set_track_number(int track_number)
{
    pimpl_->track_row_.play_order = track_number;
}

void track::set_duration(std::chrono::seconds duration)
{
    pimpl_->track_row_.length = duration.count();
    pimpl_->track_row_.length_calculated = duration.count();

    // Set string metadata, type 10, as "MM:SS";
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0');
    ss << (duration.count() / 60);
    ss << ":";
    ss << (duration.count() % 60);
    pimpl_->str_metadata_vec_[metadata_types::duration_mm_ss].text = ss.str();
}

void track::set_bpm(int bpm)
{
    pimpl_->track_row_.bpm = bpm;
    pimpl_->track_row_.bpm_analysed = bpm;
}

void track::set_year(int year)
{
    pimpl_->track_row_.year = year;
}

void track::set_title(const std::string &title)
{
    pimpl_->str_metadata_vec_[metadata_types::title].text =
        title != ""
        ? boost::optional<std::string>(title)
        : boost::none;
}

void track::set_artist(const std::string &artist)
{
    pimpl_->str_metadata_vec_[metadata_types::artist].text =
        artist != ""
        ? boost::optional<std::string>(artist)
        : boost::none;
}

void track::set_album(const std::string &album)
{
    pimpl_->str_metadata_vec_[metadata_types::album].text =
        album != ""
        ? boost::optional<std::string>(album)
        : boost::none;
}

void track::set_genre(const std::string &genre)
{
    pimpl_->str_metadata_vec_[metadata_types::genre].text =
        genre != ""
        ? boost::optional<std::string>(genre)
        : boost::none;
}

void track::set_comment(const std::string &comment)
{
    pimpl_->str_metadata_vec_[metadata_types::comment].text =
        comment != ""
        ? boost::optional<std::string>(comment)
        : boost::none;
}

void track::set_publisher(const std::string &publisher)
{
    pimpl_->str_metadata_vec_[metadata_types::publisher].text =
        publisher != ""
        ? boost::optional<std::string>(publisher)
        : boost::none;
}

void track::set_composer(const std::string &composer)
{
    pimpl_->str_metadata_vec_[metadata_types::composer].text =
        composer != ""
        ? boost::optional<std::string>(composer)
        : boost::none;
}

void track::set_key(musical_key key)
{
    pimpl_->int_metadata_vec_[
        metadata_integer_types::musical_key].value = (int)key;
}

void track::set_path(const std::string &path)
{
    pimpl_->track_row_.path = path;
}

void track::set_filename(const std::string &filename)
{
    pimpl_->track_row_.filename = filename;
}

void track::set_file_extension(const std::string &file_extension)
{
    pimpl_->str_metadata_vec_[metadata_types::file_extension].text = file_extension;
}

void track::set_last_modified_at(std::chrono::system_clock::time_point last_modified_at)
{
    pimpl_->int_metadata_vec_[metadata_integer_types::last_modified_ts].value =
        last_modified_at.time_since_epoch().count() != 0
        ? boost::optional<int>{
            last_modified_at.time_since_epoch().count() *
            std::chrono::system_clock::period::num /
            std::chrono::system_clock::period::den}
        : boost::none;
}

void track::set_bitrate(int bitrate)
{
    pimpl_->track_row_.bitrate = bitrate;
}

void track::set_ever_played(bool ever_played)
{
    pimpl_->str_metadata_vec_[metadata_types::ever_played].text = ever_played
        ? boost::optional<std::string>{"1"}
        : boost::none;
}

void track::set_last_played_at(std::chrono::system_clock::time_point last_played_at)
{
    pimpl_->int_metadata_vec_[metadata_integer_types::last_played_ts].value =
        last_played_at.time_since_epoch().count() != 0
        ? boost::optional<int>{
            last_played_at.time_since_epoch().count() *
            std::chrono::system_clock::period::num /
            std::chrono::system_clock::period::den}
        : boost::none;
}

void track::set_last_accessed_at(std::chrono::system_clock::time_point last_accessed_at)
{
    // Field is always truncated to the midnight at the end of the day the
    // track is played, it seems.
    auto secs = last_accessed_at.time_since_epoch().count() *
        std::chrono::system_clock::period::num /
        std::chrono::system_clock::period::den;
    if (secs % 86400 != 0)
        secs = secs + 86400 - (secs % 86400);
    last_accessed_at = std::chrono::system_clock::time_point{
        std::chrono::seconds{secs}};

    pimpl_->int_metadata_vec_[metadata_integer_types::last_accessed_ts].value =
        last_accessed_at.time_since_epoch().count() != 0
        ? boost::optional<int>{
            last_accessed_at.time_since_epoch().count() *
            std::chrono::system_clock::period::num /
            std::chrono::system_clock::period::den}
        : boost::none;
}

void track::set_imported(bool imported)
{
    pimpl_->track_row_.is_external_track = imported ? 1 : 0;
    if (!imported)
    {
        pimpl_->track_row_.uuid_of_external_database = boost::none;
        pimpl_->track_row_.id_track_in_external_database = boost::none;
    }
}

void track::set_imported(
        bool imported,
        const std::string &external_database_uuid,
        int track_id_in_external_database)
{
    pimpl_->track_row_.is_external_track = imported ? 1 : 0;
    pimpl_->track_row_.uuid_of_external_database = external_database_uuid;
    pimpl_->track_row_.id_track_in_external_database = track_id_in_external_database;
}

void track::set_album_art_id(int album_art_id)
{
    pimpl_->track_row_.id_album_art = album_art_id;
}

void track::set_no_album_art()
{
    // 1 is the magic number for "no album art", and it is a valid entry in the
    // AlbumArt table with no image data
    pimpl_->track_row_.id_album_art = 1;
}


/**
 * Save a track to a given database
 *
 * If the track came from the supplied database originally, it is updated
 * in-place.  If the track does not already exist in the supplied database,
 * then it will be saved as a new track there.  The `id()` method will
 * return a new value after a new track is saved.
 */
void track::save(const database &database)
{
    // Check mandatory fields
    if (path() == "")
        throw std::invalid_argument{"Path must be populated"};
    if (filename() == "")
        throw std::invalid_argument{"Filename must be populated"};

    // Work out if we are creating a new entry or not
    auto new_entry = id() == 0 || pimpl_->load_db_uuid_ != database.uuid();

    // Do all DB writing in a transaction
    sqlite::database m_db{database.music_db_path()};
    m_db << "BEGIN";

    // Insert/update the Track table
    if (new_entry)
    {
        // Insert a new entry in the track table
        if (database.version() >= version_firmware_1_0_3)
        {
            m_db <<
                "INSERT INTO Track ("
                "  playOrder, length, lengthCalculated, bpm, "
                "  year, path, filename, bitrate, bpmAnalyzed, trackType, "
                "  isExternalTrack, uuidOfExternalDatabase, "
                "  idTrackInExternalDatabase, idAlbumArt, pdbImportKey"
                ")"
                "VALUES ("
                "  ?,?,?,?,?,?,?,?,?,?,?,?,?,?,?"
                ")"
                << pimpl_->track_row_.play_order
                << pimpl_->track_row_.length
                << pimpl_->track_row_.length_calculated
                << pimpl_->track_row_.bpm
                << pimpl_->track_row_.year
                << pimpl_->track_row_.path
                << pimpl_->track_row_.filename
                << pimpl_->track_row_.bitrate
                << pimpl_->track_row_.bpm_analysed
                << pimpl_->track_row_.track_type
                << pimpl_->track_row_.is_external_track
                << pimpl_->track_row_.uuid_of_external_database
                << pimpl_->track_row_.id_track_in_external_database
                << pimpl_->track_row_.id_album_art
                << pimpl_->track_row_.pdb_import_key;
        }
        else
        {
            m_db <<
                "INSERT INTO Track ("
                "  playOrder, length, lengthCalculated, bpm, "
                "  year, path, filename, bitrate, bpmAnalyzed, trackType, "
                "  isExternalTrack, uuidOfExternalDatabase, "
                "  idTrackInExternalDatabase, idAlbumArt"
                ")"
                "VALUES ("
                "  ?,?,?,?,?,?,?,?,?,?,?,?,?,?"
                ")"
                << pimpl_->track_row_.play_order
                << pimpl_->track_row_.length
                << pimpl_->track_row_.length_calculated
                << pimpl_->track_row_.bpm
                << pimpl_->track_row_.year
                << pimpl_->track_row_.path
                << pimpl_->track_row_.filename
                << pimpl_->track_row_.bitrate
                << pimpl_->track_row_.bpm_analysed
                << pimpl_->track_row_.track_type
                << pimpl_->track_row_.is_external_track
                << pimpl_->track_row_.uuid_of_external_database
                << pimpl_->track_row_.id_track_in_external_database
                << pimpl_->track_row_.id_album_art;
        }
        pimpl_->id_ = m_db.last_insert_rowid();
    }
    else
    {
        // Update existing entry
        if (database.version() >= version_firmware_1_0_3)
        {
            m_db <<
                "UPDATE Track SET "
                "  playOrder = ?, "
                "  length = ?, "
                "  lengthCalculated = ?, "
                "  bpm = ?, "
                "  year = ?, "
                "  path = ?, "
                "  filename = ?, "
                "  bitrate = ?, "
                "  bpmAnalyzed = ?, "
                "  trackType = ?, "
                "  isExternalTrack = ?, "
                "  uuidOfExternalDatabase = ?, "
                "  idTrackInExternalDatabase = ?, "
                "  idAlbumArt = ?, "
                "  pdbImportKey = ? "
                "WHERE id = ?"
                << pimpl_->track_row_.play_order
                << pimpl_->track_row_.length
                << pimpl_->track_row_.length_calculated
                << pimpl_->track_row_.bpm
                << pimpl_->track_row_.year
                << pimpl_->track_row_.path
                << pimpl_->track_row_.filename
                << pimpl_->track_row_.bitrate
                << pimpl_->track_row_.bpm_analysed
                << pimpl_->track_row_.track_type
                << pimpl_->track_row_.is_external_track
                << pimpl_->track_row_.uuid_of_external_database
                << pimpl_->track_row_.id_track_in_external_database
                << pimpl_->track_row_.id_album_art
                << pimpl_->track_row_.pdb_import_key
                << pimpl_->id_;
        }
        else
        {
            m_db <<
                "UPDATE Track SET "
                "  playOrder = ?, "
                "  length = ?, "
                "  lengthCalculated = ?, "
                "  bpm = ?, "
                "  year = ?, "
                "  path = ?, "
                "  filename = ?, "
                "  bitrate = ?, "
                "  bpmAnalyzed = ?, "
                "  trackType = ?, "
                "  isExternalTrack = ?, "
                "  uuidOfExternalDatabase = ?, "
                "  idTrackInExternalDatabase = ?, "
                "  idAlbumArt = ? "
                "WHERE id = ?"
                << pimpl_->track_row_.play_order
                << pimpl_->track_row_.length
                << pimpl_->track_row_.length_calculated
                << pimpl_->track_row_.bpm
                << pimpl_->track_row_.year
                << pimpl_->track_row_.path
                << pimpl_->track_row_.filename
                << pimpl_->track_row_.bitrate
                << pimpl_->track_row_.bpm_analysed
                << pimpl_->track_row_.track_type
                << pimpl_->track_row_.is_external_track
                << pimpl_->track_row_.uuid_of_external_database
                << pimpl_->track_row_.id_track_in_external_database
                << pimpl_->track_row_.id_album_art
                << pimpl_->id_;
        }
    }

    // Insert/update the Metadata table
    for (auto &str_metadata : pimpl_->str_metadata_vec_)
    {
        if (str_metadata.persist_nulls || str_metadata.text)
        {
            str_metadata.id = pimpl_->id_;
            m_db <<
                "INSERT OR REPLACE INTO MetaData (id, type, text) "
                "VALUES (?, ?, ?)"
                << str_metadata.id
                << str_metadata.type
                << str_metadata.text;
        }
    }

    // Insert/update the MetadataInteger table
    for (auto &int_metadata : pimpl_->int_metadata_vec_)
    {
        if (int_metadata.persist_nulls || int_metadata.value)
        {
            int_metadata.id = pimpl_->id_;
            m_db <<
                "INSERT OR REPLACE INTO MetaDataInteger (id, type, value) "
                "VALUES (?, ?, ?)"
                << int_metadata.id
                << int_metadata.type
                << int_metadata.value;
        }
    }

    m_db << "COMMIT";
    pimpl_->load_db_uuid_ = database.uuid();
}


std::vector<int> all_track_ids(const database &database)
{
    sqlite::database m_db{database.music_db_path()};

    std::vector<int> ids;
    m_db
        << "SELECT id FROM Track ORDER BY id"
        >> [&ids](int id) { ids.push_back(id); };

    return ids;
}

} // namespace engineprime
