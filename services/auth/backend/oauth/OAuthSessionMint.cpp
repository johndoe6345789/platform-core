/**
 * @file OAuthSessionMint.cpp
 * @brief Builds the JSON login payload mirroring the
 *        password-login shape.
 */

#include "OAuthUserUpsert.h"
#include "drogon-host/backend/utils/JwtUtil.h"

namespace services::auth::oauth
{

nlohmann::json mintOAuthSession(
    const std::string& userId,
    const std::string& email,
    const std::string& displayName,
    const std::string& role)
{
    auto access =
        ::utils::generateAccessToken(userId, role);
    auto refresh =
        ::utils::generateRefreshToken(userId);
    nlohmann::json user = {
        {"id", userId},
        {"email", email},
        {"displayName", displayName},
        {"role", role}};
    nlohmann::json tokens = {
        {"accessToken", access},
        {"refreshToken", refresh}};
    return {{"tokens", tokens}, {"user", user}};
}

} // namespace services::auth::oauth
