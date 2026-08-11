#pragma once
/**
 * @file CoseKey.h
 * @brief Parse a COSE_Key CBOR map to extract pub-key material.
 *
 * Supports ES256 (EC2 P-256) and RS256 (RSA) keys as required
 * by the WebAuthn Level 2 spec section 6.5.6.
 */

#include "Cbor.h"
#include "PasskeyTypes.h"

namespace services::auth::passkeys
{

/** @brief Parse a COSE_Key CBOR map into CoseKeyParsed. */
CoseKeyParsed parseCoseKey(const cbor::Map& map);

} // namespace services::auth::passkeys
