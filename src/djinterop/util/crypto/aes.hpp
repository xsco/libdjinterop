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

namespace djinterop::util::crypto
{
constexpr size_t aes_block_length = 16;
constexpr size_t aes256_key_length = 32;

/// AES-256 in cipher block chaining mode, as specified by FIPS 197 and
/// NIST SP 800-38A.
///
/// No padding scheme is applied: input lengths must be a whole number of
/// blocks.  SQLCipher pages are always block-aligned by construction, which is
/// why a padding mode is not needed.
class aes256_cbc
{
public:
    /// Construct a cipher for a given key, which must be
    /// `aes256_key_length` bytes long.
    explicit aes256_cbc(const uint8_t* key) noexcept;

    /// Encrypt `length` bytes from `input` into `output`, which may alias
    /// `input`.  `length` must be a multiple of the AES block length.
    /// The initialisation vector must be `aes_block_length` bytes long.
    void encrypt(
        const uint8_t* iv, const uint8_t* input, uint8_t* output,
        size_t length) const noexcept;

    /// Decrypt `length` bytes from `input` into `output`, which may alias
    /// `input`.  `length` must be a multiple of the AES block length.
    /// The initialisation vector must be `aes_block_length` bytes long.
    void decrypt(
        const uint8_t* iv, const uint8_t* input, uint8_t* output,
        size_t length) const noexcept;

private:
    void encrypt_block(const uint8_t* input, uint8_t* output) const noexcept;
    void decrypt_block(const uint8_t* input, uint8_t* output) const noexcept;

    /// Expanded key schedule: 15 round keys of 16 bytes each.
    std::array<uint8_t, 16 * 15> round_keys_;
};

}  // namespace djinterop::util::crypto
