#pragma once

#include <type_traits>
#include <array>

template<class T, class ByteT>
constexpr auto AsBytes(T x)
{
    static_assert(
            std::is_integral_v<T>,
            "T requires to be an integral"
        );
    static_assert(
            std::is_integral_v<ByteT>
            && !std::is_same_v<std::remove_cv_t<ByteT>, bool>
            && sizeof(ByteT) == 1u,
            "ByteT requires to be suitable to represent a byte"
        );

    using U = std::make_unsigned_t<T>;

    std::array<ByteT, sizeof x> ret{};
    for (size_t i = 0; i < sizeof x; ++i)
    {
        ret[i] = (static_cast<U>(x) >> i * 8u) & 0xFF;
    }

    return ret;
}
