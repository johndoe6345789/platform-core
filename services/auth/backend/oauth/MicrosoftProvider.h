#pragma once
/**
 * @file MicrosoftProvider.h
 * @brief Microsoft Graph OAuth provider glue.
 */

#include "OAuthTypes.h"

#include <nlohmann/json.hpp>

namespace services::auth::oauth::microsoft
{

/** @brief Map a Graph /oidc/userinfo JSON to UserProfile. */
UserProfile mapProfile(const nlohmann::json& j);

} // namespace services::auth::oauth::microsoft
