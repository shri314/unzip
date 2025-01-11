#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"
#include "zipEOCDRec.hpp"
#include "zipCDFHeader.hpp"
#include "ForEachFindEnd.hpp"
#include "MemoryMappedFile.hpp"

#include <cassert>
#include <iostream>

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
        && r.sizeOfCentralDir <= r.totalEntries * zip::CDFHeader::MaxBytes();
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

        RdBuf_t eocdScanBuf = zipBuf.Last(
                                std::min(zip::EOCDRec::MaxBytes(), zipBuf.Size())
                            );

        int e = 0;
        ForEachFindEnd(
                eocdScanBuf,
                zip::EOCDRec::SIG,
                [&e, zipBuf](RdBuf_t match)
                {
                    auto eocd = zip::EOCDRec::read(match);
                    if (!eocd || !Validate(*eocd, zipBuf.Size()))
                    {
                        std::cout << "eocd is bogus (1)" << "\n";

                        return true;
                    }

                    std::cout << "eocd: " << *eocd << "\n";

                    RdBuf_t cdBuf = zipBuf.SubSpan(
                                        eocd->offsetOfCentralDir,
                                        std::min<size_t>(
                                            zip::CDFHeader::MaxBytes() * eocd->totalEntries,
                                            eocd->sizeOfCentralDir
                                        )
                                    );

                    auto cdfh = zip::CDFHeader::read(cdBuf);
                    if (!cdfh) // || !Validate(*cdfh, zipBuf.Size()))
                    {
                        std::cout << "cdfh is bogus (1)" << "\n";

                        return true;
                    }

                    std::cout << "cdfh: " << *cdfh << "\n";

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
