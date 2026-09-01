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

#include <bit>
#include <cstring>

#include "aes_hardware.hpp"

namespace djinterop::util::crypto
{
namespace
{
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
constexpr uint8_t gmul(uint8_t a, uint8_t b) noexcept
{
    uint8_t result = 0;
    while (b != 0)
    {
        if (b & 1)
            result ^= a;
        a = static_cast<uint8_t>((a << 1) ^ ((a & 0x80) ? 0x1b : 0x00));
        b = static_cast<uint8_t>(b >> 1);
    }
    return result;
}

inline uint32_t load_be32(const uint8_t* p) noexcept
{
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

inline void store_be32(uint8_t* p, uint32_t v) noexcept
{
    p[0] = static_cast<uint8_t>(v >> 24);
    p[1] = static_cast<uint8_t>(v >> 16);
    p[2] = static_cast<uint8_t>(v >> 8);
    p[3] = static_cast<uint8_t>(v);
}

/// Pack four field elements into a column, most significant byte first.
constexpr uint32_t column(uint8_t r0, uint8_t r1, uint8_t r2, uint8_t r3)
{
    return (static_cast<uint32_t>(r0) << 24) |
           (static_cast<uint32_t>(r1) << 16) |
           (static_cast<uint32_t>(r2) << 8) | static_cast<uint32_t>(r3);
}

/// The substitution boxes, and the round tables built on top of them.
///
/// The boxes are derived rather than tabulated: each entry is the affine
/// transform of the multiplicative inverse in GF(2^8), which the standard walk
/// over p = 3^i, q = 3^-i enumerates in a single pass.
///
/// The round tables fold substitution and column mixing together, so a round
/// costs four lookups and four exclusive-ors per column rather than the field
/// arithmetic the definition calls for.  Only the first table of each set is
/// held: the other three are rotations of it, and a rotation is cheaper than a
/// second cache line.
struct aes_tables
{
    uint8_t forward[256];
    uint8_t inverse[256];
    uint32_t encrypt[256];
    uint32_t decrypt[256];

    aes_tables() noexcept : forward{}, inverse{}, encrypt{}, decrypt{}
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

        for (int i = 0; i < 256; ++i)
        {
            // The column MixColumns makes of a byte standing alone in row 0,
            // and the one InvMixColumns makes of the same.
            const auto s = forward[i];
            encrypt[i] = column(gmul(s, 2), s, s, gmul(s, 3));

            const auto t = inverse[i];
            decrypt[i] =
                column(gmul(t, 14), gmul(t, 9), gmul(t, 13), gmul(t, 11));
        }
    }
};

const aes_tables& tables() noexcept
{
    static const aes_tables instance;
    return instance;
}

/// Extract the byte of a column that ShiftRows will place in a given row.
inline uint8_t row_of(uint32_t c, int row) noexcept
{
    return static_cast<uint8_t>(c >> (24 - (8 * row)));
}

/// Apply InvMixColumns to a round key in place, turning the forward schedule
/// into the one the equivalent inverse cipher wants.
void inverse_mix_columns(uint8_t* key) noexcept
{
    for (int i = 0; i < 4; ++i)
    {
        auto* c = key + (4 * i);
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

/// The number of 32-bit words in a key schedule.
constexpr int schedule_words = 4 * (aes256_rounds + 1);

/// Which way ShiftRows moves the rows along.
constexpr int forwards = 1;
constexpr int backwards = -1;

/// Unpack a schedule into words once, rather than reassembling four bytes at
/// every use: a page is thousands of blocks, and a schedule is sixty words.
void unpack_schedule(const uint8_t* keys, uint32_t* words) noexcept
{
    for (int i = 0; i < schedule_words; ++i)
        words[i] = load_be32(keys + (4 * i));
}

/// One block through the round tables, in either direction.
///
/// Once decryption goes through the equivalent inverse cipher the two
/// directions have the same shape, and differ only in which tables they read
/// and which way the rows shift.
template <int direction>
void transform_block(
    const uint32_t* keys, const uint32_t* table, const uint8_t* box,
    const uint8_t* input, uint8_t* output) noexcept
{
    // ShiftRows draws row r of an output column from the column r steps away,
    // forwards when encrypting and backwards when decrypting.
    const auto from = [](int c, int row)
    { return (c + (direction * row) + 4) & 3; };

    uint32_t s[4];
    for (int i = 0; i < 4; ++i)
        s[i] = load_be32(input + (4 * i)) ^ keys[i];

    for (int round = 1; round < aes256_rounds; ++round)
    {
        const auto* rk = keys + (4 * round);

        uint32_t u[4];
        for (int c = 0; c < 4; ++c)
        {
            u[c] = table[row_of(s[from(c, 0)], 0)] ^
                   std::rotr(table[row_of(s[from(c, 1)], 1)], 8) ^
                   std::rotr(table[row_of(s[from(c, 2)], 2)], 16) ^
                   std::rotr(table[row_of(s[from(c, 3)], 3)], 24) ^ rk[c];
        }

        std::memcpy(s, u, sizeof(s));
    }

    // The last round leaves out the column mixing, so it reads the box direct.
    const auto* rk = keys + (4 * aes256_rounds);
    for (int c = 0; c < 4; ++c)
    {
        const auto substituted = column(
            box[row_of(s[from(c, 0)], 0)], box[row_of(s[from(c, 1)], 1)],
            box[row_of(s[from(c, 2)], 2)], box[row_of(s[from(c, 3)], 3)]);

        store_be32(output + (4 * c), substituted ^ rk[c]);
    }
}

}  // anonymous namespace

aes256_cbc::aes256_cbc(
    const uint8_t* key, aes_implementation implementation) noexcept :
    round_keys_{}, inverse_round_keys_{},
    hardware_{
        implementation == aes_implementation::automatic &&
        hardware::aes_available()}
{
    const auto& sbox = tables().forward;

    std::memcpy(round_keys_.data(), key, aes256_key_length);

    uint8_t rcon = 1;
    for (int word = key_words; word < 4 * (aes256_rounds + 1); ++word)
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

    // Reverse the schedule, and fold InvMixColumns into every round key but
    // the two on the ends, which are only ever added and never mixed.
    for (int round = 0; round <= aes256_rounds; ++round)
    {
        auto* destination =
            inverse_round_keys_.data() + (aes_block_length * round);
        std::memcpy(
            destination,
            round_keys_.data() + (aes_block_length * (aes256_rounds - round)),
            aes_block_length);

        if (round != 0 && round != aes256_rounds)
            inverse_mix_columns(destination);
    }
}

void aes256_cbc::encrypt(
    const uint8_t* iv, const uint8_t* input, uint8_t* output,
    size_t length) const noexcept
{
    if (hardware_)
    {
        hardware::aes256_cbc_encrypt(
            round_keys_.data(), iv, input, output, length);
        return;
    }

    const auto& t = tables();

    uint32_t schedule[schedule_words];
    unpack_schedule(round_keys_.data(), schedule);

    uint8_t chain[aes_block_length];
    std::memcpy(chain, iv, aes_block_length);

    for (size_t offset = 0; offset < length; offset += aes_block_length)
    {
        uint8_t block[aes_block_length];
        for (size_t i = 0; i < aes_block_length; ++i)
            block[i] = input[offset + i] ^ chain[i];

        transform_block<forwards>(
            schedule, t.encrypt, t.forward, block, output + offset);
        std::memcpy(chain, output + offset, aes_block_length);
    }
}

void aes256_cbc::decrypt(
    const uint8_t* iv, const uint8_t* input, uint8_t* output,
    size_t length) const noexcept
{
    if (hardware_)
    {
        hardware::aes256_cbc_decrypt(
            inverse_round_keys_.data(), iv, input, output, length);
        return;
    }

    const auto& t = tables();

    uint32_t schedule[schedule_words];
    unpack_schedule(inverse_round_keys_.data(), schedule);

    uint8_t chain[aes_block_length];
    std::memcpy(chain, iv, aes_block_length);

    for (size_t offset = 0; offset < length; offset += aes_block_length)
    {
        // Retain the ciphertext before writing, in case output aliases input.
        uint8_t next_chain[aes_block_length];
        std::memcpy(next_chain, input + offset, aes_block_length);

        transform_block<backwards>(
            schedule, t.decrypt, t.inverse, input + offset, output + offset);
        for (size_t i = 0; i < aes_block_length; ++i)
            output[offset + i] ^= chain[i];

        std::memcpy(chain, next_chain, aes_block_length);
    }
}

}  // namespace djinterop::util::crypto
