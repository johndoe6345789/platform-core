/**
 * @file token_refresh_service.cpp
 * @brief Access-token refresh using a valid
 *        refresh JWT.
 */

#include "auth/backend/token_refresh_service.h"
#include "auth/backend/auth_helpers.h"
#include "auth/backend/token_blocklist_check.h"
#include "auth/backend/token_role_lookup.h"
#include "drogon-host/backend/utils/JwtUtil.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void TokenRefreshService::refreshAccessToken(
    const std::string& refreshToken,
    Callback onSuccess, ErrCallback onError)
{
    try {
        auto claims =
            ::utils::verifyToken(refreshToken);
        if (!claims.isRefresh) {
            onError(k401Unauthorized,
                    "Invalid token");
            return;
        }

        auto dbClient = authDb();
        *dbClient
            << std::string(kBlocklistCheckSql)
            << refreshToken >>
            [onSuccess, onError, claims](
                const Result& result) {
                if (!result.empty()) {
                    onError(k401Unauthorized,
                            "Token revoked");
                    return;
                }
                lookupRoleAndIssue(
                    claims, onSuccess, onError);
            } >>
            [onError](const DrogonDbException& e) {
                spdlog::error(
                    "refreshAccessToken blocklist "
                    "check error: {}",
                    e.base().what());
                onError(k500InternalServerError,
                        "Internal server error");
            };
    } catch (const std::exception&) {
        onError(k401Unauthorized,
                "Invalid token");
    }
}

} // namespace services
