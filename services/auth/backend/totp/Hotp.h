#pragma once
/**
 * @file Hotp.h
 * @brief RFC 4226 HOTP counter-based one-time password.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::totp
{

/**
 * @brief Compute HOTP value for (secret, counter).
 * @param key  Raw shared secret bytes.
 * @param counter 64-bit monotonic counter.
 * @param digits Number of digits to return (usually 6).
 */
std::string hotp(
    const std::vector<std::uint8_t>& key,
    std::uint64_t counter,
    int digits);

} // namespace services::auth::totp
