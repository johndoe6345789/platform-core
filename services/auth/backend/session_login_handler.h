#pragma once
/**
 * @file session_login_handler.h
 * @brief Builds the login response JSON payload.
 */

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using json = nlohmann::json;

/**
 * @brief Build login success payload from a DB row.
 * @param row     DB result row for the user.
 * @param access  Generated access token.
 * @param refresh Generated refresh token.
 * @return JSON with tokens and user profile.
 */
inline auto buildLoginPayload(
    const drogon::orm::Row& row,
    const std::string& access,
    const std::string& refresh) -> json
{
    auto userId =
        row["id"].as<std::string>();
    auto role =
        row["role"].as<std::string>();

    json user = {
        {"id", userId},
        {"email",
         row["email"].as<std::string>()},
        {"username",
         row["username"].as<std::string>()},
        {"displayName",
         row["display_name"]
             .as<std::string>()},
        {"role", role}};

    json tokens = {{"accessToken", access},
                   {"refreshToken", refresh}};
    json payload = {{"tokens", tokens},
                    {"user", user}};

    spdlog::info("User logged in: {}", userId);
    return payload;
}

} // namespace services
