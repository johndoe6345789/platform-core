#pragma once
/**
 * @file passkey_assert_helpers.h
 * @brief Helpers for PasskeyAssertFinish.cpp.
 */

#include "auth/backend/passkeys/Cbor.h"
#include "auth/backend/passkeys/CoseKey.h"
#include "auth/backend/passkeys/AssertionVerifier.h"
#include "auth/backend/passkeys/PasskeyHex.h"

#include <cstdint>
#include <drogon/HttpResponse.h>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace controllers::passkey_assert
{

/** @brief Parse a CBOR-encoded COSE_Key bytes blob. */
inline services::auth::passkeys::CoseKeyParsed parsePubKey(
    const std::vector<std::uint8_t>& pub)
{
    namespace pk = services::auth::passkeys;
    std::size_t off = 0;
    auto v = pk::cbor::decode(pub, off);
    auto m = std::get_if<pk::cbor::Map>(&v->data);
    if (!m) throw std::runtime_error("invalid COSE key format");
    return pk::parseCoseKey(*m);
}

/** @brief Verify an assertion against a stored COSE public key. */
inline auto verifyStoredCredential(
    const std::string& publicKeyHex,
    const std::vector<std::uint8_t>& authData,
    const std::vector<std::uint8_t>& hash,
    const std::vector<std::uint8_t>& sig) -> bool
{
    namespace pk = services::auth::passkeys;
    auto pub = pk::fromHex(publicKeyHex);
    auto parsed = parsePubKey(pub);
    return pk::verifyAssertion(parsed, authData, hash, sig);
}

/**
 * @brief Build the signed-in JSON response, attach the
 *        nextra_sso cookie, and async-touch last_used_at.
 * @param userId Local user UUID from the credential row.
 * @param credIdHex Hex credential id for the UPDATE.
 * @param cb     Controller response callback to invoke.
 */
void issuePasskeySession(
    const std::string& userId,
    const std::string& credIdHex,
    std::function<void(
        const drogon::HttpResponsePtr&)> cb);

} // namespace controllers::passkey_assert
