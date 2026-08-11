#pragma once
/**
 * @file auth_token_refresh.h
 * @brief Refresh-token extraction helper for
 *        AuthTokenController.
 */

#include <nlohmann/json.hpp>
#include <string>

namespace controllers
{

using json = nlohmann::json;

/**
 * @brief Extract the refresh token from a request body.
 *
 * Accepts either "refreshToken" (camelCase) or
 * "refresh_token" (snake_case) field names.
 *
 * @param body  Parsed JSON request body.
 * @return The refresh token string, or empty.
 */
inline auto extractRefreshToken(
    const json& body) -> std::string
{
    std::string rt;
    if (body.contains("refreshToken")) {
        rt = body["refreshToken"]
                 .get<std::string>();
    } else if (body.contains("refresh_token")) {
        rt = body["refresh_token"]
                 .get<std::string>();
    }
    return rt;
}

} // namespace controllers
