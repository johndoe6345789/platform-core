#pragma once
/**
 * @file registration_db_handler.h
 * @brief Builds user JSON from registration DB result
 *        and handles duplicate-key errors.
 */

#include "auth_service_types.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using json = nlohmann::json;

/**
 * @brief Build JSON from a registration DB row.
 * @param r The DB result row.
 * @return User profile JSON.
 */
inline auto buildRegisteredUser(
    const drogon::orm::Row& r) -> json
{
    return {
        {"id", r["id"].as<std::string>()},
        {"email",
         r["email"].as<std::string>()},
        {"username",
         r["username"].as<std::string>()},
        {"displayName",
         r["display_name"]
             .as<std::string>()},
        {"emailConfirmed", false},
        {"createdAt",
         r["created_at"].as<std::string>()}};
}

/**
 * @brief Handle registration DB exceptions.
 * @param e       The Drogon DB exception.
 * @param onError Error callback.
 */
inline void handleRegistrationDbError(
    const drogon::orm::DrogonDbException& e,
    ErrCallback onError)
{
    std::string msg = e.base().what();
    if (msg.find("users_email_key") !=
        std::string::npos) {
        onError(drogon::k409Conflict,
                "Email already registered");
    } else if (msg.find("users_username_key") !=
               std::string::npos) {
        onError(drogon::k409Conflict,
                "Username already taken");
    } else {
        spdlog::error(
            "registerUser DB error: {}", msg);
        onError(drogon::k500InternalServerError,
                "Internal server error");
    }
}

} // namespace services
