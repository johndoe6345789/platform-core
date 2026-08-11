#pragma once
/**
 * @file AdminEnvController.h
 * @brief Admin endpoint exposing environment variables
 *        (sensitive values masked).
 */

#include <drogon/HttpController.h>

namespace controllers
{

class AdminEnvController
    : public drogon::HttpController<
          AdminEnvController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        AdminEnvController::getEnvVars,
        "/api/admin/env",
        drogon::Get,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief Return allowlisted env vars. */
    void getEnvVars(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);
};

} // namespace controllers
