#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"
#include "zipEOCDRec.hpp"
#include "zipCDFHeader.hpp"
#include "ForEachFindEnd.hpp"
#include "MemoryMappedFile.hpp"
#include "ScopeExit.hpp"

#include <cassert>
#include <iostream>

constexpr
bool Validate(const zip::EOCDRec& r, size_t zipFileSize)
{
    return
        AsBytes<uint32_t, unsigned char>(r.sig) == zip::EOCDRec::SIG
        && r.thisDiskNum == 0u
        && r.startDiskNum == 0u
        && r.totalEntries == r.totalEntriesThisDisk
        && r.commentLen == r.comment.size()
        && r.offsetOfCentralDir < zipFileSize
        && r.sizeOfCentralDir <= zipFileSize - r.offsetOfCentralDir
        && r.sizeOfCentralDir <= r.totalEntries * zip::CDFHeader::MaxBytes()
        && r.sizeOfCentralDir >= r.totalEntries * zip::CDFHeader::MinBytes();
}


constexpr
bool Validate(const zip::CDFHeader& r, const zip::EOCDRec& eocd, size_t zipFileSize)
{
    return
        AsBytes<uint32_t, unsigned char>(r.sig) == zip::CDFHeader::SIG
        && r.diskNum == 0u
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.commentLen == r.comment.size()
        && r.offsetOfLFHeader < zipFileSize
        && r.offsetOfLFHeader < eocd.offsetOfCentralDir
        && eocd.offsetOfCentralDir - r.offsetOfLFHeader >= zip::LFHeader::MinBytes();
}


constexpr
bool Validate(const zip::LFHeader& r, const zip::CDFHeader& cdfh, const zip::EOCDRec& eocd, size_t zipFileSize)
{
    return
        AsBytes<uint32_t, unsigned char>(r.sig) == zip::LFHeader::SIG
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.compressedSz < zipFileSize
        && r.compressedSz < eocd.offsetOfCentralDir
        ;
}

#include <zlib.h>

int
Inflate (
    RdBuf_t Buf
    )
{
    //
    // https://www.zlib.net/zlib_how.html
    //

    std::cout << "==============================================" << std::endl;
    ScopeExit cleanup2{ []() {
    std::cout << "\n";
    std::cout << "==============================================" << std::endl;
    } };

    z_stream strm{};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    constexpr int CHUNK = 1024;
    unsigned char out[CHUNK];

    int ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
    {
        std::cout << "bad inflateInit2\n";
        return -1;
    }

    ScopeExit cleanup{ [&strm]() { inflateEnd(&strm); } };

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = Buf.size();
        strm.next_in = (decltype(strm.next_in))Buf.data();
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
                case Z_STREAM_ERROR:
                    // assert(false);
                    return -1;
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    return -1;
            }
            unsigned have = CHUNK - strm.avail_out;
            for (size_t i = 0; i < have; ++i)
                std::cout << out[i];

        } while (strm.avail_out == 0);

        break;

      /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <zip-file>\n";
        return -1;
    }

    const char* fname = argv[1];

    {
        MemoryMappedFile zipFile{fname};
        if (!zipFile.IsValid())
        {
            std::cout << "file: " << fname << " could not be mmapped\n";
            return -1;
        }

        RdBuf_t zipBuf = zipFile.Buffer();

        RdBuf_t eocdScanBuf = zipBuf.last(
                                std::min(zip::EOCDRec::MaxBytes(), zipBuf.size())
                            );

        int e = 0;
        ForEachFindEnd(
                eocdScanBuf,
                zip::EOCDRec::SIG,
                [&e, zipBuf](RdBuf_t match)
                {
                    auto eocd = zip::EOCDRec::read(match);
                    if (!eocd || !Validate(*eocd, zipBuf.size()))
                    {
                        std::cout << "eocd is bogus (1)" << "\n";

                        return true;
                    }

                    // std::cout << "eocd: " << *eocd << "\n";

                    RdBuf_t cdBuf = zipBuf.subspan(
                                        eocd->offsetOfCentralDir,
                                        std::min<size_t>(
                                            zip::CDFHeader::MaxBytes() * eocd->totalEntries,
                                            eocd->sizeOfCentralDir
                                        )
                                    );

                    for (size_t fn = 0; fn < eocd->totalEntries; ++fn)
                    {
                        auto [cdfh, remCdBuf] = zip::CDFHeader::read(cdBuf);
                        if (!cdfh || !Validate(*cdfh, *eocd, zipBuf.size()))
                        {
                            std::cout << "cdfh[" << fn << "] is bogus (1)" << "\n";

                            return true;
                        }

                        // std::cout << "cdfh[" << fn << "]: " << *cdfh << "\n";

                        RdBuf_t lfBuf = zipBuf.subspan(
                                            cdfh->offsetOfLFHeader,
                                            std::min<size_t>(
                                                zip::LFHeader::MaxBytes(),
                                                eocd->offsetOfCentralDir
                                            )
                                        );

                        auto [lfh, dataBuf] = zip::LFHeader::read(lfBuf);
                        if (!lfh || !Validate(*lfh, *cdfh, *eocd, zipBuf.size()))
                        {
                            std::cout << "lfh[" << fn << "] is bogus (1)" << "\n";

                            return true;
                        }

                        // std::cout << "lfh[" << fn << "]: " << *lfh << "\n";

                        RdBuf_t fileBuf = dataBuf.first(lfh->compressedSz);

                        Inflate(fileBuf);

                        cdBuf = remCdBuf;
                    }

                    return false;
                }
            );

        if (e == -1)
        {
            return -1;
        }
    }

    std::cout << "DONE\n";
    return 0;
}
