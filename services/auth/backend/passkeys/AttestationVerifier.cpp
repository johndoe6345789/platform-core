/**
 * @file AttestationVerifier.cpp
 * @brief Attestation verifier entry point.
 */

#include "AttestationVerifier.h"
#include "AttestationAuthData.h"
#include "AttestationStatement.h"
#include "Cbor.h"

#include <stdexcept>

namespace services::auth::passkeys
{

AttestationResult verifyAttestation(
    const std::vector<std::uint8_t>& attObj,
    const std::vector<std::uint8_t>& clientDataHash)
{
    std::size_t off = 0;
    auto root = cbor::decode(attObj, off);
    auto rootMap = std::get_if<cbor::Map>(&root->data);
    if (!rootMap)
        throw std::runtime_error("att root not map");

    auto fmtV = cbor::mapGetStr(*rootMap, "fmt");
    auto authDataV = cbor::mapGetStr(*rootMap, "authData");
    auto attStmtV = cbor::mapGetStr(*rootMap, "attStmt");
    if (!fmtV || !authDataV || !attStmtV)
        throw std::runtime_error("att missing fields");
    auto fmt = std::get_if<std::string>(&fmtV->data);
    auto ad = std::get_if<cbor::Bytes>(&authDataV->data);
    auto stmt = std::get_if<cbor::Map>(&attStmtV->data);
    if (!fmt || !ad || !stmt)
        throw std::runtime_error("att bad types");

    auto out = parseAttestedCredentialData(*ad);
    verifyAttestationStatement(
        *fmt, *stmt, out.parsedKey, *ad, clientDataHash);
    return out;
}

} // namespace services::auth::passkeys
