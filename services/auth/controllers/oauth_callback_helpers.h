#pragma once
/**
 * @file oauth_callback_helpers.h
 * @brief Shared helpers for the OAuth callback controller.
 */

#include "auth/backend/oauth/OAuthTypes.h"

#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>
#include <string>

namespace controllers::oauth_cb
{

/** @brief Map provider userinfo JSON to a UserProfile. */
[[nodiscard]] services::auth::oauth::UserProfile
mapProfile(const std::string& provider,
           const nlohmann::json& j);

/** @brief Attach the nextra_sso cookie to @p r. */
void attachSso(const drogon::HttpResponsePtr& r,
               const std::string& refreshToken);

} // namespace controllers::oauth_cb
