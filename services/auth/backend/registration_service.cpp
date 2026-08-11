/** @file registration_service.cpp */
#include "auth/backend/registration_service.h"
#include "auth/backend/auth_helpers.h"
#include "auth/backend/registration_db_handler.h"
#include "drogon-host/backend/utils/PasswordHash.h"
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace services
{
using namespace drogon;
using namespace drogon::orm;

void RegistrationService::registerUser(
    const std::string& email,
    const std::string& username,
    const std::string& password,
    const std::string& displayName,
    Callback onSuccess, ErrCallback onError)
{
    if (!isValidAuthEmail(email)) {
        onError(k400BadRequest, "Invalid email format");
        return;
    }
    if (!isValidAuthUsername(username)) {
        onError(k400BadRequest,
                "Username must be 3-30 alphanumeric "
                "characters");
        return;
    }
    if (!isStrongAuthPassword(password)) {
        onError(k400BadRequest,
                "Password must be at least 8 chars "
                "with upper, lower, and digit");
        return;
    }
    auto dbClient = authDb();
    auto token = generateRandomToken();
    auto hashed = ::utils::hashPassword(password);
    auto display =
        displayName.empty() ? username : displayName;
    const std::string sql = R"(
        INSERT INTO users
            (email, username, password_hash,
             display_name, email_confirm_token,
             created_at, updated_at)
        VALUES ($1,$2,$3,$4,$5,NOW(),NOW())
        RETURNING id, email, username,
                  display_name, email_confirmed,
                  created_at
    )";
    *dbClient << sql << email << username << hashed
              << display << token >>
        [onSuccess, onError, username, email](
            const Result& result) {
            if (result.empty()) {
                spdlog::error(
                    "registerUser: INSERT returned"
                    " no rows for {} ({})",
                    username, email);
                onError(k500InternalServerError,
                        "Internal server error");
                return;
            }
            auto user = buildRegisteredUser(
                result[0]);
            spdlog::info("User registered: {} ({})",
                         username, email);
            onSuccess(user);
        } >>
        [onError](const DrogonDbException& e) {
            handleRegistrationDbError(e, onError);
        };
}
} // namespace services
