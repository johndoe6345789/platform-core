/**
 * @file ApiKeyAdminSet.cpp
 * @brief Admin endpoint: set a system API key.
 */

#include "ApiKeyController.h"
#include "api-keys/backend/ApiKeyService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ApiKeyController::setSystemKey(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            drogon::k403Forbidden,
            "Admin access required"));
        return;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("provider")
        || !body.contains("apiKey")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "provider and apiKey required"));
        return;
    }

    auto prov =
        body["provider"].get<std::string>();
    auto key =
        body["apiKey"].get<std::string>();
    auto settingKey = prov + "_api_key";

    services::ApiKeyService::setSystemSetting(
        settingKey, key,
        [cb](json result) {
            cb(::utils::jsonOk(result));
        },
        [cb](drogon::HttpStatusCode s,
             std::string msg) {
            cb(::utils::jsonError(s, msg));
        });
}

} // namespace controllers
