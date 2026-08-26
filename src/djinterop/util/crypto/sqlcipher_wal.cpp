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

#include "sqlcipher_wal.hpp"

#include <cstring>
#include <fstream>

namespace djinterop::util::crypto
{
namespace
{
constexpr size_t wal_header_length = 32;
constexpr size_t wal_frame_header_length = 24;

/// The two magic numbers that open a log, differing in the byte order used
/// for its checksums.
constexpr uint32_t wal_magic_little_endian = 0x377f0682;
constexpr uint32_t wal_magic_big_endian = 0x377f0683;

/// Offsets of the two version bytes in the SQLite header.
constexpr size_t write_version_offset = 18;
constexpr size_t read_version_offset = 19;
constexpr uint8_t rollback_journal_version = 1;

uint32_t load_be32(const uint8_t* p) noexcept
{
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

uint32_t load_le32(const uint8_t* p) noexcept
{
    return (static_cast<uint32_t>(p[3]) << 24) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[1]) << 8) | static_cast<uint32_t>(p[0]);
}

std::vector<uint8_t> read_file(const std::string& path)
{
    std::ifstream file{path, std::ios::binary};
    if (!file)
        return {};

    return std::vector<uint8_t>{
        std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

/// The running checksum SQLite keeps over the contents of a log.
///
/// Each step folds a pair of 32-bit words into the pair of accumulators, so
/// the checksum of a frame depends on every frame before it: a log cannot be
/// truncated or reordered without detection.
struct wal_checksum
{
    uint32_t s0 = 0;
    uint32_t s1 = 0;

    void accumulate(
        const uint8_t* data, size_t length, bool big_endian) noexcept
    {
        for (size_t offset = 0; offset + 8 <= length; offset += 8)
        {
            const auto first = big_endian ? load_be32(data + offset)
                                          : load_le32(data + offset);
            const auto second = big_endian ? load_be32(data + offset + 4)
                                           : load_le32(data + offset + 4);
            s0 += first + s1;
            s1 += second + s0;
        }
    }

    /// Compare against a stored pair of checksums.
    ///
    /// The byte order of the magic number governs how the *contents* are read
    /// while accumulating, but the two stored values are header fields, and
    /// every header field in a log is big-endian.
    [[nodiscard]] bool matches(const uint8_t* expected) const noexcept
    {
        return s0 == load_be32(expected) && s1 == load_be32(expected + 4);
    }
};

}  // anonymous namespace

std::vector<uint8_t> decrypt_database_to_image(
    const std::string& database_path, const sqlcipher_codec& codec)
{
    const auto& params = codec.parameters();
    auto encrypted = read_file(database_path);
    if (encrypted.size() < params.page_size)
        throw sqlcipher_error{
            "`" + database_path + "` is too small to be a database"};

    if (encrypted.size() % params.page_size != 0)
        throw sqlcipher_error{
            "`" + database_path + "` is not a whole number of pages"};

    const auto page_size = params.page_size;
    const auto page_count = encrypted.size() / page_size;

    std::vector<uint8_t> image(encrypted.size());
    for (size_t index = 0; index < page_count; ++index)
    {
        codec.decrypt_page(
            static_cast<uint32_t>(index + 1),
            encrypted.data() + (index * page_size),
            image.data() + (index * page_size));
    }

    // Page one opens with the salt, where a plain database has its magic.
    std::memcpy(image.data(), sqlite_file_magic, sqlite_file_magic_length);

    const auto log = read_file(database_path + "-wal");
    if (log.size() >= wal_header_length)
    {
        const auto magic = load_be32(log.data());
        const auto big_endian_checksums = magic == wal_magic_big_endian;
        if (magic != wal_magic_little_endian && !big_endian_checksums)
            throw sqlcipher_error{
                "`" + database_path +
                "-wal` does not begin like a write-ahead log"};

        const auto log_page_size = load_be32(log.data() + 8);
        if (log_page_size != page_size)
            throw sqlcipher_error{
                "the write-ahead log of `" + database_path +
                "` uses a different page size from the database"};

        const auto salt_1 = load_be32(log.data() + 16);
        const auto salt_2 = load_be32(log.data() + 20);

        wal_checksum running;
        running.accumulate(log.data(), 24, big_endian_checksums);
        if (!running.matches(log.data() + 24))
            throw sqlcipher_error{
                "the write-ahead log of `" + database_path +
                "` has a damaged header"};

        // Walk the frames, keeping only those up to the last one that
        // committed: anything after it belongs to a transaction that never
        // finished, exactly as SQLite's own recovery decides.
        const auto frame_length = wal_frame_header_length + page_size;
        std::vector<std::pair<uint32_t, size_t>> frames;
        size_t committed_frames = 0;
        uint32_t committed_page_count = 0;

        for (size_t offset = wal_header_length;
             offset + frame_length <= log.size(); offset += frame_length)
        {
            const auto* frame = log.data() + offset;
            const auto page_number = load_be32(frame);
            const auto truncate_to = load_be32(frame + 4);

            // A frame written after the log was reset carries the salt of the
            // previous incarnation, and is not part of it.
            if (load_be32(frame + 8) != salt_1 ||
                load_be32(frame + 12) != salt_2)
                break;

            auto candidate = running;
            candidate.accumulate(frame, 8, big_endian_checksums);
            candidate.accumulate(
                frame + wal_frame_header_length, page_size,
                big_endian_checksums);
            if (!candidate.matches(frame + 16))
                break;

            running = candidate;
            frames.emplace_back(page_number, offset + wal_frame_header_length);

            if (truncate_to != 0)
            {
                committed_frames = frames.size();
                committed_page_count = truncate_to;
            }
        }

        if (committed_page_count != 0)
        {
            const auto committed_bytes =
                static_cast<size_t>(committed_page_count) * page_size;
            image.resize(committed_bytes, 0);
        }

        for (size_t index = 0; index < committed_frames; ++index)
        {
            const auto [page_number, payload_offset] = frames[index];
            const auto page_offset =
                static_cast<size_t>(page_number - 1) * page_size;
            if (page_number == 0 || page_offset + page_size > image.size())
                continue;

            codec.decrypt_page(
                page_number, log.data() + payload_offset,
                image.data() + page_offset);

            if (page_number == 1)
                std::memcpy(
                    image.data(), sqlite_file_magic, sqlite_file_magic_length);
        }
    }

    // The image no longer has a log, so mark it as using a rollback journal.
    // Left as it is, SQLite would look for the log that has just been folded
    // in, and refuse to open the database read-only without it.
    if (image.size() > read_version_offset)
    {
        image[write_version_offset] = rollback_journal_version;
        image[read_version_offset] = rollback_journal_version;
    }

    return image;
}

}  // namespace djinterop::util::crypto
