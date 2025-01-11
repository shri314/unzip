#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"
#include "zipEOCDRec.hpp"
#include "zipCDFHeader.hpp"
#include "ForEachFindEnd.hpp"
#include "MemoryMappedFile.hpp"

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

                    std::cout << "eocd: " << *eocd << "\n";

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

                        std::cout << "cdfh[" << fn << "]: " << *cdfh << "\n";

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

                        std::cout << "lfh[" << fn << "]: " << *lfh << "\n";

                        RdBuf_t fileBuf = dataBuf.first(lfh->compressedSz);

                        std::cout << "X = " << fileBuf.size() << "\n";

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
