#pragma once
/**
 * @file AssertionVerifier.h
 * @brief Verify a WebAuthn assertion signature (ES256/RS256).
 *
 * Uses OpenSSL EVP to compute signatureBase = authenticatorData
 * || SHA256(clientDataJSON), then verifies against the stored
 * COSE public key.
 */

#include "PasskeyTypes.h"

#include <cstdint>
#include <vector>

namespace services::auth::passkeys
{

/**
 * @brief Verify an assertion signature.
 * @return true on valid signature.
 */
bool verifyAssertion(
    const CoseKeyParsed& key,
    const std::vector<std::uint8_t>& authData,
    const std::vector<std::uint8_t>& clientDataHash,
    const std::vector<std::uint8_t>& signature);

} // namespace services::auth::passkeys
