#pragma once
/**
 * @file translation_admin_helpers.h
 * @brief Admin role check helper for translation
 *        write endpoints.
 */

#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/RoleCheck.h"

#include <drogon/HttpRequest.h>
#include <functional>
#include <string>

namespace controllers
{

/**
 * @brief Check if the request caller is an admin.
 *
 * If not, sends a 403 response and returns false.
 *
 * @param req  The HTTP request.
 * @param cb   Response callback.
 * @return True if admin, false if rejected.
 */
inline auto requireAdmin(
    const drogon::HttpRequestPtr& req,
    const std::function<void(
        const drogon::HttpResponsePtr&)>& cb)
    -> bool
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (!::utils::hasRole(role, "admin")) {
        cb(::utils::jsonError(
            drogon::k403Forbidden, "Admin only"));
        return false;
    }
    return true;
}

} // namespace controllers
