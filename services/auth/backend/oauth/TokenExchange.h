#pragma once
/**
 * @file TokenExchange.h
 * @brief Blocking HTTP POST to the provider token endpoint.
 */

#include "OAuthTypes.h"

#include <string>

namespace services::auth::oauth
{

/**
 * @brief Send a form-encoded authorization_code exchange.
 * @param cfg          Resolved provider config.
 * @param code         Authorization code from redirect.
 * @param codeVerifier PKCE verifier.
 * @param redirectUri  Original redirect URI.
 * @param acceptJson   Set true for GitHub to force JSON.
 * @return Parsed TokenResponse (empty on failure).
 */
[[nodiscard]] TokenResponse postTokenRequest(
    const ProviderConfig& cfg,
    const std::string& code,
    const std::string& codeVerifier,
    const std::string& redirectUri,
    bool acceptJson);

} // namespace services::auth::oauth
