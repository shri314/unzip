#pragma once

#include "bitmask.hpp"
#include "msgPos.hpp"
#include "msgDyn.hpp"

#include <utility>
#include <string_view> // std::size
#include <cstring>
#include <cstddef>
#include <type_traits>

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
    static constexpr std::pair<const unsigned char*, bool> read(const unsigned char* Beg, const unsigned char* End, ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (End - Beg >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, Beg, sizeof(x));
                Cls.*MemPtrV = (x >> Pos_t::LSBit_v) & bitmask<UIntW_t>(Pos_t::BitWidth_v);
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Beg += sizeof(x);
            }

            return {Beg, true};
        }

        return {Beg, false};
    }

    template<class ClassT>
    static constexpr std::pair<unsigned char*, bool> write(unsigned char* Beg, const unsigned char* End, const ClassT& Cls)
    {
        using UIntW_t = typename Pos_t::UIntW_t;

        UIntW_t x;
        if (End - Beg >= sizeof(x))
        {
            if constexpr (MemPtrV != nullptr)
            {
                std::memcpy(&x, Beg, sizeof(x));
                x |= (Cls.*MemPtrV & bitmask<UIntW_t>(Pos_t::BitWidth_v)) << Pos_t::LSBit_v;
                std::memcpy(Beg, &x, sizeof(x));
            }

            if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
            {
                Beg += sizeof(x);
            }

            return {Beg, true};
        }

        return {Beg, false};
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
    static constexpr std::pair<const unsigned char*, bool> read(const unsigned char* Beg, const unsigned char* End, ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;
        size_t BytesAvailable = End - Beg;

        if (BytesRequested >= MinBytesV
            && BytesRequested <= MaxBytesV
            && BytesRequested <= BytesAvailable)
        {
            Cls.*MemPtr1V = {Beg, Beg + BytesRequested};

            Beg += BytesRequested;

            return {Beg, true};
        }

        return {Beg, false};
    }

    template<class ClassT>
    static constexpr std::pair<unsigned char*, bool> write(unsigned char* Beg, const unsigned char* End, const ClassT& Cls)
    {
        size_t BytesRequested = Cls.*MemPtr2V;
        size_t BytesAvailable = End - Beg;

        const auto& src = Cls.*MemPtr1V;

        if (MinBytesV <= BytesAvailable && BytesAvailable <= MaxBytesV &&
            MinBytesV <= BytesRequested && BytesRequested <= MaxBytesV &&
            BytesRequested == std::size(src))
        {
            std::memcpy(Beg, &src[0], BytesRequested);

            Beg += BytesRequested;

            return {Beg, true};
        }

        return {Beg, false};
    }
};

}
