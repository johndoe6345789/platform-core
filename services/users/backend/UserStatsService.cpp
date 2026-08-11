/**
 * @file UserStatsService.cpp
 * @brief Implementation of aggregated user statistics.
 */

#include "users/backend/UserStatsService.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

#include <array>
#include <utility>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto UserStatsService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void UserStatsService::getUserStats(const std::string& userId,
                                    Callback onSuccess, ErrCallback onError)
{
    auto dbClient = db();
    const std::string sql = R"(
        SELECT u.total_points, u.current_streak,
               u.longest_streak,
               COUNT(DISTINCT ub.badge_id) AS badge_count,
               COUNT(DISTINCT cm.id)       AS message_count
        FROM users u
        LEFT JOIN user_badges ub ON ub.user_id = u.id
        LEFT JOIN chat_messages cm ON cm.user_id = u.id
        WHERE u.id = $1
        GROUP BY u.id
    )";

    *dbClient << sql << userId >> [onSuccess, onError](const Result& result) {
        if (result.empty()) {
            onError(k404NotFound, "User not found");
            return;
        }
        const auto& r = result[0];
        auto pts = r["total_points"].as<std::int64_t>();

        constexpr std::array<std::pair<std::int64_t, std::int32_t>, 8>
            kThresholds = {{{10000, 8},
                            {5000, 7},
                            {2000, 6},
                            {1000, 5},
                            {500, 4},
                            {250, 3},
                            {100, 2},
                            {0, 1}}};
        std::int32_t level = 1;
        for (auto [threshold, lv] : kThresholds) {
            if (pts >= threshold) {
                level = lv;
                break;
            }
        }

        onSuccess({{"totalPoints", pts},
                   {"level", level},
                   {"currentStreak", r["current_streak"].as<std::int32_t>()},
                   {"longestStreak", r["longest_streak"].as<std::int32_t>()},
                   {"badgeCount", r["badge_count"].as<std::int64_t>()},
                   {"messageCount", r["message_count"].as<std::int64_t>()}});
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("getUserStats DB error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

} // namespace services
