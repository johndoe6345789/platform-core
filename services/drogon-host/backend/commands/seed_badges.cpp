/**
 * @file seed_badges.cpp
 * @brief Badge + criteria seed loading.
 */

#include "seed_handlers.h"
#include "seed_io.h"

#include <string>

namespace commands::seed
{

void loadBadges(const std::filesystem::path& path,
                const drogon::orm::DbClientPtr& db)
{
    for (const auto& badge : readSeedArray(path)) {
        auto rows = db->execSqlSync(
            "INSERT INTO badges (name, description, icon_url,"
            " category, points_required, created_at, updated_at)"
            " VALUES ($1,$2,$3,$4,$5,NOW(),NOW())"
            " ON CONFLICT (name) DO UPDATE SET"
            " description = EXCLUDED.description,"
            " icon_url = EXCLUDED.icon_url,"
            " category = EXCLUDED.category,"
            " points_required = EXCLUDED.points_required,"
            " updated_at = NOW() RETURNING id",
            badge.at("name").get<std::string>(),
            badge.value("description", std::string{}),
            badge.value("icon_url", std::string{}),
            badge.value("category", "general"),
            badge.value("points_required", 0));
        auto badgeId = rows[0]["id"].as<std::string>();
        db->execSqlSync(
            "DELETE FROM badge_criteria WHERE badge_id = $1",
            badgeId);
        auto criteria = badge.value("criteria", json::array());
        for (std::size_t i = 0; i < criteria.size(); ++i) {
            const auto& rule = criteria[i];
            db->execSqlSync(
                "INSERT INTO badge_criteria (badge_id, metric,"
                " operator, threshold, sort_order)"
                " VALUES ($1,$2,$3,$4,$5)",
                badgeId, rule.at("metric").get<std::string>(),
                rule.value("operator", ">="),
                rule.value("threshold", 0),
                static_cast<int>(i));
        }
    }
}

} // namespace commands::seed
