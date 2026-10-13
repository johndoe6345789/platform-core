/**
 * @file PublicProfileController.cpp
 * @brief GET /api/users/profile/{username}
 */

#include "PublicProfileController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace controllers
{

using json = nlohmann::json;

void PublicProfileController::getPublicProfile(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& username)
{
    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "SELECT u.username,"
        "       u.display_name,"
        "       u.bio,"
        "       u.created_at AS joined_at,"
        "       COALESCE(p.startup_name, '') AS startup_name,"
        "       COALESCE(p.startup_type, '') AS startup_type,"
        "       COALESCE(p.stage, '') AS stage,"
        "       COALESCE(p.is_mentor, FALSE) AS is_mentor,"
        "       COALESCE(gs.badge_count, 0) AS badge_count,"
        "       COALESCE(st.current_streak, 0)"
        "           AS current_streak"
        " FROM users u"
        " LEFT JOIN user_profiles p"
        "        ON p.user_id = u.id"
        " LEFT JOIN gamification_stats gs"
        "        ON gs.user_id = u.id"
        " LEFT JOIN streak_stats st"
        "        ON st.user_id = u.id"
        " WHERE u.username = $1"
        "   AND u.active = TRUE"
        " LIMIT 1",
        [cb](const drogon::orm::Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            const auto& row = r[0];
            json out;
            out["username"]    = row["username"].as<std::string>();
            out["displayName"] = row["display_name"].as<std::string>();
            out["bio"]         = row["bio"].as<std::string>();
            out["joined_at"]   = row["joined_at"].as<std::string>();
            out["startupName"] = row["startup_name"].as<std::string>();
            out["startupType"] = row["startup_type"].as<std::string>();
            out["stage"]       = row["stage"].as<std::string>();
            out["is_mentor"]   = row["is_mentor"].as<bool>();
            out["badge_count"] = row["badge_count"].as<int>();
            out["current_streak"] =
                row["current_streak"].as<int>();
            cb(::utils::jsonOk(out));
        },
        [cb](const drogon::orm::DrogonDbException& e) {
            spdlog::error("public-profile DB: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "DB error"));
        },
        username);
}

} // namespace controllers
