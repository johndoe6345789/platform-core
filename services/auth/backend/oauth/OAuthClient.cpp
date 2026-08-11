/**
 * @file OAuthClient.cpp
 * @brief Generic OAuth 2.0 PKCE helper implementation.
 */

#include "OAuthClient.h"
#include "ProviderConfigLoader.h"
#include "TokenExchange.h"
#include "auth/backend/passkeys/Base64Url.h"

#include <openssl/rand.h>
#include <openssl/sha.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace services::auth::oauth
{

namespace
{

std::string urlEnc(const std::string& s)
{
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    for (unsigned char c : s) {
        if (isalnum(c) || c == '-' || c == '_' ||
            c == '.' || c == '~') {
            out.push_back(c);
        } else {
            out.push_back('%');
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 0x0f]);
        }
    }
    return out;
}

} // namespace

std::string buildAuthorizeUrl(
    const ProviderConfig& cfg,
    const std::string& state,
    const std::string& codeChallenge,
    const std::string& redirectUri)
{
    std::string u = cfg.authorizeUrl;
    u += "?response_type=code";
    u += "&client_id=" + urlEnc(cfg.clientId);
    u += "&redirect_uri=" + urlEnc(redirectUri);
    u += "&scope=" + urlEnc(cfg.scope);
    u += "&state=" + urlEnc(state);
    u += "&code_challenge=" + urlEnc(codeChallenge);
    u += "&code_challenge_method=S256";
    return u;
}

std::pair<std::string, std::string> generatePkcePair()
{
    std::vector<std::uint8_t> v(32);
    RAND_bytes(v.data(), v.size());
    auto verifier =
        services::auth::passkeys::b64urlEncode(v);
    unsigned char d[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char*>(verifier.data()),
        verifier.size(), d);
    std::vector<std::uint8_t> dv(d, d + SHA256_DIGEST_LENGTH);
    auto challenge =
        services::auth::passkeys::b64urlEncode(dv);
    return {verifier, challenge};
}

TokenResponse exchangeCode(
    const ProviderConfig& cfg,
    const std::string& code,
    const std::string& codeVerifier,
    const std::string& redirectUri)
{
    return postTokenRequest(
        cfg, code, codeVerifier, redirectUri,
        providerWantsJsonAccept(cfg.name));
}

} // namespace services::auth::oauth
