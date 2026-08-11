/**
 * @file AttestationAuthData.cpp
 * @brief Parse authenticator data for registration.
 */

#include "AttestationAuthData.h"
#include "Cbor.h"
#include "CoseKey.h"

#include <stdexcept>

namespace services::auth::passkeys
{

AttestationResult parseAttestedCredentialData(
    const std::vector<std::uint8_t>& authData)
{
    if (authData.size() < 55)
        throw std::runtime_error("authData too short");
    std::uint8_t flags = authData[32];
    if ((flags & 0x40) == 0)
        throw std::runtime_error("AT flag not set");

    AttestationResult out;
    out.aaguid.assign(
        authData.begin() + 37, authData.begin() + 53);
    std::size_t pos = 53;
    std::uint16_t idLen =
        (std::uint16_t(authData[pos]) << 8) | authData[pos + 1];
    pos += 2;
    if (pos + idLen > authData.size())
        throw std::runtime_error("credId len");
    out.credentialId.assign(
        authData.begin() + pos, authData.begin() + pos + idLen);
    pos += idLen;

    std::size_t keyOff = pos;
    auto keyVal = cbor::decode(authData, keyOff);
    auto keyMap = std::get_if<cbor::Map>(&keyVal->data);
    if (!keyMap)
        throw std::runtime_error("cose not map");
    out.parsedKey = parseCoseKey(*keyMap);
    out.publicKeyCose.assign(
        authData.begin() + pos, authData.begin() + keyOff);
    return out;
}

} // namespace services::auth::passkeys
