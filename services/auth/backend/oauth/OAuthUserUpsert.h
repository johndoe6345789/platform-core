#pragma once
/**
 * @file OAuthUserUpsert.h
 * @brief Create a local user for an OAuth identity and
 *        mint a session JSON payload.
 */

#include "OAuthIdentityLogin.h"
#include "OAuthTypes.h"

#include <string>

namespace services::auth::oauth
{

/**
 * @brief Build the login JSON payload from a user row.
 * @param userId      UUID of the local user.
 * @param email       User's email address.
 * @param displayName Display name to embed.
 * @param role        Role string (usually "user").
 */
[[nodiscard]] nlohmann::json mintOAuthSession(
    const std::string& userId,
    const std::string& email,
    const std::string& displayName,
    const std::string& role);

/**
 * @brief Ensure a local user exists for the OAuth email and
 *        link the oauth_identities row, then run @p cb.
 */
void ensureUserAndLink(
    const std::string& provider,
    const UserProfile& profile,
    LoginCallback cb);

} // namespace services::auth::oauth
