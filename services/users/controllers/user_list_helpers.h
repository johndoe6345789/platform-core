#pragma once
/**
 * @file user_list_helpers.h
 * @brief Helper to build user JSON from a DB row.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

namespace controllers::detail
{

/**
 * @brief Build a user JSON object from a DB row.
 * @param row Database result row.
 * @return JSON object with user fields.
 */
inline auto userFromRow(
    const drogon::orm::Row& row)
    -> nlohmann::json
{
    return {
        {"id", row["id"].as<std::string>()},
        {"email",
         row["email"].as<std::string>()},
        {"username",
         row["username"].as<std::string>()},
        {"displayName",
         row["display_name"]
             .as<std::string>()},
        {"role",
         row["role"].as<std::string>()},
        {"isActive",
         row["is_active"].as<bool>()},
        {"createdAt",
         row["created_at"]
             .as<std::string>()},
    };
}

} // namespace controllers::detail
