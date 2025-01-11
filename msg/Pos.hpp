#pragma once

#include "utils/UIntX.hpp"

#include <cstddef>

namespace msg
{

template<size_t LSBitV, size_t BitWidthV>
struct Pos
{
    static_assert(BitWidthV > 0u && BitWidthV <= 64u, "Pos<> is not correctly specified");

    static constexpr size_t LSBit_v = LSBitV;
    static constexpr size_t MSBit_v = LSBitV + BitWidthV - 1u;
    static constexpr size_t BitWidth_v = BitWidthV;

    using UIntW_t = decltype(utils::UIntX<BitWidthV + LSBitV>());
};

}
