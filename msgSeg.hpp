#pragma once

#include "bitmask.hpp"
#include "msgPos.hpp"
#include "msgDyn.hpp"
#include "Span.hpp"

#include <utility>
#include <array> // std::size
#include <optional>
#include <type_traits>
#include <cstring>
#include <cstddef>

namespace msg::Impl
{

template<auto MemPtrV>
struct SizeSpecDef;

template<class MemClassT, class MemT, MemT MemClassT::*MemPtrV>
struct SizeSpecDef<MemPtrV>
{
    static_assert(
            std::is_integral_v<MemT>
            && !std::is_same_v<std::remove_cv_t<MemT>, bool>,
            "Pos<> or Dyn<> is required to be explicitly specified, can't be deduced for non-integral types"
        );

    using Pos_t = Pos<0u, 8u * sizeof(MemT)>;
};

template<>
struct SizeSpecDef<nullptr>
{
};

}

namespace msg
{

template<auto MemPtrV, class PosT = typename Impl::SizeSpecDef<MemPtrV>::Pos_t>
struct Seg;

template<auto MemPtrV, size_t LSBitV, size_t BitWidthV>
struct Seg<MemPtrV, Pos<LSBitV, BitWidthV>>
{
    using Pos_t = Pos<LSBitV, BitWidthV>;
    using RdBuf_t = Span<const unsigned char>;
    using WrBuf_t = Span<unsigned char>;

    static constexpr size_t MinBytes()
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
        {
            return sizeof(UIntW_t);
        }

        return 0u;
    }

    static constexpr size_t MaxBytes()
    {
        return MinBytes();
    }

    template<class ClassT>
    static constexpr std::optional<RdBuf_t> read(RdBuf_t Buf, ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (Buf.Size() >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, Buf.Data(), sizeof(x));
                Cls.*MemPtrV = (x >> Pos_t::LSBit_v) & bitmask<UIntW_t>(Pos_t::BitWidth_v);
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Buf = Buf.SubSpan(sizeof(x));
            }

            return Buf;
        }

        return std::nullopt;
    }

    template<class ClassT>
    static constexpr std::optional<WrBuf_t> write(WrBuf_t Buf, const ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (Buf.Size() >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, &Buf[0], sizeof(x));
                x |= (Cls.*MemPtrV & bitmask<UIntW_t>(Pos_t::BitWidth_v)) << Pos_t::LSBit_v;
                std::memcpy(&Buf[0], &x, sizeof(x));
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Buf = Buf.SubSpan(sizeof(x));
            }

            return Buf;
        }

        return std::nullopt;
    }
};

template<
    auto MemPtr1V,
    auto MemPtr2V, size_t MinBytesV, size_t MaxBytesV
>
struct Seg<MemPtr1V, Dyn<MemPtr2V, MinBytesV, MaxBytesV>>
{
    using Pos_t = Pos<0u, 8u>;
    using RdBuf_t = Span<const unsigned char>;
    using WrBuf_t = Span<unsigned char>;

    static constexpr size_t MinBytes()
    {
        return MinBytesV;
    }

    static constexpr size_t MaxBytes()
    {
        return MaxBytesV;
    }

    template<class ClassT>
    static constexpr std::optional<RdBuf_t> read(RdBuf_t Buf, ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;

        if (BytesRequested >= MinBytesV
            && BytesRequested <= MaxBytesV
            && BytesRequested <= Buf.Size())
        {
            if (BytesRequested > 0)
            {
                Cls.*MemPtr1V = {&Buf[0], &Buf[BytesRequested]};
            }

            return Buf.SubSpan(BytesRequested);
        }

        return std::nullopt;
    }

    template<class ClassT>
    static constexpr std::optional<WrBuf_t> write(WrBuf_t Buf, const ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;

        const auto& src = Cls.*MemPtr1V;

        if (MinBytesV <= Buf.Size() && Buf.Size() <= MaxBytesV &&
            MinBytesV <= BytesRequested && BytesRequested <= MaxBytesV &&
            BytesRequested == std::size(src))
        {
            if (BytesRequested > 0u)
            {
                std::memcpy(&Buf[0], &src[0], BytesRequested);
            }

            return Buf.SubSpan(BytesRequested);
        }

        return std::nullopt;
    }
};

}
