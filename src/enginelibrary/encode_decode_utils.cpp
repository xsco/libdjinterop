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

#include <zlib.h>
#include <algorithm>
#include <stdexcept>
#include <system_error>
#include <vector>

#include "enginelibrary/encode_decode_utils.hpp"

namespace djinterop
{
namespace enginelibrary
{
// Uncompress a zlib'ed BLOB
std::vector<char> zlib_uncompress(
    const std::vector<char>& compressed, std::vector<char> uncompressed)
{
    if (compressed.size() > 0 && compressed.size() < 4)
        throw std::length_error(
            "Compressed data is less than the minimum size of 4 bytes");

    uncompressed.clear();

    auto apparent_size =
        compressed.size() == 0 ? 0 : decode_int32_be(compressed.data()).first;

    if (apparent_size == 0)
    {
        // No data, which is a valid situation
        return uncompressed;  // Named RVO
    }

    uncompressed.reserve(apparent_size);

    const char* ptr = &compressed[4];
    const char* end = ptr + compressed.size();
    const int chunk_size = 16384;
    int ret;
    unsigned int have;
    unsigned char out[chunk_size];

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    ret = inflateInit(&strm);
    if (ret != Z_OK)
        throw std::system_error{ret, std::system_category(),
                                "Error calling inflateInit from zlib"};

    // Take chunks from the input vector
    do
    {
        strm.next_in = (Bytef*)ptr;
        strm.avail_in = (ptr + chunk_size) < end ? chunk_size : end - ptr;
        ptr += strm.avail_in;

        // Run inflate() on until we are no longer filling the output buffer
        do
        {
            strm.next_out = out;
            strm.avail_out = chunk_size;

            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret)
            {
                case Z_NEED_DICT: ret = Z_DATA_ERROR;  // Fall through
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&strm);
                    throw std::system_error{ret, std::system_category(),
                                            "Error calling inflate from zlib"};
            }

            have = chunk_size - strm.avail_out;
            uncompressed.insert(uncompressed.end(), out, out + have);
        } while (strm.avail_out == 0);

        // done when inflate() says it's done
    } while (ret != Z_STREAM_END);

    // Clean up
    inflateEnd(&strm);
    if (ret != Z_STREAM_END)
    {
        throw std::system_error{Z_DATA_ERROR, std::system_category(),
                                "Error at end of inflation"};
    }

    return uncompressed;  // Named RVO
}

// Compress a byte array using zlib
std::vector<char> zlib_compress(
    const std::vector<char>& uncompressed, std::vector<char> compressed)
{
    // Put a placeholder four bytes to hold the uncompressed buffer size
    compressed.resize(4);
    encode_int32_be(uncompressed.size(), compressed.data());

    // Compress
    const char* ptr = &uncompressed[0];
    const char* end = ptr + uncompressed.size();
    const int chunk_size = 16384;
    int ret, flush;
    unsigned int have;
    z_stream strm;
    unsigned char out[chunk_size];

    // Allocate deflate state
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK)
        throw std::system_error{ret, std::system_category(),
                                "Error calling deflateInit from zlib"};

    // Compress until end of input
    do
    {
        strm.next_in = (Bytef*)ptr;
        if (ptr + chunk_size < end)
        {
            strm.avail_in = chunk_size;
            flush = Z_NO_FLUSH;
        }
        else
        {
            strm.avail_in = end - ptr;
            flush = Z_FINISH;
        }
        ptr += strm.avail_in;

        // Run deflate() on input until output buffer not full, finish
        // compression if all of source has been read in
        do
        {
            strm.next_out = out;
            strm.avail_out = chunk_size;

            ret = deflate(&strm, flush); /* no bad return value */

            have = chunk_size - strm.avail_out;
            compressed.insert(compressed.end(), out, out + have);
        } while (strm.avail_out == 0);

        // Done when last data in input processed
    } while (flush != Z_FINISH);

    // Clean up and return
    deflateEnd(&strm);
    return compressed;
}

}  // namespace enginelibrary
}  // namespace djinterop
