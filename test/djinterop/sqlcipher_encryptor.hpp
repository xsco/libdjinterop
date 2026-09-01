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
#include <cstdint>
#include <cstring>
#include <random>
#include <string>
#include <vector>

#include "../../src/djinterop/util/crypto/aes.hpp"
#include "../../src/djinterop/util/crypto/sha512.hpp"
#include "../../src/djinterop/util/crypto/sqlcipher_codec.hpp"

/// Writes SQLCipher 4 pages, so that a test can build a database for the
/// library to read.
///
/// The library only ever decrypts, so the encrypting direction lives here
/// rather than beside it.  Writing the pages from the format description
/// independently also makes a round trip a check of the format, rather than a
/// check that the library agrees with itself.
class sqlcipher_encryptor
{
public:
    using parameters = djinterop::util::crypto::sqlcipher_parameters;
    using salt_type = djinterop::util::crypto::sqlcipher_salt;

    explicit sqlcipher_encryptor(
        const std::string& passphrase, const parameters& params = {}) :
        params_{params},
        salt_{random_bytes<salt_type>()},
        page_key_{djinterop::util::crypto::pbkdf2_hmac_sha512(
            reinterpret_cast<const uint8_t*>(passphrase.data()),
            passphrase.size(), salt_.data(), salt_.size(),
            params.kdf_iterations, djinterop::util::crypto::aes256_key_length)},
        hmac_key_{derive_hmac_key()}, cipher_{page_key_.data()}
    {
    }

    /// The parameters the pages are written under.
    [[nodiscard]] const parameters& params() const noexcept { return params_; }

    /// The salt written into the first page, with which a codec that is to
    /// read the database has to be built.
    [[nodiscard]] const salt_type& salt() const noexcept { return salt_; }

    /// Encrypt one page, of `page_size` bytes, under a fresh random vector.
    void encrypt_page(
        uint32_t page_number, const uint8_t* decrypted,
        uint8_t* encrypted) const
    {
        using namespace djinterop::util::crypto;

        // Page one carries the salt where a plain SQLite file has its magic,
        // so its ciphertext starts after it.
        const auto offset = page_number == 1 ? sqlcipher_salt_length : 0;
        const auto length = params_.payload_size() - offset;
        const auto iv = random_bytes<std::array<uint8_t, aes_block_length>>();

        cipher_.encrypt(
            iv.data(), decrypted + offset, encrypted + offset, length);

        if (page_number == 1)
            std::memcpy(encrypted, salt_.data(), salt_.size());

        // The tag covers ciphertext || IV || page number, the last as a
        // little-endian 32-bit integer, which binds a page to its position.
        std::vector<uint8_t> message;
        message.insert(
            message.end(), encrypted + offset, encrypted + offset + length);
        message.insert(message.end(), iv.begin(), iv.end());
        for (int shift : {0, 8, 16, 24})
            message.push_back(static_cast<uint8_t>(page_number >> shift));

        const auto tag = hmac_sha512(
            hmac_key_.data(), hmac_key_.size(), message.data(), message.size());

        auto* reserve = encrypted + params_.payload_size();
        std::memcpy(reserve, iv.data(), iv.size());
        std::memcpy(reserve + iv.size(), tag.data(), tag.size());

        const auto used = iv.size() + tag.size();
        if (params_.reserve > used)
            std::memset(reserve + used, 0, params_.reserve - used);
    }

private:
    template <typename Bytes> static Bytes random_bytes()
    {
        static std::random_device rng;
        static std::uniform_int_distribution<int> byte_dist{0, 255};

        Bytes bytes{};
        for (auto& byte : bytes)
            byte = static_cast<uint8_t>(byte_dist(rng));

        return bytes;
    }

    /// The tag key is derived from the page key, under the salt with every
    /// byte flipped by 0x3a, as SQLCipher does it.
    [[nodiscard]] std::vector<uint8_t> derive_hmac_key() const
    {
        salt_type hmac_salt{};
        for (size_t index = 0; index < salt_.size(); ++index)
            hmac_salt[index] = static_cast<uint8_t>(salt_[index] ^ 0x3a);

        return djinterop::util::crypto::pbkdf2_hmac_sha512(
            page_key_.data(), page_key_.size(), hmac_salt.data(),
            hmac_salt.size(), params_.hmac_kdf_iterations, 32);
    }

    parameters params_;
    salt_type salt_;
    std::vector<uint8_t> page_key_;
    std::vector<uint8_t> hmac_key_;
    djinterop::util::crypto::aes256_cbc cipher_;
};
