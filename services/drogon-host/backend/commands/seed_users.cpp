/**
 * @file seed_users.cpp
 * @brief User + streak seed loading.
 */

#include "seed_handlers.h"
#include "seed_io.h"
#include "auth/backend/auth_helpers.h"
#include "drogon-host/backend/utils/PasswordHash.h"

#include <stdexcept>
#include <string>

namespace commands::seed
{

void loadUsers(const std::filesystem::path& path,
               const drogon::orm::DbClientPtr& db)
{
    for (const auto& user : readSeedArray(path)) {
        auto email = user.at("email").get<std::string>();
        auto name = user.at("username").get<std::string>();
        auto pw = user.at("password").get<std::string>();
        if (!services::isValidAuthEmail(email))
            throw std::runtime_error("Invalid seed email");
        if (!services::isValidAuthUsername(name))
            throw std::runtime_error("Invalid seed username");
        if (!services::isStrongAuthPassword(pw))
            throw std::runtime_error("Weak seed password");
        auto rows = db->execSqlSync(
            "INSERT INTO users (email, username, display_name,"
            " password_hash, role, is_active, email_confirmed,"
            " total_points, current_level, created_at, updated_at)"
            " VALUES ($1,$2,$3,$4,$5,$6,$7,$8,$9,NOW(),NOW())"
            " ON CONFLICT (email) DO UPDATE SET"
            " username = EXCLUDED.username,"
            " display_name = EXCLUDED.display_name,"
            " password_hash = EXCLUDED.password_hash,"
            " role = EXCLUDED.role,"
            " is_active = EXCLUDED.is_active,"
            " email_confirmed = EXCLUDED.email_confirmed,"
            " total_points = EXCLUDED.total_points,"
            " current_level = EXCLUDED.current_level,"
            " updated_at = NOW() RETURNING id",
            email, name, user.value("display_name", name),
            utils::hashPassword(pw), user.value("role", "user"),
            user.value("is_active", true),
            user.value("email_confirmed", false),
            user.value("total_points", 0),
            user.value("current_level", 1));
        if (rows.empty())
            throw std::runtime_error("User upsert returned no id");
        auto userId = rows[0]["id"].as<std::string>();
        db->execSqlSync(
            "INSERT INTO streaks (user_id, current_streak,"
            " longest_streak, last_activity_date)"
            " VALUES ($1,$2,$3,CURRENT_DATE)"
            " ON CONFLICT (user_id) DO UPDATE SET"
            " current_streak = EXCLUDED.current_streak,"
            " longest_streak = EXCLUDED.longest_streak,"
            " last_activity_date = EXCLUDED.last_activity_date",
            userId, user.value("streak_current", 0),
            user.value("streak_longest", 0));
    }
}

} // namespace commands::seed
