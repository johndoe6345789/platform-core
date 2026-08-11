/**
 * @file password_reset_request.cpp
 * @brief Request a password-reset email.
 */

#include "auth/backend/password_reset_request.h"
#include "auth/backend/auth_helpers.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void PasswordResetRequest::execute(
    const std::string& email,
    Callback onSuccess, ErrCallback onError)
{
    auto resetToken = generateRandomToken();
    auto dbClient = authDb();

    const std::string sql = R"(
        UPDATE users
        SET password_reset_token = $1,
            password_reset_expires = NOW()
                + INTERVAL '1 hour',
            updated_at = NOW()
        WHERE email = $2
        RETURNING id, username
    )";

    *dbClient << sql << resetToken << email >>
        [onSuccess, email](const Result& result) {
            if (!result.empty()) {
                spdlog::info(
                    "Password reset requested for {}",
                    email);
            }
            onSuccess(
                {{"message",
                  "If the email exists, a reset "
                  "link has been sent"}});
        } >>
        [onError](const DrogonDbException& e) {
            spdlog::error(
                "requestPasswordReset DB error: {}",
                e.base().what());
            onError(k500InternalServerError,
                    "Internal server error");
        };
}

} // namespace services
