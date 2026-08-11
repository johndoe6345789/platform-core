/**
 * @file ApiKeyController.cpp
 * @brief User API key getAll and remove endpoints.
 *
 * The upsert handler is split into
 * api_key_upsert.cpp for file-size compliance.
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

void ApiKeyController::getAll(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");

    services::ApiKeyService::getAllUserKeys(
        userId,
        [cb](json result) {
            cb(::utils::jsonOk(result));
        },
        [cb](drogon::HttpStatusCode s,
             std::string msg) {
            cb(::utils::jsonError(s, msg));
        });
}

void ApiKeyController::remove(
    const drogon::HttpRequestPtr& req,
    Cb&& cb,
    const std::string& provider)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");

    services::ApiKeyService::deleteUserKey(
        userId, provider,
        [cb](json result) {
            cb(::utils::jsonOk(result));
        },
        [cb](drogon::HttpStatusCode s,
             std::string msg) {
            cb(::utils::jsonError(s, msg));
        });
}

} // namespace controllers
