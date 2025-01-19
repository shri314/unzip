#pragma once

#include <algorithm>

namespace utils
{

template<class Range, class F>
constexpr void
ForEach(
    Range&& Rng,
    F&& Func
)
{
    for (auto& e : Rng)
    {
        if (!Func(e))
        {
            break;
        }
    }
}

}
