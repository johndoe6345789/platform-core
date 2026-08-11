#pragma once
/**
 * @file AttestationStatement.h
 * @brief Verify WebAuthn attestation statements.
 */

#include "Cbor.h"
#include "PasskeyTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::passkeys
{

/**
 * @brief Verify an attestation statement for a registration.
 * @param fmt Attestation format string.
 * @param stmt Decoded attStmt CBOR map.
 * @param key Parsed credential public key.
 * @param authData Raw authenticator data.
 * @param clientDataHash SHA-256 of clientDataJSON.
 * @throws std::runtime_error on verification failure.
 */
void verifyAttestationStatement(
    const std::string& fmt,
    const cbor::Map& stmt,
    const CoseKeyParsed& key,
    const std::vector<std::uint8_t>& authData,
    const std::vector<std::uint8_t>& clientDataHash);

} // namespace services::auth::passkeys
