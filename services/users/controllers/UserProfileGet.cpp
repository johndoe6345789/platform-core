/**
 * @file UserProfileGet.cpp
 * @brief User profile retrieval endpoint.
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

void UserController::getProfile(
    const drogon::HttpRequestPtr& /*req*/,
    Cb&& cb, const std::string& id)
{
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        SELECT id, username, display_name,
               avatar_url, role, created_at
        FROM users WHERE id = $1
    )";

    *db << sql << id
        >> [cb](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            const auto& row = r[0];
            cb(::utils::jsonOk(
                {{"id", row["id"]
                      .as<std::string>()},
                 {"username",
                  row["username"]
                      .as<std::string>()},
                 {"displayName",
                  row["display_name"]
                      .as<std::string>()},
                 {"avatarUrl",
                  row["avatar_url"]
                      .as<std::string>()},
                 {"role", row["role"]
                      .as<std::string>()},
                 {"createdAt",
                  row["created_at"]
                      .as<std::string>()}}));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error("getProfile: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Internal error"));
        };
}

} // namespace controllers
