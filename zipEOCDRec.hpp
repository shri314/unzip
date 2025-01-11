#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"
#include "AsBytes.hpp"

#include <optional>
#include <ostream>
#include <string>

namespace zip
{

struct EOCDRec
{
    static constexpr auto SIG = AsBytes<uint32_t, unsigned char>(0x06054b50);

    uint32_t sig;
    uint16_t thisDiskNum;
    uint16_t startDiskNum;
    uint16_t totalEntriesThisDisk;
    uint16_t totalEntries;
    uint32_t sizeOfCentralDir;
    uint32_t offsetOfCentralDirStartDisk;
    uint16_t zipFileCommentLen;
    std::string zipFileComment;

    using Format = msg::Fmt<
        msg::Seg<&EOCDRec::sig>,
        msg::Seg<&EOCDRec::thisDiskNum>,
        msg::Seg<&EOCDRec::startDiskNum>,
        msg::Seg<&EOCDRec::totalEntriesThisDisk>,
        msg::Seg<&EOCDRec::totalEntries>,
        msg::Seg<&EOCDRec::sizeOfCentralDir>,
        msg::Seg<&EOCDRec::offsetOfCentralDirStartDisk>,
        msg::Seg<&EOCDRec::zipFileCommentLen>,
        msg::Seg<&EOCDRec::zipFileComment, msg::Dyn<&EOCDRec::zipFileCommentLen>>
    >;

    friend bool operator==(const EOCDRec& l, const EOCDRec& r)
    {
        return
            l.sig == r.sig &&
            l.thisDiskNum == r.thisDiskNum &&
            l.startDiskNum == r.startDiskNum &&
            l.totalEntriesThisDisk == r.totalEntriesThisDisk &&
            l.totalEntries == r.totalEntries &&
            l.sizeOfCentralDir == r.sizeOfCentralDir &&
            l.offsetOfCentralDirStartDisk == r.offsetOfCentralDirStartDisk &&
            l.zipFileCommentLen == r.zipFileCommentLen &&
            l.zipFileComment == r.zipFileComment;
    }

    friend std::ostream& operator<<(std::ostream& os, const EOCDRec& r)
    {
        return os
            << "{ " << "sig:" << Hexed{r.sig, "0x", 8}
            << ", " << "thisDiskNum:" << r.thisDiskNum
            << ", " << "startDiskNum:" << r.startDiskNum
            << ", " << "totalEntriesThisDisk:" << r.totalEntriesThisDisk
            << ", " << "totalEntries:" << r.totalEntries
            << ", " << "sizeOfCentralDir:" << r.sizeOfCentralDir
            << ", " << "offsetOfCentralDirStartDisk:" << r.offsetOfCentralDirStartDisk
            << ", " << "zipFileCommentLen:" << r.zipFileCommentLen
            << ", " << "zipFileComment:" << std::quoted(r.zipFileComment)
            << " }";
    }

    static size_t MinBytes()
    {
        return Format::MinBytes();
    }

    static size_t MaxBytes()
    {
        return Format::MaxBytes();
    }

    static std::optional<EOCDRec> read(const unsigned char* Beg, const unsigned char* End)
    {
        std::optional<EOCDRec> h = EOCDRec{};

        auto [_, s] = Format::read(Beg, End, *h);
        if (!s)
        {
            h.reset();
        }

        return h;
    }
};

}
