/**
 * @file TranslationController.cpp
 * @brief Public translation endpoints.
 */

#include "TranslationController.h"
#include "i18n/backend/TranslationService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>

namespace controllers
{

using json = nlohmann::json;

void TranslationController::listLocales(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    services::TranslationService::listLocales(
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

void TranslationController::getLocale(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& locale)
{
    services::TranslationService::getByLocale(
        locale,
        [cb](const json& data) {
            cb(::utils::jsonOk(data));
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
