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

#include <cstddef>
#include <cstdint>

/// The processor's own AES instructions, where it has them.
///
/// x86 has had AES-NI since 2010 and AArch64 the ARMv8 cryptographic extension
/// from the start, so in practice these are what runs, and the tables in
/// `aes.cpp` are the fallback.  Whether the instructions are there is a
/// property of the processor rather than of the build, so it is asked at run
/// time and this is the only translation unit compiled with them enabled.
namespace djinterop::util::crypto::hardware
{
/// Whether the processor running this program has AES instructions.  The
/// underlying query is made once and remembered.
[[nodiscard]] bool aes_available() noexcept;

/// Encrypt in cipher block chaining mode.  `length` is a whole number of
/// blocks, `output` may alias `input`, and `round_keys` is the forward
/// schedule of 15 round keys.
void aes256_cbc_encrypt(
    const uint8_t* round_keys, const uint8_t* iv, const uint8_t* input,
    uint8_t* output, size_t length) noexcept;

/// Decrypt in cipher block chaining mode, taking the schedule of the
/// equivalent inverse cipher, which is the form both instruction sets expect.
///
/// Chaining constrains encryption to one block at a time, but not decryption:
/// a block needs only its own ciphertext and the one before it, so several go
/// at once here to keep the pipeline of the AES instructions full.
void aes256_cbc_decrypt(
    const uint8_t* inverse_round_keys, const uint8_t* iv, const uint8_t* input,
    uint8_t* output, size_t length) noexcept;

}  // namespace djinterop::util::crypto::hardware
