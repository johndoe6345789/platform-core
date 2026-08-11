#pragma once
/**
 * @file GithubProvider.h
 * @brief GitHub OAuth provider glue.
 */

#include "OAuthTypes.h"

#include <nlohmann/json.hpp>

namespace services::auth::oauth::github
{

/** @brief Map a GitHub /user JSON blob to UserProfile. */
UserProfile mapProfile(const nlohmann::json& j);

} // namespace services::auth::oauth::github
