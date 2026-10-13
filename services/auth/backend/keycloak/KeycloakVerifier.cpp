/**
 * @file KeycloakVerifier.cpp
 * @brief RS256 + claims verification against the OIDC
 *        provider configured by environment.
 */

#include "KeycloakVerifier.h"

#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <openssl/evp.h>
#include <spdlog/spdlog.h>

#include <cctype>
#include <cstdlib>

namespace services::auth::keycloak
{

static std::string env(const char* name)
{
    const char* v = std::getenv(name);
    return v ? v : "";
}

std::string oidcIssuer() { return env("OIDC_ISSUER"); }
std::string oidcClientId() { return env("OIDC_CLIENT_ID"); }
std::string oidcJwksUrl() { return env("OIDC_JWKS_URL"); }

static bool isUuid(const std::string& s)
{
    if (s.size() != 36) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        const bool dash = i == 8 || i == 13 || i == 18
                          || i == 23;
        if (dash ? s[i] != '-'
                 : !std::isxdigit(
                       static_cast<unsigned char>(s[i])))
            return false;
    }
    return true;
}

std::string userIdFor(const std::string& issuer,
                      const std::string& sub)
{
    if (isUuid(sub)) return sub;
    // RFC 4122 UUIDv5 in the URL namespace.
    static const unsigned char kNs[16] = {
        0x6b, 0xa7, 0xb8, 0x11, 0x9d, 0xad, 0x11, 0xd1,
        0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8};
    std::string data(reinterpret_cast<const char*>(kNs), 16);
    data += issuer + "#" + sub;
    unsigned char h[EVP_MAX_MD_SIZE];
    unsigned int n = 0;
    EVP_Digest(data.data(), data.size(), h, &n, EVP_sha1(),
               nullptr);
    h[6] = static_cast<unsigned char>((h[6] & 0x0f) | 0x50);
    h[8] = static_cast<unsigned char>((h[8] & 0x3f) | 0x80);
    static const char* kHex = "0123456789abcdef";
    std::string out;
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) out += '-';
        out += kHex[h[i] >> 4];
        out += kHex[h[i] & 0x0f];
    }
    return out;
}

KeycloakVerifier::KeycloakVerifier(JwksClient& jwks)
    : jwks_(jwks)
{
}

static bool audOk(const jwt::decoded_jwt<
                  jwt::traits::nlohmann_json>& d,
                  const std::string& clientId)
{
    if (d.has_audience()) {
        for (const auto& a : d.get_audience())
            if (a == clientId) return true;
    }
    const auto& payload = d.get_payload_json();
    return payload.count("azp")
           && payload.at("azp").is_string()
           && payload.at("azp").get<std::string>()
                  == clientId;
}

std::optional<KeycloakClaims> KeycloakVerifier::verify(
    const std::string& token) const
{
    const auto issuer = oidcIssuer();
    const auto clientId = oidcClientId();
    if (issuer.empty() || clientId.empty()) {
        spdlog::warn(
            "OIDC_ISSUER / OIDC_CLIENT_ID not set");
        return std::nullopt;
    }
    try {
        auto decoded = jwt::decode(token);
        if (!decoded.has_key_id()) return std::nullopt;
        auto pem = jwks_.getPublicKeyPem(
            decoded.get_key_id());
        if (!pem) return std::nullopt;

        auto v = jwt::verify()
                     .allow_algorithm(
                         jwt::algorithm::rs256(*pem))
                     .with_issuer(issuer);
        v.verify(decoded);  // exp + signature + iss

        if (!audOk(decoded, clientId)) return std::nullopt;

        KeycloakClaims c;
        c.sub = decoded.get_subject();
        if (c.sub.empty()) return std::nullopt;
        c.userId = userIdFor(issuer, c.sub);
        const auto& payload = decoded.get_payload_json();
        const auto str = [&](const char* k) {
            return payload.count(k) && payload.at(k).is_string()
                       ? payload.at(k).get<std::string>()
                       : std::string{};
        };
        c.preferredUsername = str("preferred_username");
        c.email = str("email");
        c.name = str("name");
        if (!str("role").empty()) c.roles.push_back(str("role"));
        if (payload.count("roles")
            && payload.at("roles").is_array()) {
            for (const auto& r : payload.at("roles"))
                if (r.is_string())
                    c.roles.push_back(r.get<std::string>());
        }
        return c;
    } catch (const std::exception& e) {
        spdlog::debug("OIDC verify fail: {}", e.what());
        return std::nullopt;
    }
}

KeycloakVerifier& defaultVerifier()
{
    static JwksClient jwks{oidcJwksUrl()};
    static KeycloakVerifier v{jwks};
    return v;
}

} // namespace services::auth::keycloak
