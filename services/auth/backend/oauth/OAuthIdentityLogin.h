#pragma once
/**
 * @file OAuthIdentityLogin.h
 * @brief Upsert oauth identities, ensure a local user,
 *        and mint a Nextra session payload.
 */

#include "OAuthTypes.h"

#include <functional>
#include <nlohmann/json.hpp>
#include <string>

namespace services::auth::oauth
{

/**
 * @brief Callback shape for loginViaOAuth.
 * Receives (ok, payload) where payload contains
 * {tokens: {accessToken, refreshToken}, user: {...}}
 * matching the password-login response.
 */
using LoginCallback =
    std::function<void(bool, nlohmann::json)>;

/**
 * @brief Resolve or create a local user from an OAuth
 *        profile and mint a session.
 * @param provider Provider canonical name.
 * @param profile  Normalised profile from the provider.
 * @param cb       Invoked with the final payload.
 */
void loginViaOAuth(
    const std::string& provider,
    const UserProfile& profile,
    LoginCallback cb);

} // namespace services::auth::oauth
