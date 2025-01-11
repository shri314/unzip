#pragma once

#include "bitmask.hpp"
#include "msgPos.hpp"

#include <utility>
#include <cstring>
#include <cstddef>

namespace msg
{

namespace Impl
{

template<auto MemPtrV, class Pos_t, class ClassT>
static constexpr std::pair<const char*, bool> Seg_read(const char* Beg, const char* End, ClassT& Cls)
{
    using IntW_t = typename Pos_t::IntW_t;
    using IntS_t = typename Pos_t::IntS_t;

    IntW_t x;
    if (End - Beg >= sizeof(x))
    {
        if constexpr (MemPtrV != nullptr)
        {
            std::memcpy(&x, Beg, sizeof(x));
            Cls.*MemPtrV = (x >> Pos_t::LSBit_v) & bitmask<IntS_t>(Pos_t::BitWidth_v);
        }

        if constexpr ((Pos_t::MSBit_v + 1) % 8 == 0)
        {
            Beg += sizeof(x);
        }

        return {Beg, true};
    }

    return {Beg, false};
}

template<auto MemPtrV, class Pos_t, class ClassT>
static constexpr std::pair<const char*, bool> Seg_write(char* Beg, const char* End, const ClassT& Cls)
{
    using IntW_t = typename Pos_t::IntW_t;
    using IntS_t = typename Pos_t::IntS_t;

    IntW_t x;
    if (End - Beg >= sizeof(x))
    {
        if constexpr (MemPtrV != nullptr)
        {
            std::memcpy(&x, Beg, sizeof(x));
            x |= (Cls.*MemPtrV & bitmask<IntS_t>(Pos_t::BitWidth_v)) << Pos_t::LSBit_v;
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

}

template<auto MemPtrV, class PosT>
struct Seg;

template<
    class MemClassT, class MemT, MemT MemClassT::*MemPtrV,
    size_t L, size_t M
>
struct Seg<MemPtrV, Pos<L, M>>
{
    using Pos_t = Pos<L, M>;

    template<class ClassT>
    static constexpr std::pair<const char*, bool> read(const char* Beg, const char* End, ClassT& Cls)
    {
        return Impl::Seg_read<MemPtrV, Pos_t>(Beg, End, Cls);
    }

    template<class ClassT>
    static constexpr std::pair<char*, bool> write(char* Beg, const char* End, const ClassT& Cls)
    {
        return Impl::Seg_write<MemPtrV, Pos_t>(Beg, End, Cls);
    }
};

template<size_t L, size_t M>
struct Seg<nullptr, Pos<L, M>>
{
    using Pos_t = Pos<L, M>;

    template<class ClassT>
    static constexpr std::pair<const char*, bool> read(const char* Beg, const char* End, ClassT& Cls)
    {
        return Impl::Seg_read<nullptr, Pos_t>(Beg, End, Cls);
    }

    template<class ClassT>
    static constexpr std::pair<char*, bool> write(char* Beg, const char* End, const ClassT& Cls)
    {
        return Impl::Seg_write<nullptr, Pos_t>(Beg, End, Cls);
    }
};

}
