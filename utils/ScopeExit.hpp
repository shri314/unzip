#pragma once

#include <utility>

namespace utils
{

template<class Fn>
struct ScopeExit
{
    ScopeExit(Fn f)
      : m_f(std::move(f))
    {
    }

    ~ScopeExit()
    {
        try
        {
            m_f();
        }
        catch (...)
        {
        }
    }

private:
    Fn m_f;
};

}
