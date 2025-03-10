#pragma once

#include <algorithm>

namespace utils
{

template<class Range1, class Range2, class F>
constexpr void
ForEachFindEnd(
    Range1&& Rng,
    Range2&& SRng,
    F&& Func
)
{
    auto cend = Rng.cend();

    while (true)
    {
        auto pos = std::find_end(
            Rng.cbegin(), cend, SRng.cbegin(), SRng.cend()
        );

        if (pos == cend)
        {
            break;
        }

        if (!Func({ pos, cend }))
        {
            break;
        }

        cend = pos;
    }
}

}
