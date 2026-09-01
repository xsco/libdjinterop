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
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "aes.hpp"
#include "sha512.hpp"

namespace djinterop::util::crypto
{
/// Parameters of a SQLCipher database.
///
/// The defaults are those of SQLCipher 4, which is what the AlphaTheta
/// OneLibrary `exportLibrary.db` is written with.
struct sqlcipher_parameters
{
    /// Size of a database page, in bytes, including the reserved area.
    size_t page_size = 4096;

    /// Bytes reserved at the end of each page: a 16-byte initialisation
    /// vector followed by a 64-byte HMAC-SHA-512 tag.
    size_t reserve = 80;

    /// Number of PBKDF2 iterations used to derive the page key.
    uint32_t kdf_iterations = 256000;

    /// Number of PBKDF2 iterations used to derive the HMAC key.  The input is
    /// already a strong key, so stretching it further would cost time for
    /// nothing.
    uint32_t hmac_kdf_iterations = 2;

    [[nodiscard]] size_t payload_size() const noexcept
    {
        return page_size - reserve;
    }
};

constexpr size_t sqlcipher_salt_length = 16;

using sqlcipher_salt = std::array<uint8_t, sqlcipher_salt_length>;

/// The bytes that open a plain SQLite file, and which SQLCipher overwrites
/// with the key derivation salt.
///
/// The two occupy the same 16 bytes, so a decrypted first page becomes a
/// loadable one by copying this over its salt.
constexpr const char sqlite_file_magic[] = "SQLite format 3";
constexpr size_t sqlite_file_magic_length = sqlcipher_salt_length;

/// Thrown when a page fails to decrypt, or fails its integrity check.
class sqlcipher_error : public std::runtime_error
{
public:
    explicit sqlcipher_error(const std::string& what) : std::runtime_error{what}
    {
    }
};

/// Decrypts the pages of a SQLCipher database.
///
/// Each page holds `payload_size()` bytes of ciphertext, a random
/// initialisation vector, and an HMAC-SHA-512 tag over the ciphertext, the
/// vector, and the page number -- which binds a page to its position, so pages
/// cannot be swapped without detection.
///
/// The first page is special: its leading 16 bytes are the key derivation
/// salt, in the clear where a plain SQLite file has its header magic, so its
/// ciphertext is 16 bytes shorter than that of every other page.
class sqlcipher_codec
{
public:
    /// Construct a codec for a passphrase and the salt of a database.
    ///
    /// Key derivation is deliberately expensive, so construct a codec once per
    /// database rather than once per page.
    sqlcipher_codec(
        const std::string& passphrase, const sqlcipher_salt& salt,
        const sqlcipher_parameters& params = {});

    [[nodiscard]] const sqlcipher_parameters& parameters() const noexcept
    {
        return params_;
    }

    /// Decrypt one page.
    ///
    /// `encrypted` and `decrypted` are both `page_size` bytes long and may
    /// alias each other.  Page numbers are one-based, as in SQLite itself, and
    /// the reserved area of the output is zeroed.
    ///
    /// On page one the salt is left in place, so a caller that needs a
    /// loadable SQLite image must overwrite those bytes with the header magic.
    void decrypt_page(
        uint32_t page_number, const uint8_t* encrypted,
        uint8_t* decrypted) const;

private:
    /// Test whether a page carries a valid HMAC tag.
    ///
    /// A wrong passphrase fails here rather than yielding plausible noise.
    [[nodiscard]] bool page_mac_is_valid(
        uint32_t page_number, const uint8_t* encrypted) const noexcept;

    /// Adopt an already-derived page key.  The HMAC key comes from the page
    /// key rather than the passphrase, so the public constructor derives once
    /// and delegates here.
    sqlcipher_codec(
        const std::vector<uint8_t>& page_key, const sqlcipher_salt& salt,
        const sqlcipher_parameters& params);

    /// Offset within a page at which its ciphertext begins.
    [[nodiscard]] size_t ciphertext_offset(uint32_t page_number) const noexcept
    {
        return page_number == 1 ? sqlcipher_salt_length : 0;
    }

    [[nodiscard]] sha512_digest page_mac(
        uint32_t page_number, const uint8_t* ciphertext, size_t length,
        const uint8_t* iv) const noexcept;

    sqlcipher_parameters params_;
    aes256_cbc cipher_;

    /// The HMAC key, its padded blocks already absorbed.
    hmac_sha512_key hmac_key_;
};

/// Build the codec of a database, reading its salt from the file.
///
/// Returns nothing if the file cannot be read or is shorter than a page.  Key
/// derivation is deliberately expensive, so a caller that goes on to read more
/// than one page should build the codec once, here, and keep it.
///
/// \throws sqlcipher_error If the key cannot be derived.
std::optional<sqlcipher_codec> make_codec_for(
    const std::string& database_path, const std::string& passphrase,
    const sqlcipher_parameters& params = {});

}  // namespace djinterop::util::crypto
