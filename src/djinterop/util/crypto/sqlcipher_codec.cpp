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

#include "sqlcipher_codec.hpp"

#include <fstream>

#include <algorithm>
#include <cstring>

namespace djinterop::util::crypto
{
namespace
{
constexpr size_t page_key_length = aes256_key_length;
constexpr size_t hmac_key_length = 32;

/// Derive the page encryption key from the passphrase and the database salt.
std::vector<uint8_t> derive_page_key(
    const std::string& passphrase, const sqlcipher_salt& salt,
    uint32_t iterations)
{
    return pbkdf2_hmac_sha512(
        reinterpret_cast<const uint8_t*>(passphrase.data()), passphrase.size(),
        salt.data(), salt.size(), iterations, page_key_length);
}

/// Derive the HMAC key, and prepare it for the pages checked under it.
///
/// It comes from the *page key*, not the passphrase, using the database salt
/// with every byte XORed by 0x3a.
hmac_sha512_key make_hmac_key(
    const std::vector<uint8_t>& page_key, const sqlcipher_salt& salt,
    uint32_t iterations)
{
    sqlcipher_salt hmac_salt{};
    std::transform(
        salt.begin(), salt.end(), hmac_salt.begin(),
        [](uint8_t byte) { return static_cast<uint8_t>(byte ^ 0x3a); });

    const auto key = pbkdf2_hmac_sha512(
        page_key.data(), page_key.size(), hmac_salt.data(), hmac_salt.size(),
        iterations, hmac_key_length);

    return hmac_sha512_key{key.data(), key.size()};
}

/// Reject parameters that no SQLCipher database could have, before any time
/// is spent on key derivation.
const sqlcipher_parameters& validated(const sqlcipher_parameters& params)
{
    if (params.reserve < aes_block_length + sha512_digest_length)
        throw sqlcipher_error{
            "SQLCipher reserve is too small to hold an IV and an HMAC tag"};

    if (params.page_size <= params.reserve)
        throw sqlcipher_error{
            "SQLCipher page size does not exceed its reserved area"};

    if (params.payload_size() % aes_block_length != 0)
        throw sqlcipher_error{
            "SQLCipher page payload is not a whole number of AES blocks"};

    return params;
}

/// Compare two byte sequences without leaking their contents through timing.
bool equal_in_constant_time(
    const uint8_t* left, const uint8_t* right, size_t length) noexcept
{
    uint8_t difference = 0;
    for (size_t i = 0; i < length; ++i)
        difference |= static_cast<uint8_t>(left[i] ^ right[i]);
    return difference == 0;
}

}  // anonymous namespace

sqlcipher_codec::sqlcipher_codec(
    const std::string& passphrase, const sqlcipher_salt& salt,
    const sqlcipher_parameters& params) :
    sqlcipher_codec{
        derive_page_key(passphrase, salt, validated(params).kdf_iterations),
        salt, params}
{
}

sqlcipher_codec::sqlcipher_codec(
    const std::vector<uint8_t>& page_key, const sqlcipher_salt& salt,
    const sqlcipher_parameters& params) :
    params_{params}, cipher_{page_key.data()},
    hmac_key_{make_hmac_key(page_key, salt, params.hmac_kdf_iterations)}
{
}

sha512_digest sqlcipher_codec::page_mac(
    uint32_t page_number, const uint8_t* ciphertext, size_t length,
    const uint8_t* iv) const noexcept
{
    // The tag covers ciphertext || IV || page number, the last a little-endian
    // 32-bit integer.  The three are fed in as they lie rather than gathered
    // into a buffer, which would copy every page of a database for nothing.
    const uint8_t number[4] = {
        static_cast<uint8_t>(page_number),
        static_cast<uint8_t>(page_number >> 8),
        static_cast<uint8_t>(page_number >> 16),
        static_cast<uint8_t>(page_number >> 24)};

    hmac_sha512_stream stream{hmac_key_};
    stream.update(ciphertext, length);
    stream.update(iv, aes_block_length);
    stream.update(number, sizeof(number));
    return stream.finalise();
}

bool sqlcipher_codec::page_mac_is_valid(
    uint32_t page_number, const uint8_t* encrypted) const noexcept
{
    const auto offset = ciphertext_offset(page_number);
    const auto length = params_.payload_size() - offset;
    const auto* iv = encrypted + params_.payload_size();
    const auto* tag = iv + aes_block_length;

    const auto expected = page_mac(page_number, encrypted + offset, length, iv);
    return equal_in_constant_time(expected.data(), tag, sha512_digest_length);
}

void sqlcipher_codec::decrypt_page(
    uint32_t page_number, const uint8_t* encrypted, uint8_t* decrypted) const
{
    const auto offset = ciphertext_offset(page_number);
    const auto length = params_.payload_size() - offset;
    const auto* iv = encrypted + params_.payload_size();

    if (!page_mac_is_valid(page_number, encrypted))
        throw sqlcipher_error{
            "page " + std::to_string(page_number) +
            " failed its integrity check: wrong passphrase, corrupt data, or "
            "not a SQLCipher database"};

    if (offset > 0 && decrypted != encrypted)
        std::memcpy(decrypted, encrypted, offset);

    cipher_.decrypt(iv, encrypted + offset, decrypted + offset, length);

    // Leave the reserved area zeroed.  SQLite never looks at it once the
    // header declares a reserve size, and zeroing keeps the IV and tag of the
    // encrypted page from lingering in the decrypted image.
    std::memset(decrypted + params_.payload_size(), 0, params_.reserve);
}

namespace
{
/// Read the salt of a SQLCipher database from its first page.
sqlcipher_salt read_salt(const uint8_t* first_page) noexcept
{
    sqlcipher_salt salt{};
    std::copy(first_page, first_page + salt.size(), salt.begin());
    return salt;
}

/// Read the first page of a database, or nothing if there is not one.
std::optional<std::vector<uint8_t>> read_first_page(
    const std::string& database_path, size_t page_size)
{
    std::vector<uint8_t> page(page_size);
    std::ifstream file{database_path, std::ios::binary};
    if (!file.read(
            reinterpret_cast<char*>(page.data()),
            static_cast<std::streamsize>(page.size())))
        return std::nullopt;

    return page;
}

}  // anonymous namespace

std::optional<sqlcipher_codec> make_codec_for(
    const std::string& database_path, const std::string& passphrase,
    const sqlcipher_parameters& params)
{
    const auto page = read_first_page(database_path, params.page_size);
    if (!page)
        return std::nullopt;

    return sqlcipher_codec{passphrase, read_salt(page->data()), params};
}

}  // namespace djinterop::util::crypto
