/**
 * @file session_service.cpp
 * @brief User login and email confirmation.
 */

#include "auth/backend/session_service.h"
#include "auth/backend/auth_helpers.h"
#include "auth/backend/session_login_handler.h"
#include "drogon-host/backend/utils/JwtUtil.h"
#include "drogon-host/backend/utils/PasswordHash.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void SessionService::loginUser(
    const std::string& email,
    const std::string& password,
    Callback onSuccess, ErrCallback onError)
{
    if (email.empty() || password.empty()) {
        onError(k400BadRequest,
                "Email and password are required");
        return;
    }

    auto dbClient = authDb();
    const std::string sql = R"(
        SELECT id, email, username, display_name,
               password_hash, email_confirmed, role
        FROM users WHERE email = $1 LIMIT 1
    )";

    *dbClient << sql << email >>
        [onSuccess, onError, password](
            const Result& result) {
            if (result.empty()) {
                onError(k401Unauthorized,
                        "Invalid email or password");
                return;
            }
            const auto& row = result[0];
            auto stored =
                row["password_hash"].as<std::string>();

            if (!::utils::verifyPassword(
                    password, stored)) {
                onError(k401Unauthorized,
                        "Invalid email or password");
                return;
            }
            if (!row["email_confirmed"].as<bool>()) {
                onError(k403Forbidden,
                        "Please confirm your email");
                return;
            }

            auto userId = row["id"].as<std::string>();
            auto role = row["role"].as<std::string>();
            auto access =
                ::utils::generateAccessToken(
                    userId, role);
            auto refresh =
                ::utils::generateRefreshToken(userId);

            onSuccess(buildLoginPayload(
                row, access, refresh));
        } >>
        [onError](const DrogonDbException& e) {
            spdlog::error("loginUser DB error: {}",
                          e.base().what());
            onError(k500InternalServerError,
                    "Internal server error");
        };
}

} // namespace services
