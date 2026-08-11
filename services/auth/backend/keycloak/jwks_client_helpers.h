#pragma once
/**
 * @file jwks_client_helpers.h
 * @brief Helpers for converting JWK n/e components into PEM
 *        and for splitting JWKS URLs.
 */

#include <string>
#include <utility>

namespace services::auth::keycloak::detail
{

/**
 * @brief Convert base64url-encoded RSA n/e to a PEM public key.
 * @param nB64u Base64url-encoded modulus.
 * @param eB64u Base64url-encoded exponent.
 * @return PEM-encoded public key, or empty string on error.
 */
[[nodiscard]] std::string rsaPemFromJwk(
    const std::string& nB64u, const std::string& eB64u);

/**
 * @brief Split URL into (host base, path).
 * @param url Absolute URL.
 * @return Pair of (scheme://host[:port], path).
 */
[[nodiscard]] std::pair<std::string, std::string>
splitUrl(const std::string& url);

} // namespace services::auth::keycloak::detail
