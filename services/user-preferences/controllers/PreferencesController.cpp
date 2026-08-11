/**
 * @file PreferencesController.cpp
 * @brief GET/PUT /api/users/me/preferences.
 */

#include "PreferencesController.h"
#include "user-preferences/backend/PreferencesService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>

namespace controllers
{

using json = nlohmann::json;

void PreferencesController::get(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");

    services::PreferencesService::get(
        userId,
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

void PreferencesController::update(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");

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

    /* Read current prefs, merge, then save. */
    services::PreferencesService::get(
        userId,
        [cb, userId, body](const json& cur) {
            auto tm = body.value(
                "themeMode",
                cur.value("themeMode",
                          "system"));
            auto lc = body.value(
                "locale",
                cur.value("locale", "en"));
            auto ap = body.value(
                "aiProvider",
                cur.value("aiProvider",
                          "claude"));

            services::PreferencesService::update(
                userId, tm, lc, ap,
                [cb](const json& d) {
                    cb(::utils::jsonOk(d));
                },
                [cb](drogon::HttpStatusCode c,
                     const std::string& m) {
                    cb(::utils::jsonError(c, m));
                });
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
