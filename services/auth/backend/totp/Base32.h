#pragma once
/**
 * @file Base32.h
 * @brief RFC 4648 Base32 codec (no padding output).
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::totp
{

/** @brief Encode raw bytes to uppercase Base32. */
std::string base32Encode(
    const std::vector<std::uint8_t>& data);

/** @brief Decode Base32 (case-insensitive); skips padding. */
std::vector<std::uint8_t> base32Decode(
    const std::string& text);

} // namespace services::auth::totp
