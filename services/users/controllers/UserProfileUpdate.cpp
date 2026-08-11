/**
 * @file UserProfileUpdate.cpp
 * @brief User profile update endpoint.
 */

#include "UserController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "search/events/SearchEventPublisher.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::updateProfile(
    const drogon::HttpRequestPtr& req,
    Cb&& cb, const std::string& id)
{
    auto callerId = req->attributes()
        ->get<std::string>("user_id");
    if (callerId != id) {
        cb(::utils::jsonError(
            drogon::k403Forbidden,
            "Can only update own profile"));
        return;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Invalid JSON body"));
        return;
    }

    auto displayName =
        body.value("displayName", "");
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        UPDATE users SET display_name = $1,
            updated_at = NOW()
        WHERE id = $2
        RETURNING id, display_name
    )";

    *db << sql << displayName << id
        >> [cb, displayName](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            const auto uid =
                r[0]["id"].as<std::string>();
            nextra::search::SearchEventPublisher
                ::publish("upsert", "users", uid,
                    {{"display_name", displayName}});
            cb(::utils::jsonOk(
                {{"id", uid},
                 {"message",
                  "Profile updated"}}));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error(
                "updateProfile: {}",
                e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Internal error"));
        };
}

} // namespace controllers
