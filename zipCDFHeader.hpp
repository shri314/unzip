#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"
#include "Span.hpp"
#include "Hexed.hpp"
#include "AsBytes.hpp"
#include "AsPlainStringView.hpp"

#include <optional>
#include <ostream>
#include <string>

using RdBuf_t = Span<const unsigned char>;
using WrBuf_t = Span<unsigned char>;

namespace zip
{

struct CDFHeader
{
    static constexpr auto SIG = AsBytes<uint32_t, unsigned char>(0x02014b50);

    uint32_t sig;
    uint16_t verMadeBy;
    uint16_t verNeeded;
    uint16_t flags;
    uint16_t compression;
    uint16_t lastModTime;
    uint16_t lastModDate;
    uint32_t crc32;
    uint32_t compressedSz;
    uint32_t originalSz;
    uint16_t nameLen;
    uint16_t exFieldLen;
    uint16_t commentLen;
    uint16_t diskNum;
    uint16_t attrInternal;
    uint32_t attrExternal;
    uint32_t offsetOfLFHeader;
    RdBuf_t name;
    RdBuf_t exField;
    RdBuf_t comment;

    using Format = msg::Fmt<
        msg::Seg<&CDFHeader::sig>,
        msg::Seg<&CDFHeader::verMadeBy>,
        msg::Seg<&CDFHeader::verNeeded>,
        msg::Seg<&CDFHeader::flags>,
        msg::Seg<&CDFHeader::compression>,
        msg::Seg<&CDFHeader::lastModTime>,
        msg::Seg<&CDFHeader::lastModDate>,
        msg::Seg<&CDFHeader::crc32>,
        msg::Seg<&CDFHeader::compressedSz>,
        msg::Seg<&CDFHeader::originalSz>,
        msg::Seg<&CDFHeader::nameLen>,
        msg::Seg<&CDFHeader::exFieldLen>,
        msg::Seg<&CDFHeader::commentLen>,
        msg::Seg<&CDFHeader::diskNum>,
        msg::Seg<&CDFHeader::attrInternal>,
        msg::Seg<&CDFHeader::attrExternal>,
        msg::Seg<&CDFHeader::offsetOfLFHeader>,
        msg::Seg<&CDFHeader::name, msg::Dyn<&CDFHeader::nameLen>>,
        msg::Seg<&CDFHeader::exField,  msg::Dyn<&CDFHeader::exFieldLen>>,
        msg::Seg<&CDFHeader::comment,  msg::Dyn<&CDFHeader::commentLen>>
    >;

    friend bool operator==(const CDFHeader& l, const CDFHeader& r)
    {
        return l.sig == r.sig
            && l.verMadeBy == r.verMadeBy
            && l.verNeeded == r.verNeeded
            && l.flags == r.flags
            && l.compression == r.compression
            && l.lastModTime == r.lastModTime
            && l.lastModDate == r.lastModDate
            && l.crc32 == r.crc32
            && l.compressedSz == r.compressedSz
            && l.originalSz == r.originalSz
            && l.nameLen == r.nameLen
            && l.exFieldLen == r.exFieldLen
            && l.commentLen == r.commentLen
            && l.diskNum == r.diskNum
            && l.attrInternal == r.attrInternal
            && l.attrExternal == r.attrExternal
            && l.offsetOfLFHeader == r.offsetOfLFHeader
            && l.name == r.name
            && l.exField == r.exField
            && l.comment == r.comment;
    }

    friend std::ostream& operator<<(std::ostream& os, const CDFHeader& r)
    {
        return os
            << "{ " << "sig:" << Hexed{r.sig, "0x", 8}
            << ", " << "verMadeBy:" << r.verMadeBy
            << ", " << "verNeeded:" << r.verNeeded
            << ", " << "flags:" << r.flags
            << ", " << "compression:" << r.compression
            << ", " << "lastModTime:" << r.lastModTime
            << ", " << "lastModDate:" << r.lastModDate
            << ", " << "crc32:" << Hexed{r.crc32, "0x"}
            << ", " << "compressedSz:" << r.compressedSz
            << ", " << "originalSz:" << r.originalSz
            << ", " << "diskNum:" << r.diskNum
            << ", " << "attrInternal:" << r.attrInternal
            << ", " << "attrExternal:" << r.attrExternal
            << ", " << "offsetOfLFHeader:" << r.offsetOfLFHeader
            << ", " << "name:" << std::quoted(AsPlainStringView(r.name))
            << ", " << "exField:" << "[" << r.exField.size() << "...]"
            << ", " << "comment:" << std::quoted(AsPlainStringView(r.comment))
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

    static std::pair<std::optional<CDFHeader>,RdBuf_t> read(RdBuf_t Buf)
    {
        std::optional<CDFHeader> h = CDFHeader{};

        auto b = Format::read(Buf, *h);
        if (!b)
        {
            b = Buf;
            h.reset();
        }

        return {h, *b};
    }
};

}
