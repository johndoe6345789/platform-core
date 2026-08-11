/**
 * @file UserStats.cpp
 * @brief User gamification stats endpoint.
 */

#include "UserController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::getStats(
    const drogon::HttpRequestPtr& /*req*/,
    Cb&& cb, const std::string& id)
{
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        SELECT u.total_points,
               u.current_level,
               COALESCE(s.current_streak, 0)
                   AS streak
        FROM users u
        LEFT JOIN streaks s
            ON s.user_id = u.id
        WHERE u.id = $1
    )";

    *db << sql << id
        >> [id, cb](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            const auto& row = r[0];
            cb(::utils::jsonOk(
                {{"userId", id},
                 {"totalPoints",
                  row["total_points"]
                      .as<int64_t>()},
                 {"level",
                  row["current_level"]
                      .as<int32_t>()},
                 {"streakDays",
                  row["streak"]
                      .as<int32_t>()}}));
        }
        >> [id, cb](
               const DrogonDbException& e) {
            spdlog::error("getStats: {}",
                          e.base().what());
            cb(::utils::jsonOk(
                {{"userId", id},
                 {"totalPoints", 0},
                 {"level", 1},
                 {"streakDays", 0}}));
        };
}

} // namespace controllers
