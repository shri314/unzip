#pragma once

#include "utils/RdBuf.hpp"

#include <optional>
#include <utility>
#include <cstddef>

namespace msg
{

template<class SegT, class... SegTs>
struct Fmt
{
    static constexpr size_t FirstLSBit_v = SegT::Pos_t::LSBit_v;
    static constexpr size_t FirstMSBit_v = SegT::Pos_t::MSBit_v;

    static constexpr size_t
    MinBytes()
    {
        return (SegT::MinBytes() + ... + SegTs::MinBytes());
    }

    static constexpr size_t
    MaxBytes()
    {
        return (SegT::MaxBytes() + ... + SegTs::MaxBytes());
    }

    template<class ClassT>
    static constexpr std::optional<utils::RdBuf_t>
    read(utils::RdBuf_t Buf, ClassT& Cls)
    {
        if (Buf.size() < MinBytes())
        {
            return std::nullopt;
        }

        auto ret = SegT::read(Buf, Cls);

        if constexpr (sizeof...(SegTs) > 0u)
        {
            static_assert((SegT::Pos_t::MSBit_v + 1) % 8 == Fmt<SegTs...>::FirstLSBit_v, "there should not be any disjointed segments");

            if (ret)
            {
                return Fmt<SegTs...>::read(*ret, Cls);
            }
        }

        return ret;
    }

    template<class ClassT>
    static constexpr std::optional<utils::WrBuf_t>
    write(utils::WrBuf_t Buf, const ClassT& Cls)
    {
        if (Buf.size() < MinBytes())
        {
            return std::nullopt;
        }

        auto ret = SegT::write(Buf, Cls);

        if constexpr (sizeof...(SegTs) > 0u)
        {
            if (ret)
            {
                return Fmt<SegTs...>::read(*ret, Cls);
            }
        }

        return ret;
    }
};

}
