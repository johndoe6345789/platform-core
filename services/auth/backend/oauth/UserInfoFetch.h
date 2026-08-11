#pragma once
/**
 * @file UserInfoFetch.h
 * @brief Blocking GET to the provider userinfo endpoint.
 */

#include "OAuthTypes.h"

#include <nlohmann/json.hpp>
#include <string>

namespace services::auth::oauth
{

/**
 * @brief Fetch the provider userinfo endpoint with @p token.
 * @param cfg         Resolved provider config.
 * @param accessToken OAuth2 access token string.
 * @return Parsed JSON body; returns null on failure.
 */
[[nodiscard]] nlohmann::json
fetchUserInfo(const ProviderConfig& cfg,
              const std::string& accessToken);

} // namespace services::auth::oauth
