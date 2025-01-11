#pragma once

#include <utility>
#include <cstddef>

namespace msg
{

template<class SegT, class... SegTs>
struct Fmt
{
    static constexpr size_t FirstLSBit_v = SegT::Pos_t::LSBit_v;
    static constexpr size_t FirstMSBit_v = SegT::Pos_t::MSBit_v;

    static constexpr size_t MinBytes()
    {
        return ( SegT::MinBytes() + ... + SegTs::MinBytes() );
    }

    static constexpr size_t MaxBytes()
    {
        return ( SegT::MaxBytes() + ... + SegTs::MaxBytes() );
    }

    template<class ClassT>
    static constexpr std::pair<const unsigned char*, bool> read(const unsigned char* Beg, const unsigned char* End, ClassT& Cls)
    {
        if (End - Beg < MinBytes())
        {
            return {Beg, false};
        }

        auto ret = SegT::read(Beg, End, Cls);

        if constexpr (sizeof...(SegTs) > 0u)
        {
            static_assert( (SegT::Pos_t::MSBit_v + 1) % 8 == Fmt<SegTs...>::FirstLSBit_v, "there should not be any disjointed segments" );

            if (ret.second)
            {
                return Fmt<SegTs...>::read(ret.first, End, Cls);
            }
        }

        return ret;
    }

    template<class ClassT>
    static constexpr std::pair<unsigned char*, bool> write(unsigned char* Beg, const unsigned char* End, const ClassT& Cls)
    {
        if (End - Beg < MinBytes())
        {
            return {Beg, false};
        }

        auto ret = SegT::write(Beg, End, Cls);

        if constexpr (sizeof...(SegTs) > 0u)
        {
            if (ret.second)
            {
                return Fmt<SegTs...>::read(ret.first, End, Cls);
            }
        }

        return ret;
    }
};

}
