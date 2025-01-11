#pragma once

#include <cstddef>
#include <cstdint>

template<size_t width_bits>
constexpr auto UIntX()
{
    static_assert( width_bits > 0 && width_bits <= 64 );

    if constexpr (width_bits <= 8)
    {
        return uint8_t{};
    }
    else if constexpr (width_bits <= 16)
    {
        return uint16_t{};
    }
    else if constexpr (width_bits <= 32)
    {
        return uint32_t{};
    }
    else if constexpr (width_bits <= 64)
    {
        return uint64_t{};
    }
}

//////////////////////////////////////////////////////

#include <type_traits>

inline void test_uintx()
{
    static_assert(std::is_same_v<decltype(UIntX<1u>()), uint8_t>);
    static_assert(std::is_same_v<decltype(UIntX<8u>()), uint8_t>);
    static_assert(std::is_same_v<decltype(UIntX<9u>()), uint16_t>);
    static_assert(std::is_same_v<decltype(UIntX<16u>()), uint16_t>);
    static_assert(std::is_same_v<decltype(UIntX<17u>()), uint32_t>);
    static_assert(std::is_same_v<decltype(UIntX<32u>()), uint32_t>);
    static_assert(std::is_same_v<decltype(UIntX<33u>()), uint64_t>);
    static_assert(std::is_same_v<decltype(UIntX<64u>()), uint64_t>);
}
