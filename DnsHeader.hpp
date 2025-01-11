#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"
#include "Span.hpp"

#include <optional>

using RdBuf_t = Span<const unsigned char>;
using WrBuf_t = Span<unsigned char>;

struct DnsHeader
{
    uint16_t id;
    uint8_t opcode;
    uint8_t rcode;
    // uint8_t z;
    bool qr;
    bool aa;
    bool tc;
    bool rd;
    bool ra;

    using Format = msg::Fmt<
        msg::Seg<&DnsHeader::id>,
        // --
        msg::Seg<&DnsHeader::qr,      msg::Pos<0u, 1u>>,
        msg::Seg<&DnsHeader::opcode,  msg::Pos<1u, 4u>>,
        msg::Seg<&DnsHeader::aa,      msg::Pos<5u, 1u>>,
        msg::Seg<&DnsHeader::tc,      msg::Pos<6u, 1u>>,
        msg::Seg<&DnsHeader::rd,      msg::Pos<7u, 1u>>,
        // --
        msg::Seg<&DnsHeader::ra,      msg::Pos<0u, 1u>>,
        msg::Seg<nullptr,             msg::Pos<1u, 3u>>,
        msg::Seg<&DnsHeader::rcode,   msg::Pos<4u, 4u>>
    >;

    friend bool operator==(const DnsHeader& l, const DnsHeader& r)
    {
        return l.id == r.id &&
            l.opcode == r.opcode &&
            l.rcode == r.rcode &&
            // l.z == r.z &&
            l.qr == r.qr &&
            l.aa == r.aa &&
            l.tc == r.tc &&
            l.rd == r.rd &&
            l.ra == r.ra;
    }

    static constexpr std::optional<DnsHeader> read_new(RdBuf_t Buf)
    {
        std::optional<DnsHeader> h = DnsHeader{};

        auto b = Format::read(Buf, *h);
        if (!b)
        {
            h.reset();
        }

        return h;
    }

    static constexpr std::optional<DnsHeader> read_old(RdBuf_t Buf)
    {
        std::optional<DnsHeader> h = DnsHeader{};

        if (Buf.size() < 4)
        {
            h.reset();
            return h;
        }

        {
            uint16_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->id = (x >> 0) & 0xFFFF;
        }
        Buf = Buf.subspan(2);
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->qr = (x >> 0) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->opcode = ( x >> 1 ) & 0x0F;
        }
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->aa = ( x >> 5 ) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->tc = ( x >> 6 ) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->rd = ( x >> 7 ) & 0x01;
        }
        Buf = Buf.subspan(1);
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->ra = ( x >> 0 ) & 0x01;
        }
        // {
        //     uint8_t x{};
        //     memcpy(&x, &Buf[0], sizeof(x));
        //     h->z = ( x >> 1 ) & 0x07;
        // }
        {
            uint8_t x{};
            memcpy(&x, Buf.data(), sizeof(x));
            h->rcode = ( x >> 4 ) & 0x0F;
        }
        Buf = Buf.subspan(1);

        return h;
    }
};
