#include "DnsHeader.hpp"
#include "zipLFHeader.hpp"
#include "zipEOCDRec.hpp"
#include "zipCDFHeader.hpp"
#include "ScopeExit.hpp"
#include "ForEachFindEnd.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


bool Validate(const zip::EOCDRec& r, size_t zipFileSize)
{
    return
        AsBytes<uint32_t, unsigned char>(r.sig) == zip::EOCDRec::SIG
        && r.thisDiskNum == 0u
        && r.startDiskNum == 0u
        && r.totalEntries == r.totalEntriesThisDisk
        && r.commentLen == r.comment.size()
        && r.offsetOfCentralDir < zipFileSize
        && r.sizeOfCentralDir <= zipFileSize - r.offsetOfCentralDir
        && r.sizeOfCentralDir <= r.totalEntries * zip::CDFHeader::MaxBytes();
}


int main(int argc, const char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <zip-file>\n";
        return -1;
    }

    const char* fname = argv[1];

    {
        int fd = open(fname, O_RDONLY|O_CLOEXEC);
        if (fd <= -1)
        {
            std::cout << "file: " << fname << " could not be opened\n";
            return -1;
        }

        ScopeExit cleanup{[&fd]() { close(fd); }};

        size_t zipFileSize = 0u;
        {
            struct stat sbuf{};
            int r = fstat(fd, &sbuf);
            if (r <= -1)
            {
                return -1;
            }

            zipFileSize = sbuf.st_size;
        }

        {
            std::vector<unsigned char> eocdBuf;
            {
                size_t sz = std::min<size_t>(zip::EOCDRec::MaxBytes(), zipFileSize);
                if (sz < zip::EOCDRec::MinBytes())
                    return -1;

                int r = lseek(fd, zipFileSize - sz, SEEK_SET);
                if (r <= -1)
                    return -1;

                eocdBuf.resize(sz);
                r = read(fd, &eocdBuf[0], sz);
                if (r <= -1)
                    return -1;

                eocdBuf.resize(r);
            }

            int e = 0;
            ForEachFindEnd(
                    eocdBuf,
                    zip::EOCDRec::SIG,
                    [&e, fd, zipFileSize](RdBuf_t match)
                    {
                        auto eocd = zip::EOCDRec::read(match);
                        if (!eocd || !Validate(*eocd, zipFileSize))
                        {
                            std::cout << "eocd is bogus (1)" << "\n";

                            return true;
                        }

                        std::cout << "eocd: " << *eocd << "\n";

                        int r = lseek(fd, eocd->offsetOfCentralDir, SEEK_SET);
                        if (r <= -1)
                        {
                            e = -1;
                            return false;
                        }

                        std::vector<unsigned char> v1;
                        size_t sz = std::min<size_t>(
                                            zip::CDFHeader::MaxBytes() * eocd->totalEntries,
                                            eocd->sizeOfCentralDir
                                        );
                        v1.resize(sz);
                        r = read(fd, &v1[0], sz);
                        if (r <= -1)
                        {
                            e = -1;
                            return false;
                        }

                        auto cdfh = zip::CDFHeader::read(RdBuf_t(v1));
                        if (!cdfh) // || !Validate(*cdfh, zipFileSize))
                        {
                            std::cout << "cdfh is bogus (1)" << "\n";

                            return true;
                        }

                        std::cout << "cdfh: " << *cdfh << "\n";

                        return false;
                    }
                );

            if (e == -1)
            {
                return -1;
            }
        }
    }

    std::cout << "DONE\n";
    return 0;
}
