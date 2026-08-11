/**
 * @file TranslationAdmin.cpp
 * @brief Admin-only translation coverage endpoint.
 */

#include "TranslationController.h"
#include "i18n/backend/TranslationService.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/RoleCheck.h"

#include <nlohmann/json.hpp>

namespace controllers
{

using json = nlohmann::json;

void TranslationController::coverage(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (!::utils::hasRole(role, "admin")) {
        cb(::utils::jsonError(
            drogon::k403Forbidden, "Admin only"));
        return;
    }

    auto ref = req->getOptionalParameter<
        std::string>("ref").value_or("en");

    services::TranslationService::coverage(
        ref,
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
