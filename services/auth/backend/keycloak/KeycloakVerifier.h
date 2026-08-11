#pragma once
/**
 * @file KeycloakVerifier.h
 * @brief Verifies Keycloak-issued RS256 access tokens.
 */

#include "JwksClient.h"

#include <optional>
#include <string>
#include <vector>

namespace services::auth::keycloak
{

/**
 * @brief Subset of Keycloak claims used downstream.
 */
struct KeycloakClaims
{
    std::string sub;
    std::string preferredUsername;
    std::string email;
    std::string name;
    std::vector<std::string> roles;
};

/// Fixed contract — see CLAUDE.md / migration plan.
inline constexpr const char* kKcIssuer =
    "http://localhost:8889/sso/realms/nextra";
inline constexpr const char* kKcClientId = "nextra-app";
inline constexpr const char* kKcInternalJwks =
    "http://keycloak:8080/sso/realms/nextra/"
    "protocol/openid-connect/certs";

/**
 * @brief RS256 verifier backed by a JwksClient.
 */
class KeycloakVerifier
{
  public:
    /**
     * @brief Construct with a JwksClient reference.
     * @param jwks Reference to a long-lived JwksClient.
     */
    explicit KeycloakVerifier(JwksClient& jwks);

    /**
     * @brief Verify a JWT, returning extracted claims.
     * @param token The compact-serialized JWT string.
     * @return Claims on success, std::nullopt on any failure.
     */
    [[nodiscard]] std::optional<KeycloakClaims> verify(
        const std::string& token) const;

  private:
    JwksClient& jwks_;
};

/**
 * @brief Process-wide verifier accessor.
 * @return Singleton KeycloakVerifier.
 */
[[nodiscard]] KeycloakVerifier& defaultVerifier();

} // namespace services::auth::keycloak
