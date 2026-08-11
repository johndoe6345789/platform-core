/**
 * @file token_service.cpp
 * @brief JWT refresh, logout, and blocklist operations.
 */

#include "auth/backend/token_service.h"
#include "auth/backend/auth_helpers.h"
#include "drogon-host/backend/utils/JwtUtil.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void TokenService::logoutUser(
    const std::string& jti,
    Callback onSuccess, ErrCallback onError)
{
    auto dbClient = authDb();
    const std::string sql = R"(
        INSERT INTO token_blocklist (jti, blocked_at)
        VALUES ($1, NOW())
        ON CONFLICT (jti) DO NOTHING
    )";

    *dbClient << sql << jti >>
        [onSuccess, jti](const Result&) {
            spdlog::info("Token blocked: {}...",
                         jti.substr(0, 8));
            onSuccess(json::object());
        } >>
        [onError](const DrogonDbException& e) {
            spdlog::error("logoutUser DB error: {}",
                          e.base().what());
            onError(k500InternalServerError,
                    "Internal server error");
        };
}

void TokenService::isTokenBlocked(
    const std::string& jti,
    std::function<void(bool)> callback)
{
    auto dbClient = authDb();
    const std::string sql = R"(
        SELECT 1 FROM token_blocklist
        WHERE jti = $1  LIMIT 1
    )";

    *dbClient << sql << jti >>
        [callback](const Result& result) {
            callback(!result.empty());
        } >>
        [callback](const DrogonDbException& e) {
            spdlog::error("isTokenBlocked DB error: {}",
                          e.base().what());
            callback(true);
        };
}

} // namespace services
