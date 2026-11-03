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

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <djinterop/onelibrary/onelibrary.hpp>
#include <djinterop/onelibrary/v1/library.hpp>

#include "../temporary_directory.hpp"

#define ONELIBRARY_STRINGIFY(x) ONELIBRARY_STRINGIFY_(x)
#define ONELIBRARY_STRINGIFY_(x) #x

/// Directory holding the reference script for the v1 schema.
inline std::string onelibrary_v1_script_directory()
{
    return std::string{ONELIBRARY_STRINGIFY(TESTDATA_DIR)} +
           "/ref/onelibrary/v1";
}

/// Write a script directory holding the given statements, one per line.
inline void write_script(
    const std::filesystem::path& script_directory,
    const std::vector<std::string>& statements)
{
    std::filesystem::create_directories(script_directory);
    std::ofstream script{script_directory / "exportLibrary.db.sql"};
    for (const auto& statement : statements)
        script << statement << "\n";

    if (!script)
        throw std::runtime_error{
            "Cannot write a script in " + script_directory.string()};
}

/// Write a script directory holding the reference schema, followed by
/// statements of a test's own.
inline void write_script_on_schema(
    const std::filesystem::path& script_directory,
    const std::vector<std::string>& statements)
{
    const auto schema_path =
        onelibrary_v1_script_directory() + "/exportLibrary.db.sql";
    std::ifstream schema{schema_path};
    if (!schema)
        throw std::runtime_error{"Cannot read the schema at " + schema_path};

    std::vector<std::string> lines;
    for (std::string line; std::getline(schema, line);)
        lines.push_back(line);

    lines.insert(lines.end(), statements.begin(), statements.end());
    write_script(script_directory, lines);
}

/// A device holding the reference schema and a test's own data, created
/// through the public API.
///
/// Opening an encrypted database derives its key, which is deliberately
/// expensive, so a test module should build a device once and share it.
struct onelibrary_device
{
    explicit onelibrary_device(
        const std::vector<std::string>& statements,
        const std::string& passphrase = "a passphrase for the fixture") :
        root{temp_dir.temp_dir / "device"},
        library{create(temp_dir, root, statements, passphrase)}
    {
    }

    temporary_directory temp_dir;

    /// Root directory of the device.
    std::filesystem::path root;

    djinterop::onelibrary::v1::library library;

private:
    static djinterop::onelibrary::v1::library create(
        const temporary_directory& temp_dir, const std::filesystem::path& root,
        const std::vector<std::string>& statements,
        const std::string& passphrase)
    {
        const auto script_directory = temp_dir.temp_dir / "scripts";
        write_script_on_schema(script_directory, statements);
        return djinterop::onelibrary::v1::library::create_from_scripts(
            root, script_directory, passphrase);
    }
};
