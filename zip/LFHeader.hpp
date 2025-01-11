#pragma once

#include "msg/Pos.hpp"
#include "msg/Seg.hpp"
#include "msg/Fmt.hpp"
#include "msg/Dyn.hpp"
#include "utils/Span.hpp"
#include "utils/Hexed.hpp"
#include "utils/AsBytes.hpp"
#include "utils/AsPlainStringView.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <iomanip>

namespace zip
{

struct LFHeader
{
    static constexpr auto SIG = AsBytes<uint32_t, unsigned char>(0x04034b50);

    uint32_t sig;
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
    utils::RdBuf_t name;
    utils::RdBuf_t exField;

    using Format = msg::Fmt<
        msg::Seg<&LFHeader::sig>,
        msg::Seg<&LFHeader::verNeeded>,
        msg::Seg<&LFHeader::flags>,
        msg::Seg<&LFHeader::compression>,
        msg::Seg<&LFHeader::lastModTime>,
        msg::Seg<&LFHeader::lastModDate>,
        msg::Seg<&LFHeader::crc32>,
        msg::Seg<&LFHeader::compressedSz>,
        msg::Seg<&LFHeader::originalSz>,
        msg::Seg<&LFHeader::nameLen>,
        msg::Seg<&LFHeader::exFieldLen>,
        msg::Seg<&LFHeader::name, msg::Dyn<&LFHeader::nameLen>>,
        msg::Seg<&LFHeader::exField, msg::Dyn<&LFHeader::exFieldLen>>
    >;

    friend bool operator==(const LFHeader& l, const LFHeader& r)
    {
        return l.sig == r.sig
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
            && l.name == r.name
            && l.exField == r.exField
            ;
    }

    friend std::ostream& operator<<(std::ostream& os, const LFHeader& r)
    {
        return os
            << "{ " << "sig:" << utils::Hexed{r.sig, "0x", 8}
            << ", " << "verNeeded:" << r.verNeeded
            << ", " << "flags:" << r.flags
            << ", " << "compression:" << r.compression
            << ", " << "lastModTime:" << r.lastModTime
            << ", " << "lastModDate:" << r.lastModDate
            << ", " << "crc32:" << utils::Hexed{r.crc32, "0x"}
            << ", " << "compressedSz:" << r.compressedSz
            << ", " << "originalSz:" << r.originalSz
            << ", " << "name:" << std::quoted(utils::AsPlainStringView(r.name))
            << ", " << "exField:" << "[" << r.exField.size() << "...]"
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

    static std::pair<std::optional<LFHeader>,utils::RdBuf_t> read(utils::RdBuf_t Buf)
    {
        std::optional<LFHeader> h = LFHeader{};

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
