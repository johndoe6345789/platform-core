#pragma once
/**
 * @file RoleCheck.h
 * @brief Utility for hierarchical role checks.
 *
 * Role hierarchy: guest < user < moderator < admin.
 */

#include <string>
#include <unordered_map>

namespace utils
{

/**
 * @brief Numeric weight for each role.
 * Higher value = more privileges.
 */
inline int roleLevel(const std::string& role)
{
    static const std::unordered_map<
        std::string, int> levels = {
        {"guest",     0},
        {"user",      1},
        {"moderator", 2},
        {"admin",     3},
    };
    auto it = levels.find(role);
    return it != levels.end() ? it->second : -1;
}

/**
 * @brief Check if a role meets a minimum level.
 * @param current  The user's current role.
 * @param required The minimum required role.
 * @return True if current >= required.
 */
inline bool hasRole(
    const std::string& current,
    const std::string& required)
{
    return roleLevel(current) >=
           roleLevel(required);
}

} // namespace utils
