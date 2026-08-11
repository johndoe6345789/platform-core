#pragma once
/**
 * @file AttestationVerifier.h
 * @brief Verify a WebAuthn attestation and return the parsed
 *        credential public key.
 *
 * Supports "none" and "packed" attestations. Packed statements
 * accept self attestation and x5c leaf-certificate signatures.
 */

#include "PasskeyTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::passkeys
{

/** @brief Result of a successful attestation verification. */
struct AttestationResult
{
    std::vector<std::uint8_t> credentialId;
    std::vector<std::uint8_t> publicKeyCose;
    CoseKeyParsed parsedKey;
    std::vector<std::uint8_t> aaguid;
};

/**
 * @brief Verify an attestationObject blob.
 * @param attObj Raw CBOR attestation object from the client.
 * @param clientDataHash SHA-256 of clientDataJSON.
 * @return Parsed credential info on success.
 * @throws std::runtime_error on any verification failure.
 */
AttestationResult verifyAttestation(
    const std::vector<std::uint8_t>& attObj,
    const std::vector<std::uint8_t>& clientDataHash);

} // namespace services::auth::passkeys
