#include "zip/LFHeader.hpp"
#include "zip/EOCDRec.hpp"
#include "zip/CDFHeader.hpp"
#include "zip/Inflate.hpp"
#include "utils/ForEachFindEnd.hpp"
#include "utils/MemoryMappedFile.hpp"

#include <cassert>
#include <iostream>

bool
Validate(const zip::EOCDRec& r, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == zip::EOCDRec::SIG
        && r.thisDiskNum == 0u
        && r.startDiskNum == 0u
        && r.totalEntries == r.totalEntriesThisDisk
        && r.commentLen == r.comment.size()
        && r.offsetOfCentralDir < zipFileSize
        && r.sizeOfCentralDir <= zipFileSize - r.offsetOfCentralDir
        && r.sizeOfCentralDir <= r.totalEntries * zip::CDFHeader::MaxBytes()
        && r.sizeOfCentralDir >= r.totalEntries * zip::CDFHeader::MinBytes();
}

bool
Validate(const zip::CDFHeader& r, const zip::EOCDRec& eocd, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == zip::CDFHeader::SIG
        && r.diskNum == 0u
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.commentLen == r.comment.size()
        && r.offsetOfLFHeader < zipFileSize
        && r.offsetOfLFHeader < eocd.offsetOfCentralDir
        && eocd.offsetOfCentralDir - r.offsetOfLFHeader >= zip::LFHeader::MinBytes();
}

bool
Validate(const zip::LFHeader& r, const zip::CDFHeader&, const zip::EOCDRec& eocd, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == zip::LFHeader::SIG
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.compressedSz < zipFileSize
        && r.compressedSz < eocd.offsetOfCentralDir;
}

int
main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <zip-file>\n";
        return -1;
    }

    const char* fname = argv[1];

    {
        utils::MemoryMappedFile zipFile{ fname };
        if (!zipFile.IsValid())
        {
            std::cerr << "file: " << fname << " could not be mmapped\n";
            return -1;
        }

        utils::RdBuf_t zipBuf = zipFile.Buffer();

        utils::RdBuf_t eocdScanBuf = zipBuf.last(
            std::min(zip::EOCDRec::MaxBytes(), zipBuf.size())
        );

        int e = 0;
        ForEachFindEnd(
            eocdScanBuf,
            zip::EOCDRec::SIG,
            [&e, zipBuf](utils::RdBuf_t match)
            {
                auto eocd = zip::EOCDRec::read(match);
                if (!eocd || !Validate(*eocd, zipBuf.size()))
                {
                    if (eocd)
                        std::cerr << "eocd is bogus (1):" << *eocd << "\n";
                    else
                        std::cerr << "eocd is bogus (1)" << "\n";

                    return true;
                }

                //
                // std::cerr << "eocd: " << *eocd << "\n";
                //

                utils::RdBuf_t cdBuf = zipBuf.subspan(
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
                        if (cdfh)
                            std::cerr << "cdfh[" << fn << "] is bogus (1):" << *cdfh << "\n";
                        else
                            std::cerr << "cdfh[" << fn << "] is bogus (1)" << "\n";

                        return true;
                    }

                    //
                    // std::cerr << "cdfh[" << fn << "]: " << *cdfh << "\n";
                    //

                    utils::RdBuf_t lfBuf = zipBuf.subspan(
                        cdfh->offsetOfLFHeader,
                        std::min<size_t>(
                            zip::LFHeader::MaxBytes(),
                            eocd->offsetOfCentralDir
                        )
                    );

                    auto [lfh, dataBuf] = zip::LFHeader::read(lfBuf);
                    if (!lfh || !Validate(*lfh, *cdfh, *eocd, zipBuf.size()))
                    {
                        if (lfh)
                            std::cerr << "lfh[" << fn << "] is bogus (1):" << *lfh << "\n";
                        else
                            std::cerr << "lfh[" << fn << "] is bogus (1)" << "\n";

                        return true;
                    }

                    //
                    // std::cerr << "lfh[" << fn << "]: " << *lfh << "\n";
                    //

                    utils::RdBuf_t fileBuf = dataBuf.first(lfh->compressedSz);

                    //
                    // Get to the contents of the file:
                    //
                    for (auto c : lfh->name)
                        std::cout << c;
                    std::cout << ":\n";
                    std::cout << "-------------------------------------\n";
                    if (lfh->compression == 8u)
                    {
                        zip::Inflate(
                            fileBuf,
                            [](char x)
                            {
                                std::cout << x << std::flush;
                            }
                        );
                    }
                    else if (lfh->compression == 0u)
                    {
                        for (char x : fileBuf)
                            std::cout << x << std::flush;
                    }
                    else
                    {
                        std::cerr << "compression:" << lfh->compression << " is unimplemented" << std::endl;
                    }
                    std::cout << "-------------------------------------\n";

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

    return 0;
}
