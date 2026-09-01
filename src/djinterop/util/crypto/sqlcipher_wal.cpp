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

#include <algorithm>
#include <cstring>
#include <exception>
#include <fstream>
#include <thread>
#include <utility>

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

/// Read a whole file, or nothing if it cannot be read.
std::vector<uint8_t> read_file(const std::string& path)
{
    std::ifstream file{path, std::ios::binary | std::ios::ate};
    if (!file)
        return {};

    const auto size = static_cast<std::streamoff>(file.tellg());
    if (size <= 0)
        return {};

    std::vector<uint8_t> contents(static_cast<size_t>(size));
    file.seekg(0);
    if (!file.read(
            reinterpret_cast<char*>(contents.data()),
            static_cast<std::streamsize>(size)))
        return {};

    return contents;
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

/// The pages of a log that a reader should honour.
struct wal_contents
{
    /// Page number and ciphertext offset of every committed frame, in the
    /// order the log wrote them.
    std::vector<std::pair<uint32_t, size_t>> frames;

    /// Size the database is to be truncated or extended to, in pages, or zero
    /// if the log commits nothing.
    uint32_t page_count = 0;
};

/// Walk the frames of a log, keeping those up to the last one that committed:
/// anything after it belongs to a transaction that never finished, exactly as
/// SQLite's own recovery decides.
///
/// \throws sqlcipher_error If the log is not one, or does not go with the
///                         database beside it.
wal_contents read_log(
    const std::vector<uint8_t>& log, const std::string& database_path,
    size_t page_size)
{
    wal_contents contents;
    if (log.size() < wal_header_length)
        return contents;

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

    const auto frame_length = wal_frame_header_length + page_size;
    size_t committed_frames = 0;

    for (size_t offset = wal_header_length; offset + frame_length <= log.size();
         offset += frame_length)
    {
        const auto* frame = log.data() + offset;
        const auto page_number = load_be32(frame);
        const auto truncate_to = load_be32(frame + 4);

        // A frame written after the log was reset carries the salt of the
        // previous incarnation, and is not part of it.
        if (load_be32(frame + 8) != salt_1 || load_be32(frame + 12) != salt_2)
            break;

        auto candidate = running;
        candidate.accumulate(frame, 8, big_endian_checksums);
        candidate.accumulate(
            frame + wal_frame_header_length, page_size, big_endian_checksums);
        if (!candidate.matches(frame + 16))
            break;

        running = candidate;
        contents.frames.emplace_back(
            page_number, offset + wal_frame_header_length);

        if (truncate_to != 0)
        {
            committed_frames = contents.frames.size();
            contents.page_count = truncate_to;
        }
    }

    contents.frames.resize(committed_frames);
    return contents;
}

/// The invariants of decrypting an image: everything a page needs but its own
/// number.
struct page_work
{
    const sqlcipher_codec& codec;

    /// Where each page's ciphertext lies, or null for one that is in neither
    /// the database file nor the log.
    const std::vector<const uint8_t*>& sources;

    uint8_t* image;
    size_t page_size;
};

/// Decrypt a run of pages, whose sources are already settled.
void decrypt_range(const page_work& work, size_t first, size_t last)
{
    for (size_t index = first; index < last; ++index)
    {
        const auto* encrypted = work.sources[index];
        if (encrypted == nullptr)
            continue;

        work.codec.decrypt_page(
            static_cast<uint32_t>(index + 1), encrypted,
            work.image + (index * work.page_size));
    }
}

/// Decrypt every page, over as many processors as there is work for.
///
/// A page carries its own initialisation vector and is bound to its own number,
/// so none depends on any other and the work divides by simple arithmetic.
void decrypt_pages(const page_work& work)
{
    // Enough pages that a thread earns the cost of starting it.
    constexpr size_t pages_per_worker = 256;
    constexpr size_t worker_limit = 16;

    const auto page_count = work.sources.size();
    auto workers = std::min(page_count / pages_per_worker, worker_limit);
    workers = std::min<size_t>(workers, std::thread::hardware_concurrency());

    if (workers < 2)
    {
        decrypt_range(work, 0, page_count);
        return;
    }

    std::vector<std::exception_ptr> failures(workers);
    const auto share = (page_count + workers - 1) / workers;

    const auto run = [&](size_t worker)
    {
        try
        {
            const auto first = worker * share;
            decrypt_range(work, first, std::min(page_count, first + share));
        }
        catch (...)
        {
            failures[worker] = std::current_exception();
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(workers - 1);
    for (size_t worker = 1; worker < workers; ++worker)
        threads.emplace_back(run, worker);

    run(0);

    for (auto& thread : threads)
        thread.join();

    // Report the failure nearest the start of the database, so the error a
    // caller sees does not depend on how the work happened to divide.
    for (const auto& failure : failures)
    {
        if (failure)
            std::rethrow_exception(failure);
    }
}

}  // anonymous namespace

std::vector<uint8_t> decrypt_database_to_image(
    const std::string& database_path, const sqlcipher_codec& codec)
{
    const auto page_size = codec.parameters().page_size;

    auto image = read_file(database_path);
    if (image.size() < page_size)
        throw sqlcipher_error{
            "`" + database_path + "` is too small to be a database"};

    if (image.size() % page_size != 0)
        throw sqlcipher_error{
            "`" + database_path + "` is not a whole number of pages"};

    const auto database_pages = image.size() / page_size;

    // The log is read first: it settles how large the finished image is, and
    // which pages of the database file are worth decrypting at all.
    const auto log = read_file(database_path + "-wal");
    const auto contents = read_log(log, database_path, page_size);

    const auto image_pages = contents.page_count != 0
                                 ? static_cast<size_t>(contents.page_count)
                                 : database_pages;

    // Decryption is in place, so the image is sized first rather than being
    // copied out of a second buffer afterwards.
    image.resize(image_pages * page_size, 0);

    // Settle where each page's ciphertext lies before decrypting any of it.  A
    // page the log replaces is never read from the database file, and rekordbox
    // leaves most of a fresh export in the log, so that saves the greater part
    // of the work.  A page in neither keeps the zeroes the resize gave it.
    std::vector<const uint8_t*> sources(image_pages, nullptr);
    const auto pages_in_both = std::min(image_pages, database_pages);
    for (size_t index = 0; index < pages_in_both; ++index)
        sources[index] = image.data() + (index * page_size);

    for (const auto& [page_number, payload_offset] : contents.frames)
    {
        if (page_number == 0 || page_number > image_pages)
            continue;

        sources[page_number - 1] = log.data() + payload_offset;
    }

    decrypt_pages({codec, sources, image.data(), page_size});

    // Page one opens with the salt, where a plain database has its magic.
    std::memcpy(image.data(), sqlite_file_magic, sqlite_file_magic_length);

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
