#include "zip/LFHeader.hpp"
#include "zip/EOCDRec.hpp"
#include "zip/CDFHeader.hpp"
#include "zip/Inflate.hpp"
#include "utils/ForEach.hpp"
#include "utils/ForEachFindEnd.hpp"
#include "utils/MemoryMappedFile.hpp"

#include <cassert>
#include <iostream>

namespace zip
{

bool
Validate(const EOCDRec& r, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == EOCDRec::SIG
        && r.thisDiskNum == 0u
        && r.startDiskNum == 0u
        && r.totalEntries == r.totalEntriesThisDisk
        && r.commentLen == r.comment.size()
        && r.offsetOfCentralDir < zipFileSize
        && r.sizeOfCentralDir <= zipFileSize - r.offsetOfCentralDir
        && r.sizeOfCentralDir <= r.totalEntries * CDFHeader::MaxBytes()
        && r.sizeOfCentralDir >= r.totalEntries * CDFHeader::MinBytes();
}

bool
Validate(const CDFHeader& r, const EOCDRec& eocd, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == CDFHeader::SIG
        && r.diskNum == 0u
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.commentLen == r.comment.size()
        && r.offsetOfLFHeader < zipFileSize
        && r.offsetOfLFHeader < eocd.offsetOfCentralDir
        && eocd.offsetOfCentralDir - r.offsetOfLFHeader >= LFHeader::MinBytes();
}

bool
Validate(const LFHeader& r, const CDFHeader&, const EOCDRec& eocd, size_t zipFileSize)
{
    return AsBytes<uint32_t, unsigned char>(r.sig) == LFHeader::SIG
        && r.nameLen == r.name.size()
        && r.exFieldLen == r.exField.size()
        && r.compressedSz < zipFileSize
        && r.compressedSz < eocd.offsetOfCentralDir;
}

struct CDir
{
    EOCDRec eocd;
};

}

std::vector<zip::CDir>
GetPotentialCDir(utils::RdBuf_t zipBuf)
{
    std::vector<zip::CDir> results;

    utils::RdBuf_t eocdScanBuf = zipBuf.last(
        std::min(zip::EOCDRec::MaxBytes(), zipBuf.size())
    );

    utils::ForEachFindEnd(
        eocdScanBuf,
        zip::EOCDRec::SIG,
        [&results, zipBuf](utils::RdBuf_t match)
        {
            auto eocd = zip::EOCDRec::read(match);
            if (eocd && Validate(*eocd, zipBuf.size()))
            {
                results.push_back({ *eocd });
            }

            return true;
        }
    );

    return results;
}

template<class FuncT>
bool
ForEachEntry(const zip::CDir& cdir, utils::RdBuf_t zipBuf, FuncT Func)
{
    const zip::EOCDRec& eocd = cdir.eocd;

    //
    // std::cerr << "eocd: " << eocd << "\n";
    //

    utils::RdBuf_t cdBuf = zipBuf.subspan(
        eocd.offsetOfCentralDir,
        std::min<size_t>(
            zip::CDFHeader::MaxBytes() * eocd.totalEntries,
            eocd.sizeOfCentralDir
        )
    );

    for (size_t fn = 0; fn < eocd.totalEntries; ++fn)
    {
        auto [cdfh, remCdBuf] = zip::CDFHeader::read(cdBuf);
        if (!cdfh || !Validate(*cdfh, eocd, zipBuf.size()))
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
                eocd.offsetOfCentralDir
            )
        );

        auto [lfh, dataBuf] = zip::LFHeader::read(lfBuf);
        if (!lfh || !Validate(*lfh, *cdfh, eocd, zipBuf.size()))
        {
            if (lfh)
                std::cerr << "lfh[" << fn << "] is bogus (1):" << *lfh << "\n";
            else
                std::cerr << "lfh[" << fn << "] is bogus (1)" << "\n";

            return true;
        }

        if (!Func(*lfh, dataBuf))
        {
            return false;
        }

        cdBuf = remCdBuf;
    }

    return true;
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

        std::vector<zip::CDir> potentialCDirs = GetPotentialCDir(zipBuf);
        if (potentialCDirs.size() <= 0)
        {
            std::cout << "no valid eocd records found\n";
            return -1;
        }

        bool xxxx = utils::ForEach(
            potentialCDirs,
            [zipBuf](const zip::CDir& cdir)
            {
                return ForEachEntry(
                    cdir,
                    zipBuf,
                    [&](const zip::LFHeader& lfh, utils::RdBuf_t dataBuf)
                    {
                        //
                        // std::cerr << "lfh[" << fn << "]: " << *lfh << "\n";
                        //

                        utils::RdBuf_t fileBuf = dataBuf.first(lfh.compressedSz);

                        //
                        // Get to the contents of the file:
                        //
                        for (auto c : lfh.name)
                            std::cout << c;

                        std::cout << ":\n";
                        std::cout << "-------------------------------------\n";
                        if (lfh.compression == 8u)
                        {
                            zip::Inflate(
                                fileBuf,
                                [](char x)
                                {
                                    std::cout << x << std::flush;
                                }
                            );
                        }
                        else if (lfh.compression == 0u)
                        {
                            for (char x : fileBuf)
                                std::cout << x << std::flush;
                        }
                        else
                        {
                            std::cerr << "compression:" << lfh.compression << " is unimplemented" << std::endl;
                        }
                        std::cout << "-------------------------------------\n";
                        return true;
                    }
                );
            }
        );

        if (!xxxx)
            return -1;
    }

    return 0;
}
