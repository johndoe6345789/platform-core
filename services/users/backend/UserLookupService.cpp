/**
 * @file UserLookupService.cpp
 * @brief Implementation of single-user lookup operations.
 */

#include "users/backend/UserLookupService.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto UserLookupService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

auto UserLookupService::rowToJson(const Row& row) -> json
{
    json user = {{"id", row["id"].as<std::string>()},
                 {"email", row["email"].as<std::string>()},
                 {"username", row["username"].as<std::string>()},
                 {"displayName", row["display_name"].as<std::string>()},
                 {"role", row["role"].as<std::string>()},
                 {"emailConfirmed", row["email_confirmed"].as<bool>()},
                 {"totalPoints", row["total_points"].as<std::int64_t>()},
                 {"currentStreak", row["current_streak"].as<std::int32_t>()},
                 {"createdAt", row["created_at"].as<std::string>()}};
    if (!row["avatar_url"].isNull()) {
        user["avatarUrl"] = row["avatar_url"].as<std::string>();
    }
    if (!row["bio"].isNull()) {
        user["bio"] = row["bio"].as<std::string>();
    }
    return user;
}

void UserLookupService::getUserById(const std::string& id, Callback onSuccess,
                                    ErrCallback onError)
{
    static const std::string kSql = R"(
        SELECT id, email, username, display_name,
               role, email_confirmed, total_points,
               current_streak, avatar_url, bio, created_at
        FROM users WHERE id = $1 LIMIT 1
    )";
    *db() << kSql << id >> [onSuccess, onError](const Result& r) {
        if (r.empty()) {
            onError(k404NotFound, "User not found");
            return;
        }
        onSuccess(rowToJson(r[0]));
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("getUserById DB error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

void UserLookupService::getUserByEmail(const std::string& email,
                                       Callback onSuccess, ErrCallback onError)
{
    static const std::string kSql = R"(
        SELECT id, email, username, display_name,
               role, email_confirmed, total_points,
               current_streak, avatar_url, bio, created_at
        FROM users WHERE email = $1 LIMIT 1
    )";
    *db() << kSql << email >> [onSuccess, onError](const Result& r) {
        if (r.empty()) {
            onError(k404NotFound, "User not found");
            return;
        }
        onSuccess(rowToJson(r[0]));
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("getUserByEmail DB error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

} // namespace services
