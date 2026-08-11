/**
 * @file UserByHandleController.cpp
 * @brief Implementation of the by-handle lookup.
 */

#include "UserByHandleController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon;
using namespace drogon::orm;

namespace controllers
{

void UserByHandleController::byHandle(
    const HttpRequestPtr& /*req*/,
    Cb&& cb, const std::string& handle)
{
    auto db = drogon::app().getDbClient();
    const std::string sql =
        "SELECT id, display_name, avatar_url, "
        "       bio, created_at "
        "FROM users WHERE handle = $1";

    *db << sql << handle
        >> [cb](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    k404NotFound,
                    "user not found"));
                return;
            }
            const auto& row = r[0];
            cb(::utils::jsonOk({
                {"id",
                 row["id"].as<std::string>()},
                {"displayName",
                 row["display_name"]
                     .as<std::string>()},
                {"avatarUrl",
                 row["avatar_url"]
                     .as<std::string>()},
                {"bio",
                 row["bio"].as<std::string>()},
                {"createdAt",
                 row["created_at"]
                     .as<std::string>()},
            }));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error(
                "byHandle: {}",
                e.base().what());
            cb(::utils::jsonError(
                k500InternalServerError,
                "internal error"));
        };
}

} // namespace controllers
