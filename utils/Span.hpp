#pragma once

#include "utils/ToAddress.hpp"

#include <array>
#include <cassert>
#include <algorithm>

namespace utils
{

template<class T>
struct Span
{
    constexpr Span(
        void
    ) noexcept
        = default;

    constexpr Span(
        const Span&
    ) noexcept
        = default;

    constexpr Span(
        Span&&
    ) noexcept
        = default;

    ~Span(
        void
    ) noexcept
        = default;

    Span&
    operator=(
        const Span&
    ) noexcept
        = default;

    Span&
    operator=(
        Span&&
    ) noexcept
        = default;

    template<class U>
    constexpr Span(
        U* Beg,
        size_t Sz
    ) noexcept
      : m_Data(Beg)
      , m_Sz(Sz)
    {
    }

    template<class IteratorT>
    constexpr Span(
        IteratorT BegIt,
        IteratorT EndIt
    ) noexcept
      : m_Data(ToAddress(BegIt))
      , m_Sz(EndIt - BegIt)
    {
    }

    template<class U, size_t N>
    constexpr Span(
        U (&arr)[N]
    ) noexcept
      : m_Data(&arr[0])
      , m_Sz(N)
    {
    }

    template<class U, size_t N>
    constexpr Span(
        std::array<U, N>& arr
    ) noexcept
      : m_Data(arr.data())
      , m_Sz(N)
    {
    }

    template<class U, size_t N>
    constexpr Span(
        const std::array<U, N>& arr
    ) noexcept
      : m_Data(arr.data())
      , m_Sz(N)
    {
    }

    template<class U>
    constexpr Span(
        const Span<U>& arr
    ) noexcept
      : m_Data(arr.data())
      , m_Sz(arr.size())
    {
    }

    template<
        class ContigContainerT,
        class = std::enable_if_t<!std::is_same_v<std::decay_t<ContigContainerT>, Span>>>
    constexpr Span(
        ContigContainerT&& Cont
    ) noexcept
      : m_Data(&Cont[0])
      , m_Sz(Cont.size())
    {
    }

    constexpr T*
    data(
        void
    ) const noexcept
    {
        return m_Data;
    }

    T&
    operator[](
        size_t Index
    )
    {
        assert(m_Data != nullptr && m_Sz > 0 && Index < m_Sz);

        return m_Data[std::min(Index, m_Sz - 1u)];
    }

    T&
    front(
        void
    ) const
    {
        assert(m_Sz > 0u);

        return (*this)[0];
    }

    T&
    back(
        void
    ) const
    {
        assert(m_Sz > 0u);

        return (*this)[m_Sz - 1u];
    }

    const T*
    cbegin() const
    {
        return m_Data;
    }

    const T*
    cend() const
    {
        return m_Data + m_Sz;
    }

    T*
    begin() const
    {
        return m_Data;
    }

    T*
    end() const
    {
        return m_Data + m_Sz;
    }

    constexpr size_t
    size(
        void
    ) const noexcept
    {
        return m_Sz;
    }

    constexpr bool
    empty(
        void
    ) const noexcept
    {
        return m_Sz == 0u;
    }

    constexpr Span
    first(
        size_t Count
    ) const
    {
        assert(Count <= m_Sz);

        return Span{ m_Data, std::min(m_Sz, Count) };
    }

    constexpr Span
    last(
        size_t Count
    ) const
    {
        assert(Count <= m_Sz);

        return Span{ m_Data + m_Sz - std::min(m_Sz, Count), std::min(m_Sz, Count) };
    }

    constexpr Span
    subspan(
        size_t Offset
    ) const
    {
        assert(Offset <= m_Sz);

        return Span{ m_Data + std::min(m_Sz, Offset), m_Sz - std::min(m_Sz, Offset) };
    }

    constexpr Span
    subspan(
        size_t Offset,
        size_t Count
    ) const
    {
        assert(Offset <= m_Sz && Count <= m_Sz - Offset);

        return Span{ m_Data + std::min(m_Sz, Offset), std::min(m_Sz - std::min(m_Sz, Offset), Count) };
    }

    constexpr friend bool
    operator==(
        const Span& Lhs,
        const Span& Rhs
    )
    {
        return Lhs.data() == Rhs.data()
            && Lhs.size() == Rhs.size();
    }

private:
    T* m_Data   = nullptr;
    size_t m_Sz = 0u;
};

}
