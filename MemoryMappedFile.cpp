#include "MemoryMappedFile.hpp"
#include "ScopeExit.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

MemoryMappedFile::MemoryMappedFile (
    const char* Fname
    )
{
    errno = 0;
    int fd = open(Fname, O_RDONLY|O_CLOEXEC);
    if (fd <= -1)
    {
        m_Err = strerror(errno);
        m_ErrCtx = "open failed";
        return;
    }
    ScopeExit cleanup{[fd]() { close(fd); }};

    struct stat sbuf{};
    errno = 0;
    int r = fstat(fd, &sbuf);
    if (r == -1)
    {
        m_Err = strerror(errno);
        m_ErrCtx = "stat failed";
        return;
    }

    errno = 0;
    const void* ptr = mmap(nullptr, sbuf.st_size, PROT_READ, MAP_FILE | MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED)
    {
        m_Err = strerror(errno);
        m_ErrCtx = "mmap failed";
        return;
    }

    m_Buf = { static_cast<const unsigned char*>(ptr), size_t(sbuf.st_size) };
}


MemoryMappedFile::~MemoryMappedFile (
    void
    )
{
    if (IsValid())
    {
        munmap( const_cast<unsigned char*>(m_Buf.Data()), m_Buf.Size() );
        m_Buf = {};
    }
}


std::pair<const char*, const char*>
MemoryMappedFile::Error (
    void
    ) const
{
    return {m_Err, m_ErrCtx};
}
