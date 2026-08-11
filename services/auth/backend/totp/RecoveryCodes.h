#pragma once
/**
 * @file RecoveryCodes.h
 * @brief Generate + verify TOTP recovery codes.
 *
 * Plaintext codes are 8 alphanumeric characters (roughly
 * 41 bits of entropy).  Only SHA-256 hex hashes are stored.
 */

#include <string>
#include <vector>

namespace services::auth::totp
{

/** @brief Generate N codes.  Returns the plaintext. */
std::vector<std::string> generateRecoveryCodes(
    int count, int length);

/** @brief SHA-256 hex hash of a recovery code. */
std::string hashRecoveryCode(const std::string& plain);

/** @brief Check if a code matches any hash; returns index. */
int findMatchingRecoveryCode(
    const std::string& plain,
    const std::vector<std::string>& hashes);

} // namespace services::auth::totp
