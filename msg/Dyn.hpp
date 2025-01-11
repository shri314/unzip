#pragma once

#include <type_traits>
#include <cstddef>

namespace msg::Impl
{

template<auto MemPtrV>
struct DynSpecDef;

template<class MemClassT, class MemT, MemT MemClassT::* MemPtrV>
struct DynSpecDef<MemPtrV>
{
    static_assert(
        (std::is_signed_v<MemT> && std::is_integral_v<MemT>) || (std::is_unsigned_v<MemT> && std::is_integral_v<MemT>),
        "Dyn<> can't be specified for non-integral types"
    );

    static constexpr size_t MinBytes_v = 0u;
    static constexpr size_t MaxBytes_v = 1 << (8 * sizeof(MemT));
};

}

namespace msg
{

template<
    auto MemPtrV,
    size_t MinBytesV = Impl::DynSpecDef<MemPtrV>::MinBytes_v,
    size_t MaxBytesV = Impl::DynSpecDef<MemPtrV>::MaxBytes_v>
struct Dyn
{
    static_assert(MaxBytesV >= MinBytesV && MaxBytesV > 0u, "Dyn<> is specified with an invalid range");
};

}
