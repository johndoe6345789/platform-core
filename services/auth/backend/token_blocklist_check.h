#pragma once
/**
 * @file token_blocklist_check.h
 * @brief Blocklist-check SQL and role-lookup logic for
 *        token refresh.
 */

#include <string_view>

namespace services
{

/// @brief SQL to check if a token is blocklisted.
constexpr std::string_view kBlocklistCheckSql = R"(
    SELECT 1 FROM token_blocklist
    WHERE jti = $1  LIMIT 1
)";

/// @brief SQL to fetch a user's role by ID.
constexpr std::string_view kRoleLookupSql = R"(
    SELECT role FROM users
    WHERE id = $1
)";

} // namespace services
