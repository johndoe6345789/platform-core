/**
 * @file UserAdminRole.cpp
 * @brief Admin endpoint: update user role.
 */

#include "UserController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "user_role_validation.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::setRole(
    const drogon::HttpRequestPtr& req,
    Cb&& cb, const std::string& id)
{
    auto newRole = validateRoleUpdate(req);
    if (!newRole.has_value()) {
        auto role = req->attributes()
            ->get<std::string>("user_role");
        if (role != "admin") {
            cb(::utils::jsonError(
                drogon::k403Forbidden,
                "Admin access required",
                "ADMIN_001"));
        } else {
            cb(::utils::jsonError(
                drogon::k400BadRequest,
                "Valid role field required",
                "VAL_004"));
        }
        return;
    }

    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        UPDATE users SET role = $1,
            updated_at = NOW()
        WHERE id = $2
        RETURNING id, role
    )";

    *db << sql << *newRole << id
        >> [cb](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            cb(::utils::jsonOk(
                {{"id", r[0]["id"]
                      .as<std::string>()},
                 {"role", r[0]["role"]
                      .as<std::string>()},
                 {"updated", true}}));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error("setRole: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Failed to update role",
                "DB_001"));
        };
}

} // namespace controllers
