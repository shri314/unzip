#pragma once

#include <type_traits>

namespace utils
{

//
// https://stackoverflow.com/questions/44395169/why-is-sfinae-on-if-constexpr-not-allowed
//

namespace Impl
{
template<
    template<class...> class Z,
    class,
    class...>
struct CanApply : std::false_type
{
};

template<
    template<class...> class Z,
    class... Ts>
struct CanApply<Z, std::void_t<Z<Ts...>>, Ts...> : std::true_type
{
};
}

template<template<class...> class Z, class... Ts>
using CanApply = Impl::CanApply<Z, void, Ts...>;

}
