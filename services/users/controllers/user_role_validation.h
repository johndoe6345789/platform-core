#pragma once
/**
 * @file user_role_validation.h
 * @brief Role validation helpers for admin user
 *        management endpoints.
 */

#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace controllers
{

using json = nlohmann::json;

/**
 * @brief Validate and extract a role from request.
 *
 * Checks that the caller is admin and the body
 * contains a valid role field.
 *
 * @param req  The HTTP request.
 * @return The new role, or nullopt if invalid.
 */
inline auto validateRoleUpdate(
    const drogon::HttpRequestPtr& req)
    -> std::optional<std::string>
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        return std::nullopt;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("role")) {
        return std::nullopt;
    }

    auto newRole =
        body["role"].get<std::string>();
    if (newRole != "user"
        && newRole != "moderator"
        && newRole != "admin") {
        return std::nullopt;
    }

    return newRole;
}

} // namespace controllers
