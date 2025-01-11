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

    template<class ClassT>
    static constexpr std::pair<const char*, bool> read(const char* Beg, const char* End, ClassT& Cls)
    {
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
    static constexpr std::pair<char*, bool> write(char* Beg, const char* End, const ClassT& Cls)
    {
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
