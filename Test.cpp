#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdio>

int main()
{
    // https://godbolt.org/z/E5hhvEa5o
    for (size_t i = 0; i < 256; ++i)
    {
        char buf[1024];
        for(char& x : buf)
        {
            x = i;
        }

        auto h1 = DnsHeader::read_new(buf);
        auto h2 = DnsHeader::read_old(buf);

        assert(h1.has_value());
        assert(h2.has_value());
        assert(h1 == h2);
    }

    for (size_t i = 0; i < 256; ++i)
    {
        char buf[1024];
        for(char& x : buf)
        {
            x = i;
        }

        auto h1 = zip::LFHeader::read(buf);
        assert(h1.has_value());
    }
}
