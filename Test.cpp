#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"
#include "zipEOCDRec.hpp"
#include "ScopeExit.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main()
{
    // https://godbolt.org/z/E5hhvEa5o
    for (size_t i = 0; i < 256; ++i)
    {
        unsigned char buf[1024];
        for(unsigned char& x : buf)
        {
            x = i;
        }

        auto h1 = DnsHeader::read_new(buf);
        auto h2 = DnsHeader::read_old(buf);

        assert(h1.has_value());
        assert(h2.has_value());
        assert(h1 == h2);
    }

    {
        int fd = open("./assets/test1.zip", O_RDONLY|O_CLOEXEC);
        unsigned char buf[1024] = {};
        read(fd, buf, 1024);
        close(fd);

        auto h1 = zip::LFHeader::read(buf);
        std::cout << h1.value() << "\n";
    }

    {
        int fd = open("./assets/test1.zip", O_RDONLY|O_CLOEXEC);
        if (fd <= -1)
        {
            std::cout << "E1\n";
            return -1;
        }

        ScopeExit cleanup{[&fd]() { close(fd); }};

        struct stat sbuf{};
        {
            int r = fstat(fd, &sbuf);
            if (r <= -1)
            {
                return -1;
            }
        }

        size_t sz = std::min<size_t>(zip::EOCDRec::MaxBytes(), sbuf.st_size);
        {
            if (sz < zip::EOCDRec::MinBytes())
                return -1;
        }

        std::vector<unsigned char> buf(sz);
        {
            int r = lseek(fd, sbuf.st_size - sz, SEEK_SET);
            if (r <= -1)
                return -1;

            r = read(fd, &buf[0], sz);
            if (r <= -1)
                return -1;

            buf.resize(r);
        }

        RdBuf_t match{
            std::find_end(buf.begin(), buf.end(), zip::EOCDRec::SIG.begin(), zip::EOCDRec::SIG.end()),
            buf.end()
        };

        if (!match.Empty())
        {
            std::cout << "something is found with sz = " << match.Size() << "\n";
            auto hh = zip::EOCDRec::read(match);
            if (hh)
                std::cout << *hh << "\n";
            else
                std::cout << "bogus\n";
        }

        std::cout << "min = " << zip::EOCDRec::MinBytes() << "\n";
        std::cout << "max = " << zip::EOCDRec::MaxBytes() << "\n";
    }

    return 0;
}
