#pragma once

#include <algorithm>

namespace utils
{

template<class Range, class F>
constexpr bool
ForEach(
    Range&& Rng,
    F&& Func
)
{
    for (auto& e : Rng)
    {
        if (!Func(e))
        {
            return false;
        }
    }

    return true;
}

}
