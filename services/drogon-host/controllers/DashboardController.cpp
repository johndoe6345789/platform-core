/**
 * @file DashboardController.cpp
 * @brief Dashboard stats endpoint implementation.
 */

#include "DashboardController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "dashboard_query.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon;
using namespace drogon::orm;

namespace controllers
{

void DashboardController::stats(
    const HttpRequestPtr& req, Cb&& cb)
{
    auto userId = req->attributes()->get<
        std::string>("user_id");

    auto dbClient = app().getDbClient();
    *dbClient << std::string(kDashboardStatsSql)
              << userId
              >> [cb](const Result& r) {
                     if (r.empty()) {
                         cb(::utils::jsonError(
                             k404NotFound,
                             "User not found"));
                         return;
                     }
                     const auto& row = r[0];
                     cb(::utils::jsonOk({
                         {"totalPoints",
                          row["total_points"]
                              .as<int64_t>()},
                         {"currentLevel",
                          row["current_level"]
                              .as<int32_t>()},
                         {"currentStreak",
                          row["current_streak"]
                              .as<int32_t>()},
                         {"longestStreak",
                          row["longest_streak"]
                              .as<int32_t>()},
                         {"badgeCount",
                          row["badge_count"]
                              .as<int64_t>()},
                         {"unreadNotifications",
                          row["unread_count"]
                              .as<int64_t>()},
                         {"rank",
                          row["rank"]
                              .as<int64_t>()},
                     }));
                 }
              >> [cb](const DrogonDbException& e) {
                     spdlog::error("dashboard: {}",
                                   e.base().what());
                     cb(::utils::jsonError(
                         k500InternalServerError,
                         "Internal server error"));
                 };
}

} // namespace controllers
