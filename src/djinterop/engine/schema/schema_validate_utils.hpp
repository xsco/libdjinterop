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

#include <set>
#include <string>

#include <sqlite_modern_cpp.h>

#include <djinterop/exceptions.hpp>
#include <utility>

namespace djinterop::engine::schema
{
struct master_list_entry
{
    std::string item_type;
    std::string item_name;
    std::string table_name;
};

struct table_info_entry
{
    std::string table_name;
    std::string col_name;
    std::string col_type;
    int nullable;
    std::string default_value;
    int part_of_pk;
};

struct index_list_entry
{
    std::string table_name;
    std::string index_name;
    int unique;
    std::string creation_method;
    int partial_index;
};

struct index_info_entry
{
    std::string index_name;
    int ordinal;
    std::string col_name;
};

inline bool operator<(const master_list_entry& o1, const master_list_entry& o2)
{
    return o1.item_name < o2.item_name;
}

inline bool operator<(const table_info_entry& o1, const table_info_entry& o2)
{
    return o1.col_name < o2.col_name;
}

inline bool operator<(const index_list_entry& o1, const index_list_entry& o2)
{
    return o1.index_name < o2.index_name;
}

inline bool operator<(const index_info_entry& o1, const index_info_entry& o2)
{
    return o1.ordinal < o2.ordinal;
}

struct master_list
{
    using iterator = std::set<master_list_entry>::iterator;
    using const_iterator = std::set<master_list_entry>::const_iterator;

    master_list(
        sqlite::database& db, const std::string& db_name,
        const std::string& item_type)
    {
        db << "SELECT name, tbl_name FROM " + db_name +
                    ".sqlite_master WHERE type = '" + item_type + "'" >>
            [&](std::string item_name, std::string table_name)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(master_list_entry{
                item_type, std::move(item_name), std::move(table_name)});
        };
    }

    master_list(sqlite::database& db, const std::string& item_type)
    {
        db << "SELECT name, tbl_name FROM sqlite_master WHERE type = '" +
                    item_type + "'" >>
            [&](std::string item_name, std::string table_name)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(master_list_entry{
                item_type, std::move(item_name), std::move(table_name)});
        };
    }

    [[nodiscard]] iterator begin() { return cols_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept
    {
        return cols_.cbegin();
    }
    [[nodiscard]] iterator end() { return cols_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return cols_.cend(); }

private:
    std::set<master_list_entry> cols_;
};

struct table_info
{
    using iterator = std::set<table_info_entry>::iterator;
    using const_iterator = std::set<table_info_entry>::const_iterator;

    table_info(
        sqlite::database& db, const std::string& db_name,
        const std::string& table_name)
    {
        db << "PRAGMA " + db_name + ".table_info('" + table_name + "')" >>
            [&]([[maybe_unused]] int col_id, std::string col_name,
                std::string col_type, int nullable, std::string default_value,
                int part_of_pk)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(table_info_entry{
                table_name, std::move(col_name), std::move(col_type), nullable,
                std::move(default_value), part_of_pk});
        };
    }

    table_info(sqlite::database& db, const std::string& table_name)
    {
        db << "PRAGMA table_info('" + table_name + "')" >>
            [&]([[maybe_unused]] int col_id, std::string col_name,
                std::string col_type, int nullable, std::string default_value,
                int part_of_pk)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(table_info_entry{
                table_name, std::move(col_name), std::move(col_type), nullable,
                std::move(default_value), part_of_pk});
        };
    }

    [[nodiscard]] iterator begin() { return cols_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept
    {
        return cols_.cbegin();
    }
    [[nodiscard]] iterator end() { return cols_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return cols_.cend(); }

private:
    std::set<table_info_entry> cols_;
};

struct index_list
{
    using iterator = std::set<index_list_entry>::iterator;
    using const_iterator = std::set<index_list_entry>::const_iterator;

    index_list(
        sqlite::database& db, const std::string& db_name,
        const std::string& table_name)
    {
        db << "PRAGMA " + db_name + ".index_list('" + table_name + "')" >>
            [&]([[maybe_unused]] int index_id, std::string index_name,
                int unique, std::string creation_method, int partial_index)
        {
            // Note that emplace() does not support aggregate initialisation
            indices_.insert(index_list_entry{
                table_name, std::move(index_name), unique,
                std::move(creation_method), partial_index});
        };
    }

    index_list(sqlite::database& db, const std::string& table_name)
    {
        db << "PRAGMA index_list('" + table_name + "')" >>
            [&]([[maybe_unused]] int index_id, std::string index_name,
                int unique, std::string creation_method, int partial_index)
        {
            // Note that emplace() does not support aggregate initialisation
            indices_.insert(index_list_entry{
                table_name, std::move(index_name), unique,
                std::move(creation_method), partial_index});
        };
    }

    [[nodiscard]] iterator begin() { return indices_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept
    {
        return indices_.cbegin();
    }
    [[nodiscard]] iterator end() { return indices_.end(); }
    [[nodiscard]] const_iterator end() const noexcept
    {
        return indices_.cend();
    }

private:
    std::set<index_list_entry> indices_;
};

struct index_info
{
    using iterator = std::set<index_info_entry>::iterator;
    using const_iterator = std::set<index_info_entry>::const_iterator;

    index_info(
        sqlite::database& db, const std::string& db_name,
        const std::string& index_name)
    {
        db << "PRAGMA " + db_name + ".index_info('" + index_name + "')" >>
            [&](int ordinal, [[maybe_unused]] int col_table_id,
                std::string col_name)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(
                index_info_entry{index_name, ordinal, std::move(col_name)});
        };
    }

    index_info(sqlite::database& db, const std::string& index_name)
    {
        db << "PRAGMA index_info('" + index_name + "')" >>
            [&](int ordinal, [[maybe_unused]] int col_table_id,
                std::string col_name)
        {
            // Note that emplace() does not support aggregate initialisation
            cols_.insert(
                index_info_entry{index_name, ordinal, std::move(col_name)});
        };
    }

    [[nodiscard]] iterator begin() { return cols_.begin(); }
    [[nodiscard]] const_iterator begin() const noexcept
    {
        return cols_.cbegin();
    }
    [[nodiscard]] iterator end() { return cols_.end(); }
    [[nodiscard]] const_iterator end() const noexcept { return cols_.cend(); }

private:
    std::set<index_info_entry> cols_;
};

inline void validate(
    master_list::const_iterator iter, master_list::const_iterator end,
    const std::string& db_name, const std::string& item_type,
    const std::string& item_name, const std::string& table_name)
{
    if (iter == end)
        throw database_inconsistency{
            "Item " + item_name + " of type " + item_type +
            " (relating to table " + table_name + ") missing from DB " +
            db_name};
    if (iter->item_name != item_name)
        throw database_inconsistency{
            "Item " + iter->item_name + " of type " + item_type +
            " (relating to table " + iter->table_name + ") on " + db_name +
            " in wrong order, expected " + item_name + " (relating to table " +
            table_name + ")"};
}

inline void validate(
    master_list::const_iterator iter, master_list::const_iterator end,
    const std::string& item_type, const std::string& item_name,
    const std::string& table_name)
{
    if (iter == end)
        throw database_inconsistency{
            "Item " + item_name + " of type " + item_type +
            " (relating to table " + table_name + ") missing from DB"};
    if (iter->item_name != item_name)
        throw database_inconsistency{
            "Item " + iter->item_name + " of type " + item_type +
            " (relating to table " + iter->table_name +
            ") in wrong order, expected " + item_name + " (relating to table " +
            table_name + ")"};
}

inline void validate_no_more(
    const master_list::const_iterator iter,
    const master_list::const_iterator end, const std::string& db_name)
{
    if (iter != end)
        throw database_inconsistency{
            "There are more " + iter->item_type + " entries in " + db_name +
            " than expected: next one is " + iter->item_name};
}

inline void validate_no_more(
    const master_list::const_iterator iter,
    const master_list::const_iterator end)
{
    if (iter != end)
        throw database_inconsistency{
            "There are more " + iter->item_type +
            " entries than expected: next one is " + iter->item_name};
}

inline void validate(
    table_info::const_iterator iter, table_info::const_iterator end,
    const std::string& col_name, const std::string& col_type, int nullable,
    const std::string& default_value, int part_of_pk)
{
    if (iter == end)
        throw database_inconsistency{"Column " + col_name + " missing"};
    if (iter->col_name != col_name)
        throw database_inconsistency{
            "Column " + iter->col_name + " on " + iter->table_name +
            " in wrong order, expected " + col_name};
    if (iter->col_type != col_type)
        throw database_inconsistency{
            "Column " + col_name + " on " + iter->table_name +
            " has wrong type: " + iter->col_type + ", expected " + col_type};
    if (iter->nullable != nullable)
        throw database_inconsistency{
            "Column " + col_name + " on " + iter->table_name +
            " has wrong nullability: " + std::to_string(iter->nullable)};
    if (iter->default_value != default_value)
        throw database_inconsistency{
            "Column " + col_name + " on " + iter->table_name +
            " has wrong default value: \"" + iter->default_value +
            "\", expected \"" + default_value + "\""};
    if (iter->part_of_pk != part_of_pk)
        throw database_inconsistency{
            "Column " + col_name + " on " + iter->table_name +
            " has wrong PK membership: " + std::to_string(iter->part_of_pk)};
}

inline void validate_no_more(
    const table_info::const_iterator iter, const table_info::const_iterator end)
{
    if (iter != end)
        throw database_inconsistency{
            "There are more columns on table " + iter->table_name +
            " than expected: next one is " + iter->col_name};
}

inline void validate(
    index_list::const_iterator iter, index_list::const_iterator end,
    const std::string& index_name, int unique,
    const std::string& creation_method, int partial_index)
{
    if (iter == end)
        throw database_inconsistency{"Index " + index_name + " missing"};
    if (iter->index_name != index_name)
        throw database_inconsistency{
            "Index " + iter->index_name + " on " + iter->table_name +
            " in wrong order, expected " + index_name};
    if (iter->unique != unique)
        throw database_inconsistency{
            "Index " + index_name + " on " + iter->table_name +
            " has wrong uniqueness: " + std::to_string(iter->unique)};
    if (iter->creation_method != creation_method)
        throw database_inconsistency{
            "Index " + index_name + " on " + iter->table_name +
            " has wrong creation method: \"" + iter->creation_method + "\""};
    if (iter->partial_index != partial_index)
        throw database_inconsistency{
            "Index " + index_name + " on " + iter->table_name +
            " has wrong \"partiality\": " +
            std::to_string(iter->partial_index)};
}

inline void validate_no_more(
    const index_list::const_iterator iter, const index_list::const_iterator end)
{
    if (iter != end)
        throw database_inconsistency{
            "There are more indexes on table " + iter->table_name +
            " than expected: next one is " + iter->index_name};
}

inline void validate(
    index_info::const_iterator iter, index_info::const_iterator end,
    int ordinal, const std::string& col_name)
{
    if (iter == end)
        throw database_inconsistency{"Col " + col_name + " missing from index"};
    if (iter->ordinal != ordinal)
        throw database_inconsistency{
            "Col " + col_name + " on " + iter->index_name +
            " has wrong rank within the index: " +
            std::to_string(iter->ordinal)};
    if (iter->col_name != col_name)
        throw database_inconsistency{
            "Col " + iter->col_name + " on " + iter->index_name +
            " in wrong order, expected " + col_name};
}

inline void validate_no_more(
    const index_info::const_iterator iter, const index_info::const_iterator end)
{
    if (iter != end)
        throw database_inconsistency{
            "There are more columns in index " + iter->index_name +
            " than expected: next one is " + iter->col_name};
}

}  // namespace djinterop::engine::schema
