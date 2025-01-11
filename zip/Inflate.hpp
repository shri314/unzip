#pragma once

#include "utils/RdBuf.hpp"

#include <functional>

namespace zip
{

int
Inflate(
    utils::RdBuf_t Buf,
    std::function<void(unsigned char)> Cb
);

}
