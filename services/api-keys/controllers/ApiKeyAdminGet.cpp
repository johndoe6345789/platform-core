/**
 * @file ApiKeyAdminGet.cpp
 * @brief Admin endpoint: list system API keys.
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

void ApiKeyController::getSystemKeys(
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

    auto remaining =
        std::make_shared<int>(2);
    auto result =
        std::make_shared<json>(json::array());

    auto collect = [remaining, result, cb](
                       const std::string& prov,
                       json data) {
        auto val = data.value("value", "");
        result->push_back(
            {{"provider", prov},
             {"configured", !val.empty()},
             {"maskedKey",
              val.empty()
                  ? ""
                  : val.substr(0, 4) + "..."}});
        if (--(*remaining) == 0) {
            cb(::utils::jsonOk(
                {{"keys", *result}}));
        }
    };

    auto noErr = [](drogon::HttpStatusCode,
                    std::string) {};

    services::ApiKeyService::getSystemSetting(
        "claude_api_key",
        [collect](json d) {
            collect("claude", d);
        },
        noErr);
    services::ApiKeyService::getSystemSetting(
        "openai_api_key",
        [collect](json d) {
            collect("openai", d);
        },
        noErr);
}

} // namespace controllers
