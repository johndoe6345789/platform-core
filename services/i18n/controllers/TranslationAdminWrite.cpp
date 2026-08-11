/**
 * @file TranslationAdminWrite.cpp
 * @brief Admin translation write endpoints: upsert,
 *        remove.
 */

#include "TranslationController.h"
#include "i18n/backend/TranslationService.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "translation_admin_helpers.h"

#include <nlohmann/json.hpp>

namespace controllers
{

using json = nlohmann::json;

void TranslationController::upsert(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    if (!requireAdmin(req, cb)) return;

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);

    if (!body.contains("locale")
        || !body.contains("namespace")
        || !body.contains("key")
        || !body.contains("value")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "locale, namespace, key, value "
            "required"));
        return;
    }

    auto userId = req->attributes()
        ->get<std::string>("user_id");

    services::TranslationService::upsert(
        body["locale"].get<std::string>(),
        body["namespace"].get<std::string>(),
        body["key"].get<std::string>(),
        body["value"].get<std::string>(),
        userId,
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

void TranslationController::remove(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& locale,
    const std::string& ns,
    const std::string& key)
{
    if (!requireAdmin(req, cb)) return;

    services::TranslationService::remove(
        locale, ns, key,
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
