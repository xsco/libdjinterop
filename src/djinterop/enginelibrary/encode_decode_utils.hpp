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

#include <cstdint>
#include <cstring>
#include <tuple>
#include <vector>

namespace djinterop
{
namespace enginelibrary
{
// Uncompress a zlib'ed BLOB
std::vector<char> zlib_uncompress(
    const std::vector<char>& compressed, std::vector<char> uncompressed = {});

// Compress a byte array using zlib
std::vector<char> zlib_compress(
    const std::vector<char>& uncompressed, std::vector<char> compressed = {});

// Extract an int8_t from a raw value at ptr address
inline std::pair<uint8_t, const char*> decode_uint8(const char* ptr)
{
    return {static_cast<uint8_t>(*ptr), ptr + 1};
}

// Encode an uint8_t as a raw byte to an output pointer
inline char* encode_uint8(uint8_t value, char* ptr)
{
    *ptr = static_cast<char>(value);
    return ptr + 1;
}

// Decode an int32_t from a little-endian encoded raw value at ptr address
inline std::pair<int32_t, const char*> decode_int32_le(const char* ptr)
{
    int32_t value = static_cast<int32_t>(static_cast<uint8_t>(ptr[0])) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[1]) << 8) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[2]) << 16) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[3]) << 24);
    return {value, ptr + 4};
}

// Encode an int32_t as 4 raw bytes to an output pointer with little-endianness
inline char* encode_int32_le(int32_t value, char* ptr)
{
    ptr[0] = static_cast<char>(value);
    ptr[1] = static_cast<char>(value >> 8);
    ptr[2] = static_cast<char>(value >> 16);
    ptr[3] = static_cast<char>(value >> 24);
    return ptr + 4;
}

// Decode an int32_t from a big-endian encoded raw value at ptr address
inline std::pair<int32_t, const char*> decode_int32_be(const char* ptr)
{
    int32_t value = static_cast<int32_t>(static_cast<uint8_t>(ptr[0]) << 24) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[1]) << 16) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[2]) << 8) |
                    static_cast<int32_t>(static_cast<uint8_t>(ptr[3]));
    return {value, ptr + 4};
}

// Encode an int32_t as 4 raw bytes to an output pointer with big-endianness
inline char* encode_int32_be(int32_t value, char* ptr)
{
    ptr[0] = static_cast<char>(value >> 24);
    ptr[1] = static_cast<char>(value >> 16);
    ptr[2] = static_cast<char>(value >> 8);
    ptr[3] = static_cast<char>(value);
    return ptr + 4;
}

// Decode an int64_t from a little-endian encoded raw value at ptr address
inline std::pair<int64_t, const char*> decode_int64_le(const char* ptr)
{
    int32_t e1, e2;
    std::tie(e1, ptr) = decode_int32_le(ptr);
    std::tie(e2, ptr) = decode_int32_le(ptr);
    int64_t value = static_cast<int64_t>(static_cast<uint32_t>(e1)) |
                    static_cast<int64_t>(static_cast<uint32_t>(e2)) << 32;
    return {value, ptr};
}

// Encode an int64_t as 4 raw bytes to an output pointer with little-endianness
inline char* encode_int64_le(int64_t value, char* ptr)
{
    ptr = encode_int32_le(static_cast<int32_t>(value), ptr);
    ptr = encode_int32_le(static_cast<int32_t>(value >> 32), ptr);
    return ptr;
}

// Decode an int64_t from a big-endian encoded raw value at ptr address
inline std::pair<int64_t, const char*> decode_int64_be(const char* ptr)
{
    int32_t e1, e2;
    std::tie(e1, ptr) = decode_int32_be(ptr);
    std::tie(e2, ptr) = decode_int32_be(ptr);
    int64_t value = static_cast<int64_t>(static_cast<uint32_t>(e1)) << 32 |
                    static_cast<int64_t>(static_cast<uint32_t>(e2));
    return {value, ptr};
}

// Encode an int64_t as 4 raw bytes to an output pointer with big-endianness
inline char* encode_int64_be(int64_t value, char* ptr)
{
    ptr = encode_int32_be(static_cast<int32_t>(value >> 32), ptr);
    ptr = encode_int32_be(static_cast<int32_t>(value), ptr);
    return ptr;
}

// Decode a double from a little-endian encoded raw value at ptr address
inline std::pair<double, const char*> decode_double_le(const char* ptr)
{
    int64_t value;
    std::tie(value, ptr) = decode_int64_le(ptr);
    double result;
    std::memcpy(&result, &value, 8);
    return {result, ptr};
}

// Encode a double as 8 raw bytes to an output pointer with little-endianness
inline char* encode_double_le(double value, char* ptr)
{
    int64_t temp;
    std::memcpy(&temp, &value, 8);
    return encode_int64_le(temp, ptr);
}

// Decode a double from a big-endian encoded raw value at ptr address
inline std::pair<double, const char*> decode_double_be(const char* ptr)
{
    int64_t value;
    std::tie(value, ptr) = decode_int64_be(ptr);
    double result;
    std::memcpy(&result, &value, 8);
    return {result, ptr};
}

// Encode a double as 8 raw bytes to an output pointer with big-endianness
inline char* encode_double_be(double value, char* ptr)
{
    int64_t temp;
    std::memcpy(&temp, &value, 8);
    return encode_int64_be(temp, ptr);
}

}  // namespace enginelibrary
}  // namespace djinterop
