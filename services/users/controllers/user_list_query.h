#pragma once
/**
 * @file user_list_query.h
 * @brief SQL and pagination constants for the user
 *        list endpoint.
 */

#include <string_view>

namespace controllers
{

/// @brief Count total users.
constexpr std::string_view kUserCountSql =
    "SELECT COUNT(*) AS total FROM users";

/// @brief Paginated user list query.
constexpr std::string_view kUserListSql = R"(
    SELECT id, email, username,
      display_name, role,
      is_active, created_at
    FROM users
    ORDER BY created_at DESC
    LIMIT $1 OFFSET $2
)";

} // namespace controllers
