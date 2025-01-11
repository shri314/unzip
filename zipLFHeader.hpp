#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"

#include <optional>

namespace zip
{

struct LFHeader
{
    uint32_t sig;
    uint16_t ver;
    uint16_t flags;
    uint16_t compression;
    uint16_t lastModTime;
    uint16_t lastModDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t unCompressedSize;
    uint16_t fileNameLen;
    uint16_t extraFieldLen;

    using Format = msg::Fmt<
        msg::Seg<&LFHeader::sig,              msg::Pos<0u, 32u>>,
        msg::Seg<&LFHeader::ver,              msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::flags,            msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::compression,      msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::lastModTime,      msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::lastModDate,      msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::crc32,            msg::Pos<0u, 32u>>,
        msg::Seg<&LFHeader::compressedSize,   msg::Pos<0u, 32u>>,
        msg::Seg<&LFHeader::unCompressedSize, msg::Pos<0u, 32u>>,
        msg::Seg<&LFHeader::fileNameLen,      msg::Pos<0u, 16u>>,
        msg::Seg<&LFHeader::extraFieldLen,    msg::Pos<0u, 16u>>
    >;

    friend bool operator==(const LFHeader& l, const LFHeader& r)
    {
        return
            l.sig == r.sig &&
            l.ver == r.ver &&
            l.flags == r.flags &&
            l.compression == r.compression &&
            l.lastModTime == r.lastModTime &&
            l.lastModDate == r.lastModDate &&
            l.crc32 == r.crc32 &&
            l.compressedSize == r.compressedSize &&
            l.unCompressedSize == r.unCompressedSize &&
            l.fileNameLen == r.fileNameLen &&
            l.extraFieldLen == r.extraFieldLen;
    }

    template<size_t N>
    static constexpr std::optional<LFHeader> read(const char (&Buf)[N])
    {
        std::optional<LFHeader> h = LFHeader{};

        auto [_, s] = Format::read(&Buf[0], &Buf[N], *h);
        if (!s)
        {
            h.reset();
        }

        return h;
    }
};

}
