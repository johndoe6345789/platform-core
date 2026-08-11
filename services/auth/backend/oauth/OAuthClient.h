#pragma once
/**
 * @file OAuthClient.h
 * @brief Generic PKCE OAuth client used by per-provider glue.
 */

#include "OAuthTypes.h"

#include <string>

namespace services::auth::oauth
{

/** @brief Build an authorize URL with PKCE code challenge. */
std::string buildAuthorizeUrl(
    const ProviderConfig& cfg,
    const std::string& state,
    const std::string& codeChallenge,
    const std::string& redirectUri);

/**
 * @brief Exchange an authorization code for tokens.
 *        Uses Drogon HttpClient under the hood.
 * @return Parsed TokenResponse or a zero-filled struct on err.
 */
TokenResponse exchangeCode(
    const ProviderConfig& cfg,
    const std::string& code,
    const std::string& codeVerifier,
    const std::string& redirectUri);

/** @brief Generate a random PKCE verifier + S256 challenge. */
std::pair<std::string, std::string> generatePkcePair();

} // namespace services::auth::oauth
