#include "zip/Inflate.hpp"
#include "utils/ScopeExit.hpp"

#include <zlib.h>

#include <iostream> // temp, std::cerr

namespace zip
{

int
Inflate(
    utils::RdBuf_t Buf,
    std::function<void(unsigned char)> Cb
)
{
    //
    // https://www.zlib.net/zlib_how.html
    //

    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;

    strm.avail_in = 0;
    strm.next_in  = Z_NULL;

    constexpr int CHUNK = 1024;
    unsigned char out[CHUNK];

    int ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
    {
        std::cerr << "bad inflateInit2\n";
        return -1;
    }

    utils::ScopeExit cleanup{ [&strm]()
                              { inflateEnd(&strm); } };

    /* decompress until deflate stream ends or end of file */
    do
    {
        strm.avail_in = Buf.size();
        strm.next_in  = (decltype(strm.next_in))Buf.data();
        do
        {
            strm.avail_out = CHUNK;
            strm.next_out  = out;
            ret            = inflate(&strm, Z_NO_FLUSH);
            switch (ret)
            {
                case Z_STREAM_ERROR:
                    // assert(false);
                    return -1;
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR; /* and fall through */
                    [[fallthrough]];
                case Z_DATA_ERROR:
                    [[fallthrough]];
                case Z_MEM_ERROR:
                    return -1;
            }
            unsigned have = CHUNK - strm.avail_out;
            for (size_t i = 0; i < have; ++i)
            {
                Cb(static_cast<unsigned char>(out[i]));
            }

        } while (strm.avail_out == 0);

        break;

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    return 0;
}

}
