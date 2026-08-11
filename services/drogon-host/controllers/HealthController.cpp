/**
 * @file HealthController.cpp
 * @brief Health-check endpoint implementation.
 */

#include "HealthController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <cstdlib>

namespace controllers
{

void HealthController::check(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb)
{
    nlohmann::json body = {
        {"status", "ok"},
        {"version", NEXTRA_VERSION}};
    cb(::utils::jsonOk(body));
}

void HealthController::version(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    const char* feV =
        std::getenv("FRONTEND_VERSION");
    nlohmann::json body = {
        {"backend", NEXTRA_VERSION},
        {"frontend", feV ? feV : "unknown"},
        {"service", "nextra-api"}};
    cb(::utils::jsonOk(body));
}

} // namespace controllers
