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

#include <stdexcept>
#include <string>

namespace djinterop::onelibrary
{
struct onelibrary_error : public std::runtime_error
{
    explicit onelibrary_error(const std::string& what_arg) noexcept :
        runtime_error{what_arg}
    {
    }
};

extern const char* const schema_db_version;

void create(void* db_handle);
void verify(void* db_handle);
void seed(void* db_handle);

extern const char* ddl_content;
extern const char* ddl_artist;
extern const char* ddl_album;
extern const char* ddl_genre;
extern const char* ddl_label;
extern const char* ddl_key;
extern const char* ddl_color;
extern const char* ddl_image;
extern const char* ddl_cue;
extern const char* ddl_playlist;
extern const char* ddl_playlist_content;
extern const char* ddl_history;
extern const char* ddl_history_content;
extern const char* ddl_hotCueBankList;
extern const char* ddl_hotCueBankList_cue;
extern const char* ddl_myTag;
extern const char* ddl_myTag_content;
extern const char* ddl_menuItem;
extern const char* ddl_category;
extern const char* ddl_sort;
extern const char* ddl_property;
extern const char* ddl_recommendedLike;

extern const char* idx_playlist_content_playlist_id;
extern const char* idx_myTag_content_content_id;
extern const char* idx_myTag_content_myTag_id;
extern const char* idx_hotCueBankList_cue_hotCueBankList_id;

}  // namespace djinterop::onelibrary
