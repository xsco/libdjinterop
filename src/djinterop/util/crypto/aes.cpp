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

#include "aes.hpp"

#include <cstring>

namespace djinterop::util::crypto
{
namespace
{
constexpr int rounds = 14;
constexpr int key_words = 8;

inline uint8_t rotl8(uint8_t x, unsigned shift) noexcept
{
    return static_cast<uint8_t>((x << shift) | (x >> (8 - shift)));
}

/// Multiply by x in GF(2^8) modulo the AES polynomial.
inline uint8_t xtime(uint8_t x) noexcept
{
    return static_cast<uint8_t>((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00));
}

/// Multiply two elements of GF(2^8) modulo the AES polynomial.
inline uint8_t gmul(uint8_t a, uint8_t b) noexcept
{
    uint8_t result = 0;
    while (b != 0)
    {
        if (b & 1)
            result ^= a;
        a = xtime(a);
        b = static_cast<uint8_t>(b >> 1);
    }
    return result;
}

/// The AES substitution box and its inverse.
///
/// These are derived rather than tabulated: each entry is the affine transform
/// of the multiplicative inverse in GF(2^8), which the standard walk over
/// p = 3^i, q = 3^-i enumerates in a single pass.
struct substitution_tables
{
    uint8_t forward[256];
    uint8_t inverse[256];

    substitution_tables() noexcept : forward{}, inverse{}
    {
        uint8_t p = 1;
        uint8_t q = 1;
        do
        {
            p = static_cast<uint8_t>(p ^ (p << 1) ^ ((p & 0x80) ? 0x1b : 0));

            // q = q / 3, i.e. q multiplied by the inverse of 3.
            q ^= static_cast<uint8_t>(q << 1);
            q ^= static_cast<uint8_t>(q << 2);
            q ^= static_cast<uint8_t>(q << 4);
            if (q & 0x80)
                q ^= 0x09;

            const auto value = static_cast<uint8_t>(
                q ^ rotl8(q, 1) ^ rotl8(q, 2) ^ rotl8(q, 3) ^ rotl8(q, 4) ^
                0x63);
            forward[p] = value;
            inverse[value] = p;
        } while (p != 1);

        // Zero has no multiplicative inverse; it maps to the affine constant.
        forward[0] = 0x63;
        inverse[0x63] = 0x00;
    }
};

const substitution_tables& tables() noexcept
{
    static const substitution_tables instance;
    return instance;
}

}  // anonymous namespace

aes256_cbc::aes256_cbc(const uint8_t* key) noexcept : round_keys_{}
{
    const auto& sbox = tables().forward;

    std::memcpy(round_keys_.data(), key, aes256_key_length);

    uint8_t rcon = 1;
    for (int word = key_words; word < 4 * (rounds + 1); ++word)
    {
        uint8_t temp[4];
        std::memcpy(temp, round_keys_.data() + (4 * (word - 1)), 4);

        if (word % key_words == 0)
        {
            // Rotate, substitute, and add the round constant.
            const auto first = temp[0];
            temp[0] = static_cast<uint8_t>(sbox[temp[1]] ^ rcon);
            temp[1] = sbox[temp[2]];
            temp[2] = sbox[temp[3]];
            temp[3] = sbox[first];
            rcon = xtime(rcon);
        }
        else if (word % key_words == 4)
        {
            for (auto& byte : temp)
                byte = sbox[byte];
        }

        for (int i = 0; i < 4; ++i)
        {
            round_keys_[(4 * word) + i] =
                round_keys_[(4 * (word - key_words)) + i] ^ temp[i];
        }
    }
}

void aes256_cbc::encrypt_block(
    const uint8_t* input, uint8_t* output) const noexcept
{
    const auto& sbox = tables().forward;

    uint8_t state[16];
    for (int i = 0; i < 16; ++i)
        state[i] = input[i] ^ round_keys_[i];

    for (int round = 1; round <= rounds; ++round)
    {
        for (auto& byte : state)
            byte = sbox[byte];

        // ShiftRows: the state is column-major, so row r is bytes r, r+4, ...
        uint8_t shifted[16];
        for (int column = 0; column < 4; ++column)
            for (int row = 0; row < 4; ++row)
                shifted[(4 * column) + row] =
                    state[(4 * ((column + row) % 4)) + row];
        std::memcpy(state, shifted, 16);

        if (round != rounds)
        {
            for (int column = 0; column < 4; ++column)
            {
                auto* c = state + (4 * column);
                const auto sum =
                    static_cast<uint8_t>(c[0] ^ c[1] ^ c[2] ^ c[3]);
                const auto c0 = c[0];
                c[0] ^= static_cast<uint8_t>(sum ^ xtime(c[0] ^ c[1]));
                c[1] ^= static_cast<uint8_t>(sum ^ xtime(c[1] ^ c[2]));
                c[2] ^= static_cast<uint8_t>(sum ^ xtime(c[2] ^ c[3]));
                c[3] ^= static_cast<uint8_t>(sum ^ xtime(c[3] ^ c0));
            }
        }

        for (int i = 0; i < 16; ++i)
            state[i] ^= round_keys_[(16 * round) + i];
    }

    std::memcpy(output, state, 16);
}

void aes256_cbc::decrypt_block(
    const uint8_t* input, uint8_t* output) const noexcept
{
    const auto& rsbox = tables().inverse;

    uint8_t state[16];
    for (int i = 0; i < 16; ++i)
        state[i] = input[i] ^ round_keys_[(16 * rounds) + i];

    for (int round = rounds - 1; round >= 0; --round)
    {
        // InvShiftRows.
        uint8_t shifted[16];
        for (int column = 0; column < 4; ++column)
            for (int row = 0; row < 4; ++row)
                shifted[(4 * ((column + row) % 4)) + row] =
                    state[(4 * column) + row];
        std::memcpy(state, shifted, 16);

        for (auto& byte : state)
            byte = rsbox[byte];

        for (int i = 0; i < 16; ++i)
            state[i] ^= round_keys_[(16 * round) + i];

        if (round != 0)
        {
            for (int column = 0; column < 4; ++column)
            {
                auto* c = state + (4 * column);
                const uint8_t a0 = c[0], a1 = c[1], a2 = c[2], a3 = c[3];
                c[0] = static_cast<uint8_t>(
                    gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9));
                c[1] = static_cast<uint8_t>(
                    gmul(a0, 9) ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13));
                c[2] = static_cast<uint8_t>(
                    gmul(a0, 13) ^ gmul(a1, 9) ^ gmul(a2, 14) ^ gmul(a3, 11));
                c[3] = static_cast<uint8_t>(
                    gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9) ^ gmul(a3, 14));
            }
        }
    }

    std::memcpy(output, state, 16);
}

void aes256_cbc::encrypt(
    const uint8_t* iv, const uint8_t* input, uint8_t* output,
    size_t length) const noexcept
{
    uint8_t chain[aes_block_length];
    std::memcpy(chain, iv, aes_block_length);

    for (size_t offset = 0; offset < length; offset += aes_block_length)
    {
        uint8_t block[aes_block_length];
        for (size_t i = 0; i < aes_block_length; ++i)
            block[i] = input[offset + i] ^ chain[i];

        encrypt_block(block, output + offset);
        std::memcpy(chain, output + offset, aes_block_length);
    }
}

void aes256_cbc::decrypt(
    const uint8_t* iv, const uint8_t* input, uint8_t* output,
    size_t length) const noexcept
{
    uint8_t chain[aes_block_length];
    std::memcpy(chain, iv, aes_block_length);

    for (size_t offset = 0; offset < length; offset += aes_block_length)
    {
        // Retain the ciphertext before writing, in case output aliases input.
        uint8_t next_chain[aes_block_length];
        std::memcpy(next_chain, input + offset, aes_block_length);

        decrypt_block(input + offset, output + offset);
        for (size_t i = 0; i < aes_block_length; ++i)
            output[offset + i] ^= chain[i];

        std::memcpy(chain, next_chain, aes_block_length);
    }
}

}  // namespace djinterop::util::crypto
