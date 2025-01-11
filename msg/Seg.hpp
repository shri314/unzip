#pragma once

#include "msg/Pos.hpp"
#include "msg/Dyn.hpp"
#include "utils/RdBuf.hpp"
#include "utils/WrBuf.hpp"
#include "utils/BitMask.hpp"

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
    static constexpr std::optional<utils::RdBuf_t> read(utils::RdBuf_t Buf, ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (Buf.size() >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, Buf.data(), sizeof(x));
                Cls.*MemPtrV = (x >> Pos_t::LSBit_v) & utils::BitMask<UIntW_t>(Pos_t::BitWidth_v);
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Buf = Buf.subspan(sizeof(x));
            }

            return Buf;
        }

        return std::nullopt;
    }

    template<class ClassT>
    static constexpr std::optional<utils::WrBuf_t> write(utils::WrBuf_t Buf, const ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (Buf.size() >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, Buf.data(), sizeof(x));
                x |= (Cls.*MemPtrV & utils::BitMask<UIntW_t>(Pos_t::BitWidth_v)) << Pos_t::LSBit_v;
                std::memcpy(Buf.data(), &x, sizeof(x));
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Buf = Buf.subspan(sizeof(x));
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

    static constexpr size_t MinBytes()
    {
        return MinBytesV;
    }

    static constexpr size_t MaxBytes()
    {
        return MaxBytesV;
    }

    template<class ClassT>
    static constexpr std::optional<utils::RdBuf_t> read(utils::RdBuf_t Buf, ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;

        if (BytesRequested >= MinBytesV
            && BytesRequested <= MaxBytesV
            && BytesRequested <= Buf.size())
        {
            Cls.*MemPtr1V = {Buf.data(), Buf.data() + BytesRequested};

            return Buf.subspan(BytesRequested);
        }

        return std::nullopt;
    }

    template<class ClassT>
    static constexpr std::optional<utils::WrBuf_t> write(utils::WrBuf_t Buf, const ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;

        const auto& src = Cls.*MemPtr1V;

        if (MinBytesV <= Buf.size() && Buf.size() <= MaxBytesV &&
            MinBytesV <= BytesRequested && BytesRequested <= MaxBytesV &&
            BytesRequested == std::size(src))
        {
            std::memcpy(Buf.data(), std::data(src), BytesRequested);

            return Buf.subspan(BytesRequested);
        }

        return std::nullopt;
    }
};

}
