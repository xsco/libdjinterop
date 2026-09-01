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

/// Number of rounds in AES-256, and hence one less than the number of round
/// keys that its schedule expands to.
constexpr int aes256_rounds = 14;

constexpr size_t aes256_schedule_length =
    aes_block_length * (aes256_rounds + 1);

/// Which implementation an instance should use.
enum class aes_implementation
{
    /// The processor's AES instructions where it has them, tables otherwise.
    automatic,

    /// The tables, whatever the processor offers.  Nothing in the library asks
    /// for this; it lets tests reach the fallback on a machine that would
    /// otherwise never run it.
    tabulated,
};

/// AES-256 in cipher block chaining mode, as specified by FIPS 197 and
/// NIST SP 800-38A.
///
/// No padding scheme is applied: input lengths must be a whole number of
/// blocks.  SQLCipher pages are always block-aligned by construction, which is
/// why a padding mode is not needed.
///
/// Where the processor has AES instructions they are used, and tables
/// otherwise.  Neither resists timing analysis, and the tables plainly do not;
/// the passphrase of a OneLibrary database is a constant compiled into
/// rekordbox rather than a secret, so there is nothing to learn from it.
class aes256_cbc
{
public:
    /// Construct a cipher for a given key, which must be
    /// `aes256_key_length` bytes long.
    explicit aes256_cbc(
        const uint8_t* key, aes_implementation implementation =
                                aes_implementation::automatic) noexcept;

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
    /// Expanded key schedule: 15 round keys of 16 bytes each.
    alignas(16) std::array<uint8_t, aes256_schedule_length> round_keys_;

    /// The schedule of the equivalent inverse cipher: the round keys reversed,
    /// all but the outermost two passed through InvMixColumns.
    ///
    /// Folding that transform into the keys lets the inverse rounds take the
    /// same shape as the forward ones, which is what both the tables and the
    /// processor's AES instructions expect.
    alignas(16) std::array<uint8_t, aes256_schedule_length> inverse_round_keys_;

    /// Whether the processor this program is running on has AES instructions.
    bool hardware_;
};

}  // namespace djinterop::util::crypto
