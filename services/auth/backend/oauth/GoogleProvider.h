#pragma once
/**
 * @file GoogleProvider.h
 * @brief Google Identity Platform provider glue.
 */

#include "OAuthTypes.h"

#include <nlohmann/json.hpp>

namespace services::auth::oauth::google
{

/** @brief Map a Google userinfo JSON blob to UserProfile. */
UserProfile mapProfile(const nlohmann::json& j);

} // namespace services::auth::oauth::google
