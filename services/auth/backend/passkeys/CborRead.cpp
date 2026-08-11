/**
 * @file CborRead.cpp
 * @brief Argument-read helper extracted to stay under 100 LOC.
 */

#include "CborRead.h"

#include <stdexcept>

namespace services::auth::passkeys::cbor
{

std::uint64_t readArg(
    const Bytes& b, std::size_t& off, std::uint8_t info)
{
    if (info < 24) return info;
    auto need = [&](std::size_t n) {
        if (off + n > b.size())
            throw std::runtime_error("cbor eof");
    };
    std::uint64_t v = 0;
    if (info == 24) {
        need(1);
        v = b[off++];
        return v;
    }
    if (info == 25) {
        need(2);
        v = (std::uint64_t(b[off]) << 8) | b[off + 1];
        off += 2;
        return v;
    }
    if (info == 26) {
        need(4);
        for (int i = 0; i < 4; ++i)
            v = (v << 8) | b[off + i];
        off += 4;
        return v;
    }
    if (info == 27) {
        need(8);
        for (int i = 0; i < 8; ++i)
            v = (v << 8) | b[off + i];
        off += 8;
        return v;
    }
    throw std::runtime_error("cbor bad info");
}

} // namespace services::auth::passkeys::cbor
