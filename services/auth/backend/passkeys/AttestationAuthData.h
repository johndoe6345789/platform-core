#pragma once
/**
 * @file AttestationAuthData.h
 * @brief Parse attested credential data from authData.
 */

#include "AttestationVerifier.h"

#include <cstdint>
#include <vector>

namespace services::auth::passkeys
{

/**
 * @brief Parse authenticator data with attested credential data.
 * @param authData Raw WebAuthn authenticator data bytes.
 * @return Parsed credential details.
 * @throws std::runtime_error on malformed authData.
 */
AttestationResult parseAttestedCredentialData(
    const std::vector<std::uint8_t>& authData);

} // namespace services::auth::passkeys
