#pragma once
/**
 * @file KeycloakVerifier.h
 * @brief Verifies RS256 access tokens from the OIDC provider
 *        (dbal) named by OIDC_ISSUER / OIDC_CLIENT_ID /
 *        OIDC_JWKS_URL.
 */

#include "JwksClient.h"

#include <optional>
#include <string>
#include <vector>

namespace services::auth::keycloak
{

/**
 * @brief Subset of token claims used downstream.
 */
struct KeycloakClaims
{
    std::string sub;
    /// UUID for users.id: sub itself if it is a UUID, else
    /// a UUIDv5 of issuer + sub (dbal's sub is a username).
    std::string userId;
    std::string preferredUsername;
    std::string email;
    std::string name;
    std::vector<std::string> roles;
};

/** @brief OIDC_ISSUER: the exact `iss` tokens must carry. */
[[nodiscard]] std::string oidcIssuer();
/** @brief OIDC_CLIENT_ID: required in `aud` or `azp`. */
[[nodiscard]] std::string oidcClientId();
/** @brief OIDC_JWKS_URL: where signing keys are fetched. */
[[nodiscard]] std::string oidcJwksUrl();

/**
 * @brief Stable users.id for a token subject.
 * @param issuer Token issuer.
 * @param sub Token subject.
 * @return Lower-case UUID string.
 */
[[nodiscard]] std::string userIdFor(const std::string& issuer,
                                    const std::string& sub);

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
