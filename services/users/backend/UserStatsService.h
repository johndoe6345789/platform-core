#pragma once
/**
 * @file UserStatsService.h
 * @brief Aggregated statistics retrieval for a user.
 *
 * Queries points, streaks, badge count, and message count
 * for a given user and derives a level from a fixed
 * threshold table.
 */

#include "users/backend/user_service_types.h"

#include <string>

namespace services
{

/**
 * @class UserStatsService
 * @brief Retrieve aggregated statistics for a user.
 */
class UserStatsService
{
  public:
    UserStatsService() = default;
    ~UserStatsService() = default;

    /**
     * @brief Get aggregated statistics for a user.
     *
     * Returns total points, derived level, current/longest
     * streaks, badge count, and message count.
     *
     * @param userId    User UUID.
     * @param onSuccess Callback with stats JSON.
     * @param onError   Callback on failure.
     */
    void getUserStats(const std::string& userId, Callback onSuccess,
                      ErrCallback onError);

  private:
    /// Convenience DB accessor.
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
