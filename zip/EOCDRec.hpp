#pragma once

#include "msg/Pos.hpp"
#include "msg/Seg.hpp"
#include "msg/Fmt.hpp"
#include "utils/RdBuf.hpp"
#include "utils/WrBuf.hpp"
#include "utils/Hexed.hpp"
#include "utils/AsBytes.hpp"
#include "utils/AsPlainStringView.hpp"

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
    uint32_t offsetOfCentralDir;
    uint16_t commentLen;
    utils::RdBuf_t comment;

    using Format = msg::Fmt<
        msg::Seg<&EOCDRec::sig>,
        msg::Seg<&EOCDRec::thisDiskNum>,
        msg::Seg<&EOCDRec::startDiskNum>,
        msg::Seg<&EOCDRec::totalEntriesThisDisk>,
        msg::Seg<&EOCDRec::totalEntries>,
        msg::Seg<&EOCDRec::sizeOfCentralDir>,
        msg::Seg<&EOCDRec::offsetOfCentralDir>,
        msg::Seg<&EOCDRec::commentLen>,
        msg::Seg<&EOCDRec::comment, msg::Dyn<&EOCDRec::commentLen>>>;

    friend bool
    operator==(const EOCDRec& l, const EOCDRec& r)
    {
        return l.sig == r.sig
            && l.thisDiskNum == r.thisDiskNum
            && l.startDiskNum == r.startDiskNum
            && l.totalEntriesThisDisk == r.totalEntriesThisDisk
            && l.totalEntries == r.totalEntries
            && l.sizeOfCentralDir == r.sizeOfCentralDir
            && l.offsetOfCentralDir == r.offsetOfCentralDir
            && l.commentLen == r.commentLen
            && l.comment == r.comment;
    }

    friend std::ostream&
    operator<<(std::ostream& os, const EOCDRec& r)
    {
        return os
            << "{ " << "sig:" << utils::Hexed{ r.sig, "0x", 8 }
            << ", " << "thisDiskNum:" << r.thisDiskNum
            << ", " << "startDiskNum:" << r.startDiskNum
            << ", " << "totalEntriesThisDisk:" << r.totalEntriesThisDisk
            << ", " << "totalEntries:" << r.totalEntries
            << ", " << "sizeOfCentralDir:" << r.sizeOfCentralDir
            << ", " << "offsetOfCentralDir:" << r.offsetOfCentralDir
            << ", " << "comment:" << std::quoted(utils::AsPlainStringView(r.comment))
            << " }";
    }

    static size_t
    MinBytes()
    {
        return Format::MinBytes();
    }

    static size_t
    MaxBytes()
    {
        return Format::MaxBytes();
    }

    static std::optional<EOCDRec>
    read(utils::RdBuf_t Buf)
    {
        std::optional<EOCDRec> h = EOCDRec{};

        auto b = Format::read(Buf, *h);
        if (!b)
        {
            h.reset();
        }

        return h;
    }
};

}
