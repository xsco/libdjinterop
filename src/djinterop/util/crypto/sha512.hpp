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

#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace djinterop::util::crypto
{
constexpr size_t sha512_digest_length = 64;
constexpr size_t sha512_block_length = 128;

using sha512_digest = std::array<uint8_t, sha512_digest_length>;

/// Streaming SHA-512, as specified by FIPS 180-4.
class sha512
{
public:
    sha512() noexcept;

    void update(const uint8_t* data, size_t length) noexcept;

    /// Finalise the hash.  The object must not be reused afterwards.
    [[nodiscard]] sha512_digest finalise() noexcept;

    static sha512_digest hash(const uint8_t* data, size_t length) noexcept;

private:
    void compress(const uint8_t* block) noexcept;

    std::array<uint64_t, 8> state_;
    std::array<uint8_t, sha512_block_length> buffer_;
    size_t buffered_;
    uint64_t total_length_;
};

/// Compute HMAC-SHA-512, as specified by RFC 2104.
sha512_digest hmac_sha512(
    const uint8_t* key, size_t key_length, const uint8_t* data,
    size_t data_length) noexcept;

/// Derive `length` bytes using PBKDF2-HMAC-SHA-512, as specified by RFC 8018.
std::vector<uint8_t> pbkdf2_hmac_sha512(
    const uint8_t* password, size_t password_length, const uint8_t* salt,
    size_t salt_length, uint32_t iterations, size_t length);

}  // namespace djinterop::util::crypto
