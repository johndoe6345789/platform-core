#pragma once
/**
 * @file OAuthTypes.h
 * @brief Provider-neutral OAuth POD types.
 */

#include <string>

namespace services::auth::oauth
{

/** @brief Provider configuration loaded from constants JSON. */
struct ProviderConfig
{
    std::string name;
    std::string clientId;
    std::string clientSecret;
    std::string authorizeUrl;
    std::string tokenUrl;
    std::string userInfoUrl;
    std::string scope;
};

/** @brief Normalised user profile returned by providers. */
struct UserProfile
{
    std::string subject;
    std::string email;
    std::string displayName;
    std::string avatarUrl;
};

/** @brief Exchanged token response. */
struct TokenResponse
{
    std::string accessToken;
    std::string idToken;
    std::string refreshToken;
    std::string tokenType;
    int expiresIn{0};
};

} // namespace services::auth::oauth
