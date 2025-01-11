#pragma once

#include <type_traits>

namespace utils
{

template <typename R>
constexpr R BitMask(size_t BitWidth)
{
    static_assert( std::is_unsigned_v<R> );

    //
    // https://stackoverflow.com/questions/1392059/algorithm-to-generate-bit-mask
    //
    return (BitWidth >= 8u * sizeof(R) ? 0u : (R(1) << BitWidth)) - 1u;
}

/////////////////////////////////////////////////////////////

inline void test_BitMask()
{
    static_assert(BitMask<uint8_t>(0u) == 0x0u);
    static_assert(BitMask<uint8_t>(1u) == 0x1u);
    static_assert(BitMask<uint8_t>(2u) == 0x3u);
    static_assert(BitMask<uint8_t>(3u) == 0x7u);
    static_assert(BitMask<uint8_t>(4u) == 0xFu);

    static_assert(BitMask<uint16_t>(0u) == 0x0u);
    static_assert(BitMask<uint16_t>(1u) == 0x1u);
    static_assert(BitMask<uint16_t>(2u) == 0x3u);
    static_assert(BitMask<uint16_t>(3u) == 0x7u);
    static_assert(BitMask<uint16_t>(4u) == 0xFu);
    static_assert(BitMask<uint16_t>(5u) == 0x1Fu);
    static_assert(BitMask<uint16_t>(6u) == 0x3Fu);
    static_assert(BitMask<uint16_t>(7u) == 0x7Fu);
    static_assert(BitMask<uint16_t>(8u) == 0xFFu);

    static_assert(BitMask<uint32_t>(0u) == 0x0u);
    static_assert(BitMask<uint32_t>(1u) == 0x1u);
    static_assert(BitMask<uint32_t>(2u) == 0x3u);
    static_assert(BitMask<uint32_t>(3u) == 0x7u);
    static_assert(BitMask<uint32_t>(4u) == 0xFu);
    static_assert(BitMask<uint32_t>(5u) == 0x1Fu);
    static_assert(BitMask<uint32_t>(6u) == 0x3Fu);
    static_assert(BitMask<uint32_t>(7u) == 0x7Fu);
    static_assert(BitMask<uint32_t>(8u) == 0xFFu);
    static_assert(BitMask<uint32_t>(9u) == 0x1FFu);
    static_assert(BitMask<uint32_t>(10u) == 0x3FFu);
    static_assert(BitMask<uint32_t>(11u) == 0x7FFu);
    static_assert(BitMask<uint32_t>(12u) == 0xFFFu);
    static_assert(BitMask<uint32_t>(13u) == 0x1FFFu);
    static_assert(BitMask<uint32_t>(14u) == 0x3FFFu);
    static_assert(BitMask<uint32_t>(15u) == 0x7FFFu);
    static_assert(BitMask<uint32_t>(16u) == 0xFFFFu);
    static_assert(BitMask<uint32_t>(17u) == 0x1FFFFu);
    static_assert(BitMask<uint32_t>(18u) == 0x3FFFFu);
    static_assert(BitMask<uint32_t>(19u) == 0x7FFFFu);
    static_assert(BitMask<uint32_t>(20u) == 0xFFFFFu);
    static_assert(BitMask<uint32_t>(21u) == 0x1FFFFFu);
    static_assert(BitMask<uint32_t>(22u) == 0x3FFFFFu);
    static_assert(BitMask<uint32_t>(23u) == 0x7FFFFFu);
    static_assert(BitMask<uint32_t>(24u) == 0xFFFFFFu);
    static_assert(BitMask<uint32_t>(25u) == 0x1FFFFFFu);
    static_assert(BitMask<uint32_t>(26u) == 0x3FFFFFFu);
    static_assert(BitMask<uint32_t>(27u) == 0x7FFFFFFu);
    static_assert(BitMask<uint32_t>(28u) == 0xFFFFFFFu);
    static_assert(BitMask<uint32_t>(29u) == 0x1FFFFFFFu);
    static_assert(BitMask<uint32_t>(30u) == 0x3FFFFFFFu);
    static_assert(BitMask<uint32_t>(31u) == 0x7FFFFFFFu);
    static_assert(BitMask<uint32_t>(32u) == 0xFFFFFFFFu);

    static_assert(BitMask<uint64_t>(0u) == 0x0u);
    static_assert(BitMask<uint64_t>(1u) == 0x1u);
    static_assert(BitMask<uint64_t>(2u) == 0x3u);
    static_assert(BitMask<uint64_t>(3u) == 0x7u);
    static_assert(BitMask<uint64_t>(4u) == 0xFu);
    static_assert(BitMask<uint64_t>(5u) == 0x1Fu);
    static_assert(BitMask<uint64_t>(6u) == 0x3Fu);
    static_assert(BitMask<uint64_t>(7u) == 0x7Fu);
    static_assert(BitMask<uint64_t>(8u) == 0xFFu);
    static_assert(BitMask<uint64_t>(9u) == 0x1FFu);
    static_assert(BitMask<uint64_t>(10u) == 0x3FFu);
    static_assert(BitMask<uint64_t>(11u) == 0x7FFu);
    static_assert(BitMask<uint64_t>(12u) == 0xFFFu);
    static_assert(BitMask<uint64_t>(13u) == 0x1FFFu);
    static_assert(BitMask<uint64_t>(14u) == 0x3FFFu);
    static_assert(BitMask<uint64_t>(15u) == 0x7FFFu);
    static_assert(BitMask<uint64_t>(16u) == 0xFFFFu);
    static_assert(BitMask<uint64_t>(17u) == 0x1FFFFu);
    static_assert(BitMask<uint64_t>(18u) == 0x3FFFFu);
    static_assert(BitMask<uint64_t>(19u) == 0x7FFFFu);
    static_assert(BitMask<uint64_t>(20u) == 0xFFFFFu);
    static_assert(BitMask<uint64_t>(21u) == 0x1FFFFFu);
    static_assert(BitMask<uint64_t>(22u) == 0x3FFFFFu);
    static_assert(BitMask<uint64_t>(23u) == 0x7FFFFFu);
    static_assert(BitMask<uint64_t>(24u) == 0xFFFFFFu);
    static_assert(BitMask<uint64_t>(25u) == 0x1FFFFFFu);
    static_assert(BitMask<uint64_t>(26u) == 0x3FFFFFFu);
    static_assert(BitMask<uint64_t>(27u) == 0x7FFFFFFu);
    static_assert(BitMask<uint64_t>(28u) == 0xFFFFFFFu);
    static_assert(BitMask<uint64_t>(29u) == 0x1FFFFFFFu);
    static_assert(BitMask<uint64_t>(30u) == 0x3FFFFFFFu);
    static_assert(BitMask<uint64_t>(31u) == 0x7FFFFFFFu);
    static_assert(BitMask<uint64_t>(32u) == 0xFFFFFFFFu);
    static_assert(BitMask<uint64_t>(33u) == 0x1FFFFFFFFu);
    static_assert(BitMask<uint64_t>(34u) == 0x3FFFFFFFFu);
    static_assert(BitMask<uint64_t>(35u) == 0x7FFFFFFFFu);
    static_assert(BitMask<uint64_t>(36u) == 0xFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(37u) == 0x1FFFFFFFFFu);
    static_assert(BitMask<uint64_t>(38u) == 0x3FFFFFFFFFu);
    static_assert(BitMask<uint64_t>(39u) == 0x7FFFFFFFFFu);
    static_assert(BitMask<uint64_t>(40u) == 0xFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(41u) == 0x1FFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(42u) == 0x3FFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(43u) == 0x7FFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(44u) == 0xFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(45u) == 0x1FFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(46u) == 0x3FFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(47u) == 0x7FFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(48u) == 0xFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(49u) == 0x1FFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(50u) == 0x3FFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(51u) == 0x7FFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(52u) == 0xFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(53u) == 0x1FFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(54u) == 0x3FFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(55u) == 0x7FFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(56u) == 0xFFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(57u) == 0x1FFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(58u) == 0x3FFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(59u) == 0x7FFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(60u) == 0xFFFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(61u) == 0x1FFFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(62u) == 0x3FFFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(63u) == 0x7FFFFFFFFFFFFFFFu);
    static_assert(BitMask<uint64_t>(64u) == 0xFFFFFFFFFFFFFFFFu);
}

}
