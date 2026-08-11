#pragma once
/**
 * @file sso_session_lookup.h
 * @brief DB query for the sso-session endpoint.
 *
 * Given a validated userId, fetches the user row and
 * issues a fresh access token, then calls onSuccess
 * with {accessToken, user} or onError on failure.
 */

#include "auth_service_types.h"
#include "drogon-host/backend/utils/JwtUtil.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace services
{

/**
 * @brief Fetch user profile + issue access token.
 * @param userId    UUID string from JWT claims.
 * @param onSuccess Called with {accessToken, user}.
 * @param onError   Called with HTTP status + message.
 */
inline void ssoSessionLookup(
    const std::string& userId,
    Callback onSuccess, ErrCallback onError)
{
    auto db = drogon::app().getDbClient();
    *db << "SELECT id, email, username,"
           " display_name, role"
           " FROM users WHERE id=$1"
        << userId >>
        [onSuccess, onError, userId](
            const drogon::orm::Result& r) {
            if (r.empty()) {
                onError(drogon::k401Unauthorized,
                        "User not found");
                return;
            }
            auto row  = r[0];
            auto role = row["role"]
                .as<std::string>();
            auto access =
                ::utils::generateAccessToken(
                    userId, role);
            json user = {
                {"id", userId},
                {"email",
                 row["email"].as<std::string>()},
                {"username",
                 row["username"].as<std::string>()},
                {"displayName",
                 row["display_name"]
                     .as<std::string>()},
                {"role", role},
            };
            onSuccess({{"accessToken", access},
                       {"user", user}});
        } >>
        [onError](
            const drogon::orm::
                DrogonDbException& e) {
            spdlog::error(
                "sso-session DB: {}",
                e.base().what());
            onError(
                drogon::k500InternalServerError,
                "Internal server error");
        };
}

} // namespace services
