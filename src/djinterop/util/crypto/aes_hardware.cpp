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

#include "aes_hardware.hpp"

#include "aes.hpp"

// One of these is defined by the build system, if it found a way to compile the
// intrinsics for the target.  Anything else falls back to the tables in
// `aes.cpp`.
#if defined(DJINTEROP_AES_INTRINSICS_X86)
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <immintrin.h>
#endif
#elif defined(DJINTEROP_AES_INTRINSICS_ARM64)
#include <arm_neon.h>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <sys/auxv.h>
#if !defined(HWCAP_AES)
#define HWCAP_AES (1 << 3)
#endif
#elif defined(__FreeBSD__)
#include <machine/elf.h>
#include <sys/auxv.h>
#endif
#endif

#if defined(DJINTEROP_AES_INTRINSICS_X86) || \
    defined(DJINTEROP_AES_INTRINSICS_ARM64)
#define DJINTEROP_AES_INTRINSICS 1
#endif

namespace djinterop::util::crypto::hardware
{
#if defined(DJINTEROP_AES_INTRINSICS)

namespace
{
// Each architecture supplies the same handful of operations, over which the
// chaining below is written once.  Rounds are counted as x86 counts them:
// AArch64 adds its round key at the start of a round rather than the end, so
// its idiom looks a round out of step while doing the same work.

#if defined(DJINTEROP_AES_INTRINSICS_X86)

using block = __m128i;

inline block load(const uint8_t* p) noexcept
{
    return _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
}

inline void store(uint8_t* p, block x) noexcept
{
    _mm_storeu_si128(reinterpret_cast<__m128i*>(p), x);
}

inline block block_xor(block a, block b) noexcept
{
    return _mm_xor_si128(a, b);
}

inline block encrypt_first(block x, const block* rk) noexcept
{
    return _mm_xor_si128(x, rk[0]);
}

inline block encrypt_round(block x, const block* rk, int round) noexcept
{
    return _mm_aesenc_si128(x, rk[round + 1]);
}

inline block encrypt_last(block x, const block* rk) noexcept
{
    return _mm_aesenclast_si128(x, rk[aes256_rounds]);
}

inline block decrypt_first(block x, const block* dk) noexcept
{
    return _mm_xor_si128(x, dk[0]);
}

inline block decrypt_round(block x, const block* dk, int round) noexcept
{
    return _mm_aesdec_si128(x, dk[round + 1]);
}

inline block decrypt_last(block x, const block* dk) noexcept
{
    return _mm_aesdeclast_si128(x, dk[aes256_rounds]);
}

#else

using block = uint8x16_t;

inline block load(const uint8_t* p) noexcept
{
    return vld1q_u8(p);
}

inline void store(uint8_t* p, block x) noexcept
{
    vst1q_u8(p, x);
}

inline block block_xor(block a, block b) noexcept
{
    return veorq_u8(a, b);
}

inline block encrypt_first(block x, const block*) noexcept
{
    return x;
}

inline block encrypt_round(block x, const block* rk, int round) noexcept
{
    return vaesmcq_u8(vaeseq_u8(x, rk[round]));
}

inline block encrypt_last(block x, const block* rk) noexcept
{
    return veorq_u8(vaeseq_u8(x, rk[aes256_rounds - 1]), rk[aes256_rounds]);
}

inline block decrypt_first(block x, const block*) noexcept
{
    return x;
}

inline block decrypt_round(block x, const block* dk, int round) noexcept
{
    return vaesimcq_u8(vaesdq_u8(x, dk[round]));
}

inline block decrypt_last(block x, const block* dk) noexcept
{
    return veorq_u8(vaesdq_u8(x, dk[aes256_rounds - 1]), dk[aes256_rounds]);
}

#endif

/// Rounds between the first and the last, of which both directions have the
/// same number.
constexpr int middle_rounds = aes256_rounds - 1;

/// Blocks decrypted together.
///
/// The AES instructions take several cycles to yield a result but accept a new
/// block every cycle, so independent blocks run several times faster than a
/// dependent chain does.  Eight covers the latency without exhausting the
/// sixteen vector registers both architectures have.
constexpr size_t lanes = 8;

void load_schedule(const uint8_t* keys, block* out) noexcept
{
    for (int i = 0; i <= aes256_rounds; ++i)
        out[i] = load(keys + (aes_block_length * i));
}

inline block decrypt_block(block x, const block* dk) noexcept
{
    x = decrypt_first(x, dk);
    for (int round = 0; round < middle_rounds; ++round)
        x = decrypt_round(x, dk, round);

    return decrypt_last(x, dk);
}

}  // anonymous namespace

void aes256_cbc_encrypt(
    const uint8_t* round_keys, const uint8_t* iv, const uint8_t* input,
    uint8_t* output, size_t length) noexcept
{
    block rk[aes256_rounds + 1];
    load_schedule(round_keys, rk);

    auto chain = load(iv);
    for (size_t offset = 0; offset < length; offset += aes_block_length)
    {
        auto x = encrypt_first(block_xor(load(input + offset), chain), rk);
        for (int round = 0; round < middle_rounds; ++round)
            x = encrypt_round(x, rk, round);

        chain = encrypt_last(x, rk);
        store(output + offset, chain);
    }
}

void aes256_cbc_decrypt(
    const uint8_t* inverse_round_keys, const uint8_t* iv, const uint8_t* input,
    uint8_t* output, size_t length) noexcept
{
    block dk[aes256_rounds + 1];
    load_schedule(inverse_round_keys, dk);

    auto chain = load(iv);
    size_t offset = 0;

    constexpr size_t stride = lanes * aes_block_length;
    for (; offset + stride <= length; offset += stride)
    {
        // Every block of the group is read before any is written, so the
        // ciphertext each one chains with survives an output that aliases the
        // input.
        block ciphertext[lanes];
        block x[lanes];
        for (size_t lane = 0; lane < lanes; ++lane)
        {
            ciphertext[lane] = load(input + offset + (lane * aes_block_length));
            x[lane] = decrypt_first(ciphertext[lane], dk);
        }

        for (int round = 0; round < middle_rounds; ++round)
            for (auto& lane : x)
                lane = decrypt_round(lane, dk, round);

        for (auto& lane : x)
            lane = decrypt_last(lane, dk);

        x[0] = block_xor(x[0], chain);
        for (size_t lane = 1; lane < lanes; ++lane)
            x[lane] = block_xor(x[lane], ciphertext[lane - 1]);

        chain = ciphertext[lanes - 1];
        for (size_t lane = 0; lane < lanes; ++lane)
            store(output + offset + (lane * aes_block_length), x[lane]);
    }

    for (; offset < length; offset += aes_block_length)
    {
        const auto ciphertext = load(input + offset);
        store(output + offset, block_xor(decrypt_block(ciphertext, dk), chain));
        chain = ciphertext;
    }
}

bool aes_available() noexcept
{
#if defined(DJINTEROP_AES_INTRINSICS_X86) && defined(_MSC_VER)
    // AES-NI is bit 25 of ECX for CPUID leaf 1.
    static const bool available = []
    {
        int registers[4] = {0, 0, 0, 0};
        __cpuid(registers, 1);
        return (registers[2] & (1 << 25)) != 0;
    }();
    return available;
#elif defined(DJINTEROP_AES_INTRINSICS_X86)
    static const bool available = __builtin_cpu_supports("aes");
    return available;
#elif defined(__APPLE__)
    // Every processor Apple has shipped in an arm64 device implements the
    // cryptographic extension, and the platform guarantees it.
    return true;
#elif defined(_WIN32)
    static const bool available =
        IsProcessorFeaturePresent(PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE) !=
        FALSE;
    return available;
#elif defined(__linux__)
    static const bool available = (getauxval(AT_HWCAP) & HWCAP_AES) != 0;
    return available;
#elif defined(__FreeBSD__)
    static const bool available = []
    {
        unsigned long capabilities = 0;
        if (elf_aux_info(AT_HWCAP, &capabilities, sizeof(capabilities)) != 0)
            return false;

        return (capabilities & HWCAP_AES) != 0;
    }();
    return available;
#else
    // With no way to ask, assume not: the fallback is correct, only slower.
    return false;
#endif
}

#else

bool aes_available() noexcept
{
    return false;
}

// Never reached: `aes_available` says so, and every caller asks first.

void aes256_cbc_encrypt(
    const uint8_t*, const uint8_t*, const uint8_t*, uint8_t*, size_t) noexcept
{
}

void aes256_cbc_decrypt(
    const uint8_t*, const uint8_t*, const uint8_t*, uint8_t*, size_t) noexcept
{
}

#endif

}  // namespace djinterop::util::crypto::hardware
