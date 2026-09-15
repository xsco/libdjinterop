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

#include "sha512.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace djinterop::util::crypto
{
namespace
{
constexpr uint64_t k[80] = {
    0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL,
    0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL,
    0x12835b0145706fbeULL, 0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
    0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL, 0x2de92c6f592b0275ULL,
    0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL, 0xb00327c898fb213fULL,
    0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL,
    0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL,
    0x92722c851482353bULL, 0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
    0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL, 0x2748774cdf8eeb99ULL,
    0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL,
    0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL,
    0xc67178f2e372532bULL, 0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL,
    0x0a637dc5a2c898a6ULL, 0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
    0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL};

inline uint64_t rotr(uint64_t x, unsigned n) noexcept
{
    return (x >> n) | (x << (64 - n));
}

/// Extend the message schedule in place.  Only the last sixteen words are ever
/// needed, so they live in a window that wraps rather than in the array of
/// eighty the specification describes.
inline uint64_t extend(uint64_t* w, int j) noexcept
{
    const auto x = w[(j + 1) & 15];
    const auto y = w[(j + 14) & 15];
    w[j] += (rotr(x, 1) ^ rotr(x, 8) ^ (x >> 7)) + w[(j + 9) & 15] +
            (rotr(y, 19) ^ rotr(y, 61) ^ (y >> 6));
    return w[j];
}

/// One round, over working variables named in the order the round expects them.
///
/// The specification shifts the eight variables along by one each round; the
/// caller below rotates their *names* instead, which is free.  Sixteen rounds
/// are two whole turns of the eight, so a block ends with every name back
/// where it started.
#define DJINTEROP_SHA512_ROUND(a, b, c, d, e, f, g, h, word, constant)        \
    do                                                                        \
    {                                                                         \
        const uint64_t t1 =                                                   \
            (h) + (rotr((e), 14) ^ rotr((e), 18) ^ rotr((e), 41)) +           \
            (((e) & (f)) ^ (~(e) & (g))) + (constant) + (word);               \
        const uint64_t t2 = (rotr((a), 28) ^ rotr((a), 34) ^ rotr((a), 39)) + \
                            (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)));        \
        (d) += t1;                                                            \
        (h) = t1 + t2;                                                        \
    } while (false)

#define DJINTEROP_SHA512_BLOCK(word)                                         \
    do                                                                       \
    {                                                                        \
        DJINTEROP_SHA512_ROUND(a, b, c, d, e, f, g, h, word(0), k[i + 0]);   \
        DJINTEROP_SHA512_ROUND(h, a, b, c, d, e, f, g, word(1), k[i + 1]);   \
        DJINTEROP_SHA512_ROUND(g, h, a, b, c, d, e, f, word(2), k[i + 2]);   \
        DJINTEROP_SHA512_ROUND(f, g, h, a, b, c, d, e, word(3), k[i + 3]);   \
        DJINTEROP_SHA512_ROUND(e, f, g, h, a, b, c, d, word(4), k[i + 4]);   \
        DJINTEROP_SHA512_ROUND(d, e, f, g, h, a, b, c, word(5), k[i + 5]);   \
        DJINTEROP_SHA512_ROUND(c, d, e, f, g, h, a, b, word(6), k[i + 6]);   \
        DJINTEROP_SHA512_ROUND(b, c, d, e, f, g, h, a, word(7), k[i + 7]);   \
        DJINTEROP_SHA512_ROUND(a, b, c, d, e, f, g, h, word(8), k[i + 8]);   \
        DJINTEROP_SHA512_ROUND(h, a, b, c, d, e, f, g, word(9), k[i + 9]);   \
        DJINTEROP_SHA512_ROUND(g, h, a, b, c, d, e, f, word(10), k[i + 10]); \
        DJINTEROP_SHA512_ROUND(f, g, h, a, b, c, d, e, word(11), k[i + 11]); \
        DJINTEROP_SHA512_ROUND(e, f, g, h, a, b, c, d, word(12), k[i + 12]); \
        DJINTEROP_SHA512_ROUND(d, e, f, g, h, a, b, c, word(13), k[i + 13]); \
        DJINTEROP_SHA512_ROUND(c, d, e, f, g, h, a, b, word(14), k[i + 14]); \
        DJINTEROP_SHA512_ROUND(b, c, d, e, f, g, h, a, word(15), k[i + 15]); \
        i += 16;                                                             \
    } while (false)

/// The first sixteen rounds read the block as it arrives; the rest extend the
/// schedule a word at a time, just before the round that consumes it.
#define DJINTEROP_SHA512_LOADED(j) w[j]
#define DJINTEROP_SHA512_EXTENDED(j) extend(w, j)

inline uint64_t load_be64(const uint8_t* p) noexcept
{
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i)
        v = (v << 8) | p[i];
    return v;
}

inline void store_be64(uint8_t* p, uint64_t v) noexcept
{
    for (int i = 7; i >= 0; --i)
    {
        p[i] = static_cast<uint8_t>(v & 0xff);
        v >>= 8;
    }
}

}  // anonymous namespace

sha512::sha512() noexcept :
    state_{0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL,
           0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
           0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL},
    buffer_{}, buffered_{0}, total_length_{0}
{
}

void sha512::compress(const uint8_t* block) noexcept
{
    uint64_t w[16];
    for (int j = 0; j < 16; ++j)
        w[j] = load_be64(block + (j * 8));

    auto a = state_[0], b = state_[1], c = state_[2], d = state_[3];
    auto e = state_[4], f = state_[5], g = state_[6], h = state_[7];

    int i = 0;
    DJINTEROP_SHA512_BLOCK(DJINTEROP_SHA512_LOADED);
    DJINTEROP_SHA512_BLOCK(DJINTEROP_SHA512_EXTENDED);
    DJINTEROP_SHA512_BLOCK(DJINTEROP_SHA512_EXTENDED);
    DJINTEROP_SHA512_BLOCK(DJINTEROP_SHA512_EXTENDED);
    DJINTEROP_SHA512_BLOCK(DJINTEROP_SHA512_EXTENDED);

    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}

void sha512::update(const uint8_t* data, size_t length) noexcept
{
    total_length_ += length;

    if (buffered_ > 0)
    {
        const auto take = std::min(length, sha512_block_length - buffered_);
        std::memcpy(buffer_.data() + buffered_, data, take);
        buffered_ += take;
        data += take;
        length -= take;

        if (buffered_ < sha512_block_length)
            return;

        compress(buffer_.data());
        buffered_ = 0;
    }

    while (length >= sha512_block_length)
    {
        compress(data);
        data += sha512_block_length;
        length -= sha512_block_length;
    }

    std::memcpy(buffer_.data(), data, length);
    buffered_ = length;
}

sha512_digest sha512::finalise() noexcept
{
    // The length field is 128 bits wide, but inputs here are far below 2^64
    // bytes, so the high half is always zero.
    const uint64_t bit_length = total_length_ * 8;

    buffer_[buffered_++] = 0x80;
    if (buffered_ > sha512_block_length - 16)
    {
        std::fill(buffer_.begin() + buffered_, buffer_.end(), uint8_t{0});
        compress(buffer_.data());
        buffered_ = 0;
    }

    std::fill(buffer_.begin() + buffered_, buffer_.end() - 8, uint8_t{0});
    store_be64(buffer_.data() + sha512_block_length - 8, bit_length);
    compress(buffer_.data());

    sha512_digest digest{};
    for (int i = 0; i < 8; ++i)
        store_be64(digest.data() + (i * 8), state_[i]);
    return digest;
}

sha512_digest sha512::hash(const uint8_t* data, size_t length) noexcept
{
    sha512 h;
    h.update(data, length);
    return h.finalise();
}

hmac_sha512_key::hmac_sha512_key(const uint8_t* key, size_t key_length) noexcept
{
    std::array<uint8_t, sha512_block_length> padded{};
    if (key_length > sha512_block_length)
    {
        const auto digest = sha512::hash(key, key_length);
        std::copy(digest.begin(), digest.end(), padded.begin());
    }
    else
    {
        std::copy(key, key + key_length, padded.begin());
    }

    std::array<uint8_t, sha512_block_length> pad{};
    for (size_t i = 0; i < sha512_block_length; ++i)
        pad[i] = static_cast<uint8_t>(padded[i] ^ 0x36);
    inner_.update(pad.data(), pad.size());

    for (size_t i = 0; i < sha512_block_length; ++i)
        pad[i] = static_cast<uint8_t>(padded[i] ^ 0x5c);
    outer_.update(pad.data(), pad.size());
}

hmac_sha512_stream::hmac_sha512_stream(const hmac_sha512_key& key) noexcept :
    inner_{key.inner_}, outer_{key.outer_}
{
}

void hmac_sha512_stream::update(const uint8_t* data, size_t length) noexcept
{
    inner_.update(data, length);
}

sha512_digest hmac_sha512_stream::finalise() noexcept
{
    const auto inner_digest = inner_.finalise();
    outer_.update(inner_digest.data(), inner_digest.size());
    return outer_.finalise();
}

sha512_digest hmac_sha512(
    const uint8_t* key, size_t key_length, const uint8_t* data,
    size_t data_length) noexcept
{
    const hmac_sha512_key prepared{key, key_length};

    hmac_sha512_stream stream{prepared};
    stream.update(data, data_length);
    return stream.finalise();
}

std::vector<uint8_t> pbkdf2_hmac_sha512(
    const uint8_t* password, size_t password_length, const uint8_t* salt,
    size_t salt_length, uint32_t iterations, size_t length)
{
    if (iterations == 0)
        throw std::invalid_argument{"PBKDF2 requires at least one iteration"};

    // Every iteration below authenticates under the same key, so its padded
    // blocks are absorbed once here rather than a quarter of a million times.
    const hmac_sha512_key prf{password, password_length};

    std::vector<uint8_t> output;
    output.reserve(length);

    for (uint32_t index = 1; output.size() < length; ++index)
    {
        // U_1 = PRF(password, salt || INT_BE32(index))
        const uint8_t counter[4] = {
            static_cast<uint8_t>(index >> 24),
            static_cast<uint8_t>(index >> 16), static_cast<uint8_t>(index >> 8),
            static_cast<uint8_t>(index)};

        hmac_sha512_stream first{prf};
        first.update(salt, salt_length);
        first.update(counter, sizeof(counter));

        auto u = first.finalise();
        auto accumulator = u;

        for (uint32_t i = 1; i < iterations; ++i)
        {
            hmac_sha512_stream next{prf};
            next.update(u.data(), u.size());
            u = next.finalise();

            for (size_t j = 0; j < accumulator.size(); ++j)
                accumulator[j] ^= u[j];
        }

        const auto take = std::min(accumulator.size(), length - output.size());
        output.insert(
            output.end(), accumulator.begin(), accumulator.begin() + take);
    }

    return output;
}

}  // namespace djinterop::util::crypto

#undef DJINTEROP_SHA512_ROUND
#undef DJINTEROP_SHA512_BLOCK
#undef DJINTEROP_SHA512_LOADED
#undef DJINTEROP_SHA512_EXTENDED
