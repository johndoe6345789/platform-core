#pragma once
/**
 * @file dashboard_query.h
 * @brief SQL query for dashboard aggregation.
 */

#include <string_view>

namespace controllers
{

/**
 * @brief Dashboard stats SQL query.
 *
 * Aggregates points, level, streaks, badge count,
 * unread notifications, and rank for a given user.
 */
constexpr std::string_view kDashboardStatsSql = R"(
    SELECT
        u.total_points,
        u.current_level,
        COALESCE(s.current_streak, 0)
            AS current_streak,
        COALESCE(s.longest_streak, 0)
            AS longest_streak,
        COUNT(DISTINCT ub.badge_id)
            AS badge_count,
        (SELECT COUNT(*) FROM notifications n
         WHERE n.user_id = u.id
           AND n.is_read = FALSE)
            AS unread_count,
        (SELECT RANK() OVER (
             ORDER BY total_points DESC)
         FROM users
         WHERE id = u.id)
            AS rank
    FROM users u
    LEFT JOIN streaks s
        ON s.user_id = u.id
    LEFT JOIN user_badges ub
        ON ub.user_id = u.id
    WHERE u.id = $1
    GROUP BY u.id, s.current_streak,
             s.longest_streak
)";

} // namespace controllers
