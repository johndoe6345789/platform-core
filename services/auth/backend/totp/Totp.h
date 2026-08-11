#pragma once
/**
 * @file Totp.h
 * @brief RFC 6238 time-based OTP wrapper around HOTP.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::totp
{

/** @brief Compute TOTP for current time with 30s step. */
std::string totpNow(
    const std::vector<std::uint8_t>& key,
    int digits = 6,
    int stepSec = 30);

/**
 * @brief Verify a user-submitted code with a ± window.
 * @param window  Number of steps of slop on each side.
 * @return true if any step in the window matches.
 */
bool totpVerify(
    const std::vector<std::uint8_t>& key,
    const std::string& code,
    int digits = 6,
    int stepSec = 30,
    int window = 1);

} // namespace services::auth::totp
