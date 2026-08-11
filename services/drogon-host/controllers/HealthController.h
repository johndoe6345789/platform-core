#pragma once
/**
 * @file HealthController.h
 * @brief Health-check endpoint controller.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class HealthController : public drogon::HttpController<HealthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        HealthController::check,
        "/api/health", drogon::Get);
    ADD_METHOD_TO(
        HealthController::version,
        "/api/version", drogon::Get);
    METHOD_LIST_END

    /** @brief Return service health status. */
    void check(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);

    /** @brief Return backend version info. */
    void version(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);
};

} // namespace controllers
