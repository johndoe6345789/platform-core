/**
 * @file UserAdminActive.cpp
 * @brief Admin endpoint: toggle user active status.
 */

#include "UserController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::setActive(
    const drogon::HttpRequestPtr& req,
    Cb&& cb, const std::string& id)
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            drogon::k403Forbidden,
            "Admin access required",
            "ADMIN_001"));
        return;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("active")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "active field required",
            "VAL_004"));
        return;
    }

    bool active = body["active"].get<bool>();
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        UPDATE users SET is_active = $1,
            updated_at = NOW()
        WHERE id = $2
        RETURNING id, is_active
    )";

    *db << sql << active << id
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
                 {"isActive",
                  r[0]["is_active"]
                      .as<bool>()},
                 {"updated", true}}));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error("setActive: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Failed to update status",
                "DB_001"));
        };
}

} // namespace controllers
