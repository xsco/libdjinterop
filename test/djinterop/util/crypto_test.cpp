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

#define BOOST_TEST_MODULE crypto_test
#include <boost/test/included/unit_test.hpp>

#include <cstring>
#include <string>
#include <vector>

#include "../../../src/djinterop/util/crypto/aes.hpp"
#include "../../../src/djinterop/util/crypto/sha512.hpp"
#include "../../../src/djinterop/util/crypto/sqlcipher_codec.hpp"
#include "../sqlcipher_encryptor.hpp"

namespace utf = boost::unit_test;

using namespace djinterop::util::crypto;

namespace
{
std::string to_hex(const uint8_t* data, size_t length)
{
    static const char* digits = "0123456789abcdef";
    std::string out;
    out.reserve(length * 2);
    for (size_t i = 0; i < length; ++i)
    {
        out.push_back(digits[data[i] >> 4]);
        out.push_back(digits[data[i] & 0x0f]);
    }
    return out;
}

const uint8_t* bytes_of(const std::string& s)
{
    return reinterpret_cast<const uint8_t*>(s.data());
}

std::vector<uint8_t> from_hex(const std::string& hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2)
    {
        out.push_back(
            static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
    }

    return out;
}

/// Both ways the cipher can do its work: a machine with AES instructions would
/// otherwise never reach the tables, and one without never the instructions, so
/// every test of the cipher runs over both.
const aes_implementation implementations[] = {
    aes_implementation::automatic, aes_implementation::tabulated};

/// Parameters with a cheap key derivation.
///
/// The real format stretches the passphrase 256,000 times, which is the point
/// of it, but paying that in every test makes the suite slow to no purpose,
/// especially in an unoptimised build.  Tests about the page format use these;
/// the tests about key derivation itself use the real ones.
sqlcipher_parameters cheap_parameters()
{
    sqlcipher_parameters params;
    params.kdf_iterations = 1000;
    return params;
}

}  // anonymous namespace

BOOST_TEST_DECORATOR(*utf::description("sha512 matches the published vectors"))
BOOST_AUTO_TEST_CASE(sha512__known_vectors__match)
{
    // FIPS 180-4 test vectors.
    const std::string abc = "abc";
    const auto digest = sha512::hash(bytes_of(abc), abc.size());
    BOOST_CHECK_EQUAL(
        to_hex(digest.data(), digest.size()),
        "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
        "2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");

    const std::string empty;
    const auto empty_digest = sha512::hash(bytes_of(empty), 0);
    BOOST_CHECK_EQUAL(
        to_hex(empty_digest.data(), empty_digest.size()),
        "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce"
        "47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}

BOOST_TEST_DECORATOR(
    *utf::description("sha512 streamed in chunks matches one shot"))
BOOST_AUTO_TEST_CASE(sha512__streamed_in_chunks__matches_one_shot)
{
    const std::string input(1000, 'x');
    const auto expected = sha512::hash(bytes_of(input), input.size());

    // Chunk sizes that divide neither the block length nor each other, so the
    // buffering path is exercised at every alignment.
    sha512 streamed;
    for (size_t offset = 0; offset < input.size(); offset += 7)
        streamed.update(
            bytes_of(input) + offset,
            std::min<size_t>(7, input.size() - offset));

    BOOST_CHECK(streamed.finalise() == expected);
}

BOOST_TEST_DECORATOR(
    *utf::description("hmac_sha512 matches the RFC 4231 vector"))
BOOST_AUTO_TEST_CASE(hmac_sha512__rfc_4231_vector__matches)
{
    const std::vector<uint8_t> key(20, 0x0b);
    const std::string data = "Hi There";
    const auto tag =
        hmac_sha512(key.data(), key.size(), bytes_of(data), data.size());

    BOOST_CHECK_EQUAL(
        to_hex(tag.data(), tag.size()),
        "87aa7cdea5ef619d4ff0b4241a1d6cb02379f4e2ce4ec2787ad0b30545e17cde"
        "daa833b7d6b8a702038b274eaea3f4e4be9d914eeb61f1702e696c203a126854");
}

BOOST_TEST_DECORATOR(
    *utf::description("pbkdf2_hmac_sha512 matches the published vector"))
BOOST_AUTO_TEST_CASE(pbkdf2_hmac_sha512__known_vector__matches)
{
    const std::string password = "passwd";
    const std::string salt = "salt";
    const auto derived = pbkdf2_hmac_sha512(
        bytes_of(password), password.size(), bytes_of(salt), salt.size(), 1,
        64);

    BOOST_CHECK_EQUAL(
        to_hex(derived.data(), derived.size()),
        "c74319d99499fc3e9013acff597c23c5baf0a0bec5634c46b8352b793e324723"
        "d55caa76b2b25c43402dcfdc06cdcf66f95b7d0429420b39520006749c51a04e");
}

BOOST_TEST_DECORATOR(*utf::description(
    "pbkdf2_hmac_sha512 spans several blocks for a long output"))
BOOST_AUTO_TEST_CASE(pbkdf2_hmac_sha512__long_output__spans_several_blocks)
{
    // More than one digest of output, so the block-index loop is exercised.
    const std::string password = "passwd";
    const std::string salt = "salt";
    const auto derived = pbkdf2_hmac_sha512(
        bytes_of(password), password.size(), bytes_of(salt), salt.size(), 1,
        100);

    BOOST_REQUIRE_EQUAL(derived.size(), 100u);

    // Its first 64 bytes are the same derivation as above.
    const auto shorter = pbkdf2_hmac_sha512(
        bytes_of(password), password.size(), bytes_of(salt), salt.size(), 1,
        64);
    BOOST_CHECK(
        std::vector<uint8_t>(derived.begin(), derived.begin() + 64) == shorter);
}

BOOST_TEST_DECORATOR(*utf::description("aes256 matches the FIPS 197 vector"))
BOOST_AUTO_TEST_CASE(aes256__fips_197_vector__matches)
{
    uint8_t key[aes256_key_length];
    for (size_t i = 0; i < sizeof(key); ++i)
        key[i] = static_cast<uint8_t>(i);

    const uint8_t plaintext[aes_block_length] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    const uint8_t zero_iv[aes_block_length] = {};

    for (const auto implementation : implementations)
    {
        // A single block under a zero vector is plain ECB, which is what the
        // published vector covers.
        uint8_t ciphertext[aes_block_length];
        const aes256_cbc cipher{key, implementation};
        cipher.encrypt(zero_iv, plaintext, ciphertext, sizeof(plaintext));
        BOOST_CHECK_EQUAL(
            to_hex(ciphertext, sizeof(ciphertext)),
            "8ea2b7ca516745bfeafc49904b496089");

        uint8_t recovered[aes_block_length];
        cipher.decrypt(zero_iv, ciphertext, recovered, sizeof(ciphertext));
        BOOST_CHECK_EQUAL(
            to_hex(recovered, sizeof(recovered)),
            to_hex(plaintext, sizeof(plaintext)));
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("aes256_cbc matches the NIST SP 800-38A vector"))
BOOST_AUTO_TEST_CASE(aes256_cbc__sp_800_38a_vector__matches)
{
    // F.2.5 and F.2.6 chain four blocks, and so pin the mode itself rather
    // than only the block cipher under it.
    const auto key = from_hex(
        "603deb1015ca71be2b73aef0857d7781"
        "1f352c073b6108d72d9810a30914dff4");
    const auto iv = from_hex("000102030405060708090a0b0c0d0e0f");
    const auto plaintext = from_hex(
        "6bc1bee22e409f96e93d7e117393172a"
        "ae2d8a571e03ac9c9eb76fac45af8e51"
        "30c81c46a35ce411e5fbc1191a0a52ef"
        "f69f2445df4f9b17ad2b417be66c3710");
    const std::string expected =
        "f58c4c04d6e5f1ba779eabfb5f7bfbd6"
        "9cfc4e967edb808d679f777bc6702c7d"
        "39f23369a9d9bacfa530e26304231461"
        "b2eb05e2c39be9fcda6c19078c6a9d1b";

    for (const auto implementation : implementations)
    {
        const aes256_cbc cipher{key.data(), implementation};

        std::vector<uint8_t> ciphertext(plaintext.size());
        cipher.encrypt(
            iv.data(), plaintext.data(), ciphertext.data(), plaintext.size());
        BOOST_CHECK_EQUAL(
            to_hex(ciphertext.data(), ciphertext.size()), expected);

        std::vector<uint8_t> recovered(ciphertext.size());
        cipher.decrypt(
            iv.data(), ciphertext.data(), recovered.data(), ciphertext.size());
        BOOST_CHECK(recovered == plaintext);
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("aes256_cbc round-trips multiple blocks"))
BOOST_AUTO_TEST_CASE(aes256_cbc__multiple_blocks__round_trip)
{
    uint8_t key[aes256_key_length];
    uint8_t iv[aes_block_length];
    for (size_t i = 0; i < sizeof(key); ++i)
        key[i] = static_cast<uint8_t>(i * 3);
    for (size_t i = 0; i < sizeof(iv); ++i)
        iv[i] = static_cast<uint8_t>(0xa0 + i);

    std::vector<uint8_t> plaintext(512);
    for (size_t i = 0; i < plaintext.size(); ++i)
        plaintext[i] = static_cast<uint8_t>(i * 7);

    for (const auto implementation : implementations)
    {
        const aes256_cbc cipher{key, implementation};

        std::vector<uint8_t> ciphertext(plaintext.size());
        cipher.encrypt(
            iv, plaintext.data(), ciphertext.data(), plaintext.size());
        BOOST_CHECK(ciphertext != plaintext);

        // Decryption in place must work, as the codec relies on it.
        std::vector<uint8_t> buffer = ciphertext;
        cipher.decrypt(iv, buffer.data(), buffer.data(), buffer.size());
        BOOST_CHECK(buffer == plaintext);
    }
}

BOOST_TEST_DECORATOR(*utf::description(
    "aes256_cbc decrypts every length the same way, whichever implementation"))
BOOST_AUTO_TEST_CASE(aes256_cbc__every_length__agrees_across_implementations)
{
    uint8_t key[aes256_key_length];
    uint8_t iv[aes_block_length];
    for (size_t i = 0; i < sizeof(key); ++i)
        key[i] = static_cast<uint8_t>(0x5a + i);
    for (size_t i = 0; i < sizeof(iv); ++i)
        iv[i] = static_cast<uint8_t>(i * 11);

    std::vector<uint8_t> ciphertext(24 * aes_block_length);
    for (size_t i = 0; i < ciphertext.size(); ++i)
        ciphertext[i] = static_cast<uint8_t>((i * 31) ^ 0x9c);

    const aes256_cbc hardware{key, aes_implementation::automatic};
    const aes256_cbc tabulated{key, aes_implementation::tabulated};

    // Decryption runs several blocks at a time where the processor allows it,
    // so lengths that do not divide by that group size exercise the tail.
    for (size_t blocks = 0; blocks <= 24; ++blocks)
    {
        const auto length = blocks * aes_block_length;

        std::vector<uint8_t> by_hardware(length);
        hardware.decrypt(iv, ciphertext.data(), by_hardware.data(), length);

        std::vector<uint8_t> by_tables(length);
        tabulated.decrypt(iv, ciphertext.data(), by_tables.data(), length);

        BOOST_CHECK(by_hardware == by_tables);
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("sqlcipher_codec round-trips a page losslessly"))
BOOST_AUTO_TEST_CASE(sqlcipher_codec__page_round_trip__is_lossless)
{
    const auto params = cheap_parameters();
    const sqlcipher_encryptor writer{"a passphrase", params};
    const sqlcipher_codec codec{"a passphrase", writer.salt(), params};

    std::vector<uint8_t> page(params.page_size, 0);
    for (size_t i = 0; i < params.payload_size(); ++i)
        page[i] = static_cast<uint8_t>((i * 31) & 0xff);

    for (uint32_t page_number : {1u, 2u, 4096u})
    {
        std::vector<uint8_t> encrypted(params.page_size);
        writer.encrypt_page(page_number, page.data(), encrypted.data());

        std::vector<uint8_t> decrypted(params.page_size);
        codec.decrypt_page(page_number, encrypted.data(), decrypted.data());

        // Page one carries the salt in place of the first sixteen bytes.
        const auto offset = page_number == 1 ? sqlcipher_salt_length : 0;
        BOOST_CHECK(
            std::memcmp(
                decrypted.data() + offset, page.data() + offset,
                params.payload_size() - offset) == 0);
    }
}

BOOST_TEST_DECORATOR(
    *utf::description("sqlcipher_codec rejects a tampered page"))
BOOST_AUTO_TEST_CASE(sqlcipher_codec__tampered_page__is_rejected)
{
    const auto params = cheap_parameters();
    const sqlcipher_encryptor writer{"a passphrase", params};
    const sqlcipher_codec codec{"a passphrase", writer.salt(), params};

    std::vector<uint8_t> page(params.page_size, 0x5a);
    std::vector<uint8_t> encrypted(params.page_size);
    writer.encrypt_page(2, page.data(), encrypted.data());

    encrypted[100] = static_cast<uint8_t>(encrypted[100] ^ 0x01);

    std::vector<uint8_t> decrypted(params.page_size);
    BOOST_CHECK_THROW(
        codec.decrypt_page(2, encrypted.data(), decrypted.data()),
        sqlcipher_error);
}

BOOST_TEST_DECORATOR(
    *utf::description("sqlcipher_codec rejects a wrong passphrase"))
BOOST_AUTO_TEST_CASE(sqlcipher_codec__wrong_passphrase__is_rejected)
{
    const auto params = cheap_parameters();
    const sqlcipher_encryptor writer{"the right one", params};
    const sqlcipher_codec reader{"the wrong one", writer.salt(), params};

    std::vector<uint8_t> page(params.page_size, 0x11);
    std::vector<uint8_t> encrypted(params.page_size);
    writer.encrypt_page(3, page.data(), encrypted.data());

    std::vector<uint8_t> decrypted(params.page_size);
    BOOST_CHECK_THROW(
        reader.decrypt_page(3, encrypted.data(), decrypted.data()),
        sqlcipher_error);
}

BOOST_TEST_DECORATOR(
    *utf::description("sqlcipher_codec binds the page number into the tag"))
BOOST_AUTO_TEST_CASE(sqlcipher_codec__page_number__is_bound_into_the_tag)
{
    const auto params = cheap_parameters();
    const sqlcipher_encryptor writer{"a passphrase", params};
    const sqlcipher_codec codec{"a passphrase", writer.salt(), params};

    std::vector<uint8_t> page(params.page_size, 0x22);
    std::vector<uint8_t> encrypted(params.page_size);
    writer.encrypt_page(7, page.data(), encrypted.data());

    std::vector<uint8_t> decrypted(params.page_size);
    BOOST_CHECK_NO_THROW(
        codec.decrypt_page(7, encrypted.data(), decrypted.data()));

    // The same bytes, read as a different page, must not verify.
    BOOST_CHECK_THROW(
        codec.decrypt_page(8, encrypted.data(), decrypted.data()),
        sqlcipher_error);
}
