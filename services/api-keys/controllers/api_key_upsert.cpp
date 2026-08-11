/**
 * @file api_key_upsert.cpp
 * @brief ApiKeyController::upsert implementation.
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

void ApiKeyController::upsert(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");

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
    auto model = body.value("model", "");

    services::ApiKeyService::setUserKey(
        userId, prov, key, model,
        [cb](json result) {
            cb(::utils::jsonOk(result));
        },
        [cb](drogon::HttpStatusCode s,
             std::string msg) {
            cb(::utils::jsonError(s, msg));
        });
}

} // namespace controllers
