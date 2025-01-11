#pragma once

#include "Span.hpp"

#include <utility>

using RdBuf_t = Span<const unsigned char>;

class MemoryMappedFile
{

public:
    MemoryMappedFile (
        void
        ) = default;

    MemoryMappedFile (
        const char* Fname
        );

    MemoryMappedFile (
        MemoryMappedFile&& Rhs
        )
        : m_Buf(std::exchange(Rhs.m_Buf, {}))
        , m_Err(std::exchange(Rhs.m_Err, ""))
        , m_ErrCtx(std::exchange(Rhs.m_ErrCtx, ""))
    {
    }

    MemoryMappedFile&
    operator= (
        MemoryMappedFile&& Rhs
        )
    {
        MemoryMappedFile tmp = std::move(Rhs);

        using std::swap;
        swap(tmp, *this);

        return *this;
    }

    MemoryMappedFile (
        const MemoryMappedFile& Rhs
        ) = delete;

    MemoryMappedFile&
    operator= (
        const MemoryMappedFile& Rhs
        ) = delete;

    static
    void
    swap (
        MemoryMappedFile& Lhs,
        MemoryMappedFile& Rhs
        )
    {
        using std::swap;
        swap(Lhs.m_Buf, Rhs.m_Buf);
        swap(Lhs.m_Err, Rhs.m_Err);
        swap(Lhs.m_ErrCtx, Rhs.m_ErrCtx);
    }

    ~MemoryMappedFile (
        void
        );

    bool
    IsValid (
        void
        ) const
    {
        return m_Buf.Data() != nullptr;
    }

    RdBuf_t
    Buffer(
        void
        ) const
    {
        return m_Buf;
    }

    std::pair<const char*, const char*>
    Error (
        void
        ) const;

private:
    RdBuf_t m_Buf;
    const char* m_ErrCtx = "";
    const char* m_Err = "";
};
