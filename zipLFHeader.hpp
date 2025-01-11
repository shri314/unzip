#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"
#include "msgDyn.hpp"
#include "Hexed.hpp"
#include "AsBytes.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <iomanip>

using RdBuf_t = Span<const unsigned char>;
using WrBuf_t = Span<unsigned char>;

namespace zip
{

struct LFHeader
{
    static constexpr auto SIG = AsBytes<uint32_t, unsigned char>(0x04034b50);

    uint32_t sig;
    uint16_t ver;
    uint16_t flags;
    uint16_t compression;
    uint16_t lastModTime;
    uint16_t lastModDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t originalSize;
    uint16_t fileNameLen;
    uint16_t exFieldLen;
    std::string fileName;
    std::vector<unsigned char> exField;

    using Format = msg::Fmt<
        msg::Seg<&LFHeader::sig>,
        msg::Seg<&LFHeader::ver>,
        msg::Seg<&LFHeader::flags>,
        msg::Seg<&LFHeader::compression>,
        msg::Seg<&LFHeader::lastModTime>,
        msg::Seg<&LFHeader::lastModDate>,
        msg::Seg<&LFHeader::crc32>,
        msg::Seg<&LFHeader::compressedSize>,
        msg::Seg<&LFHeader::originalSize>,
        msg::Seg<&LFHeader::fileNameLen>,
        msg::Seg<&LFHeader::exFieldLen>,
        msg::Seg<&LFHeader::fileName, msg::Dyn<&LFHeader::fileNameLen>>,
        msg::Seg<&LFHeader::exField, msg::Dyn<&LFHeader::exFieldLen>>
    >;

    friend bool operator==(const LFHeader& l, const LFHeader& r)
    {
        return l.sig == r.sig
            && l.ver == r.ver
            && l.flags == r.flags
            && l.compression == r.compression
            && l.lastModTime == r.lastModTime
            && l.lastModDate == r.lastModDate
            && l.crc32 == r.crc32
            && l.compressedSize == r.compressedSize
            && l.originalSize == r.originalSize
            && l.fileNameLen == r.fileNameLen
            && l.exFieldLen == r.exFieldLen
            && l.fileName == r.fileName
            && l.exField == r.exField
            ;
    }

    friend std::ostream& operator<<(std::ostream& os, const LFHeader& r)
    {
        return os
            << "{ " << "sig:" << Hexed{r.sig, "0x", 8}
            << ", " << "ver:" << r.ver
            << ", " << "flags:" << r.flags
            << ", " << "compression:" << r.compression
            << ", " << "lastModTime:" << r.lastModTime
            << ", " << "lastModDate:" << r.lastModDate
            << ", " << "crc32:" << Hexed{r.crc32, "0x"}
            << ", " << "compressedSize:" << r.compressedSize
            << ", " << "originalSize:" << r.originalSize
            << ", " << "fileNameLen:" << r.fileNameLen
            << ", " << "exFieldLen:" << r.exFieldLen
            << ", " << "fileName:" << std::quoted(r.fileName)
            << ", " << "exField:" << "[" << r.exField.size() << "...]"
            << " }";
    }

    static std::optional<LFHeader> read(RdBuf_t Buf)
    {
        std::optional<LFHeader> h = LFHeader{};

        auto b = Format::read(Buf, *h);
        if (!b)
        {
            h.reset();
        }

        return h;
    }
};

}
