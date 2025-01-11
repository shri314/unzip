#pragma once

#include <ostream>
#include <sstream>
#include <iomanip>
#include <ios>

template<class T, size_t N>
struct Hexed
{
    static_assert(
            std::is_integral_v<T>,
            "Hexed<> requires intergal types only"
        );

    friend
    std::ostream&
    operator<< (
        std::ostream& Os,
        const Hexed& Rhs
        )
    {
        std::ostringstream oss;
        oss << std::hex;
        oss << std::setfill('0');
        oss << std::setw(Rhs.m_Width);
        oss << uint64_t(Rhs.m_Ref);
        return Os << Rhs.m_Prefix << std::move(oss).str();
    }

    Hexed (
        T& Ref,
        const char (&Prefix)[N],
        size_t Width = 0
        ) : m_Ref(Ref)
          , m_Prefix(Prefix)
          , m_Width(Width)
    {
    }

private:
    T& m_Ref;
    const char (&m_Prefix)[N];
    size_t m_Width = 0u;
};
