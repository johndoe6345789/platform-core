/**
 * @file password_reset_execute.cpp
 * @brief Execute a password reset with a valid token.
 */

#include "auth/backend/password_reset_execute.h"
#include "auth/backend/auth_helpers.h"
#include "drogon-host/backend/utils/PasswordHash.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void PasswordResetExecute::execute(
    const std::string& token,
    const std::string& newPassword,
    Callback onSuccess, ErrCallback onError)
{
    if (!isStrongAuthPassword(newPassword)) {
        onError(k400BadRequest,
                "Password must be at least 8 chars "
                "with upper, lower, and digit");
        return;
    }

    auto hashed = ::utils::hashPassword(newPassword);
    auto dbClient = authDb();

    const std::string sql = R"(
        UPDATE users
        SET password_hash = $1,
            password_reset_token = NULL,
            password_reset_expires = NULL,
            updated_at = NOW()
        WHERE password_reset_token = $2
          AND password_reset_expires > NOW()
        RETURNING id
    )";

    *dbClient << sql << hashed << token >>
        [onSuccess, onError](const Result& result) {
            if (result.empty()) {
                onError(k400BadRequest,
                        "Invalid or expired "
                        "reset token");
                return;
            }
            spdlog::info(
                "Password reset for user {}",
                result[0]["id"].as<std::string>());
            onSuccess({{"reset", true}});
        } >>
        [onError](const DrogonDbException& e) {
            spdlog::error(
                "resetPassword DB error: {}",
                e.base().what());
            onError(k500InternalServerError,
                    "Internal server error");
        };
}

} // namespace services
