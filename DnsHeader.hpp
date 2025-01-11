#pragma once

#include "msgPos.hpp"
#include "msgSeg.hpp"
#include "msgFmt.hpp"

#include <optional>

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
        msg::Seg<&DnsHeader::id,      msg::Pos<0u, 16u>>,
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

    template<size_t N>
    static constexpr std::optional<DnsHeader> read_new(const char (&Buf)[N])
    {
        std::optional<DnsHeader> h = DnsHeader{};

        auto [_, s] = Format::read(&Buf[0], &Buf[N], *h);
        if (!s)
        {
            h.reset();
        }

        return h;
    }

    template<size_t N>
    static constexpr std::optional<DnsHeader> read_old(const char (&Buf)[N])
    {
        std::optional<DnsHeader> h = DnsHeader{};

        if (N < 4)
        {
            h.reset();
            return h;
        }

        {
            uint16_t x{};
            memcpy(&x, &Buf[0], sizeof(x));
            h->id = (x >> 0) & 0xFFFF;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[2], sizeof(x));
            h->qr = (x >> 0) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[2], sizeof(x));
            h->opcode = ( x >> 1 ) & 0x0F;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[2], sizeof(x));
            h->aa = ( x >> 5 ) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[2], sizeof(x));
            h->tc = ( x >> 6 ) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[2], sizeof(x));
            h->rd = ( x >> 7 ) & 0x01;
        }
        {
            uint8_t x{};
            memcpy(&x, &Buf[3], sizeof(x));
            h->ra = ( x >> 0 ) & 0x01;
        }
        // {
        //     uint8_t x{};
        //     memcpy(&x, &Buf[3], sizeof(x));
        //     h->z = ( x >> 1 ) & 0x07;
        // }
        {
            uint8_t x{};
            memcpy(&x, &Buf[3], sizeof(x));
            h->rcode = ( x >> 4 ) & 0x0F;
        }

        return h;
    }
};
