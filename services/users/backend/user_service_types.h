#pragma once
/**
 * @file user_service_types.h
 * @brief Shared type aliases and data structures for user services.
 *
 * All user-domain service files include this header to share
 * common type definitions without creating circular dependencies.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

#include <cstdint>
#include <functional>
#include <string>

namespace services
{

using json = nlohmann::json;
using DbClientPtr = drogon::orm::DbClientPtr;
using Callback = std::function<void(json)>;
using ErrCallback = std::function<void(drogon::HttpStatusCode, std::string)>;

/**
 * @brief Aggregate statistics for a single user.
 */
struct UserStats {
    std::int64_t totalPoints{0};
    std::int32_t level{1};
    std::int32_t currentStreak{0};
    std::int32_t longestStreak{0};
    std::int64_t badgeCount{0};
    std::int64_t messageCount{0};
};

} // namespace services
