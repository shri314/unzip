#pragma once

#include "CanApply.hpp"

#include <type_traits>
#include <memory>

//
// https://en.cppreference.com/w/cpp/memory/to_address
//

namespace Impl
{

template<class U>
using has_pointer_traits_to_address =
    decltype(std::pointer_traits<U>::to_address(std::declval<U>()));

}

template<class T>
constexpr T* ToAddress(T* Ptr) noexcept
{
    static_assert(!std::is_function_v<T>);
    return Ptr;
}

template<class T>
constexpr auto ToAddress(const T& PtrLike) noexcept
{
    if constexpr (CanApply<Impl::has_pointer_traits_to_address, decltype(PtrLike)>())
    {
        return std::pointer_traits<T>::to_address(PtrLike);
    }
    else
    {
        return ToAddress(PtrLike.operator->());
    }
}
