#pragma once

#include "utils/Span.hpp"

#include <string_view>
#include <type_traits>

namespace utils
{

template<class T>
const
auto
AsPlainStringView (
    Span<T> Buf
    )
{
    static_assert( std::is_fundamental_v<T>, "only fundamental type can be chars" );

    using PlainT = std::conditional_t<
            (
                std::is_same_v<std::remove_cv_t<T>, char>
             || std::is_same_v<std::remove_cv_t<T>, unsigned char>
             || std::is_same_v<std::remove_cv_t<T>, signed char>
            ),
            char,
            std::make_signed_t<std::remove_cv_t<T>>
        >;

    return std::basic_string_view<PlainT>
    {
        reinterpret_cast<PlainT*>(const_cast<std::remove_cv_t<T>*>(Buf.data())),
        Buf.size()
    };
}

}
