#pragma once
/**
 * @file CborRead.h
 * @brief Low-level CBOR argument reader shared by Cbor.cpp.
 */

#include "Cbor.h"

#include <cstdint>

namespace services::auth::passkeys::cbor
{

/** @brief Read a CBOR argument (per RFC 8949 s.3). */
std::uint64_t readArg(
    const Bytes& b, std::size_t& off, std::uint8_t info);

} // namespace services::auth::passkeys::cbor
