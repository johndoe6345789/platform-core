#pragma once
/**
 * @file token_role_lookup.h
 * @brief Role-lookup callback builder for
 *        TokenRefreshService.
 */

#include "auth_service_types.h"
#include "token_blocklist_check.h"
#include "drogon-host/backend/utils/JwtUtil.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

/**
 * @brief Issue a new access token after role lookup.
 * @param claims    Verified JWT claims.
 * @param onSuccess Callback with new access token.
 * @param onError   Error callback.
 */
inline void lookupRoleAndIssue(
    const ::utils::JwtClaims& claims,
    Callback onSuccess, ErrCallback onError)
{
    auto dbInner =
        drogon::app().getDbClient();

    *dbInner << std::string(kRoleLookupSql)
             << claims.userId >>
        [onSuccess, onError, claims](
            const drogon::orm::Result& r2) {
            std::string role = "user";
            if (!r2.empty()) {
                role = r2[0]["role"]
                    .as<std::string>();
            }
            auto tok =
                ::utils::generateAccessToken(
                    claims.userId, role);
            onSuccess({{"accessToken", tok}});
        } >>
        [onError](
            const drogon::orm::DrogonDbException& e) {
            spdlog::error(
                "refreshAccessToken DB "
                "error: {}",
                e.base().what());
            onError(
                drogon::k500InternalServerError,
                "Internal server error");
        };
}

} // namespace services
