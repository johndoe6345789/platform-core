#pragma once
/**
 * @file jwt_config.h
 * @brief JWT configuration helpers: secret and issuer
 *        resolution from Drogon config / environment.
 */

#include <cstdlib>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <string>

namespace utils
{

/// @brief Default JWT issuer string.
constexpr auto kDefaultIssuer = "next-extra";

/// @brief Access token lifetime in minutes.
constexpr int kAccessMinutes = 15;

/// @brief Refresh token lifetime in days.
constexpr int kRefreshDays = 30;

/**
 * @brief Read a string from the jwt config block.
 * @param key Config key name.
 * @return Value or empty string if missing.
 */
inline auto cfgVal(
    const std::string& key) -> std::string
{
    auto& jwt =
        drogon::app().getCustomConfig()["jwt"];
    if (jwt.isMember(key) && jwt[key].isString()
        && !jwt[key].asString().empty()) {
        return jwt[key].asString();
    }
    return {};
}

/**
 * @brief Retrieve the JWT signing secret.
 *
 * Resolution order: custom_config -> JWT_SECRET
 * env -> abort.
 *
 * @return The secret string.
 */
inline auto getJwtSecret() -> std::string
{
    auto val = cfgVal("secret");
    if (!val.empty()) {
        return val;
    }
    if (const char* env =
            std::getenv("JWT_SECRET");
        env != nullptr && env[0] != '\0') {
        return std::string{env};
    }
    spdlog::critical(
        "JWT_SECRET not set; aborting");
    std::abort();
}

/**
 * @brief Retrieve the JWT issuer string.
 * @return Issuer from config or default.
 */
inline auto getJwtIssuer() -> std::string
{
    auto val = cfgVal("issuer");
    return val.empty() ? kDefaultIssuer : val;
}

} // namespace utils
