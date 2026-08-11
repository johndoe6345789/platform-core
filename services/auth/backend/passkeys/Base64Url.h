#pragma once
/**
 * @file Base64Url.h
 * @brief Tiny base64url codec (no padding) shared by passkey
 *        controllers.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::passkeys
{

/** @brief Encode raw bytes to base64url without padding. */
std::string b64urlEncode(const std::vector<std::uint8_t>& b);

/** @brief Decode a base64url string; ignores non-alphabet. */
std::vector<std::uint8_t> b64urlDecode(const std::string& s);

} // namespace services::auth::passkeys
