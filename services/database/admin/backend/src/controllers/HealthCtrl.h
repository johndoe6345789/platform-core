/**
 * @file HealthCtrl.h
 * @brief Health check endpoint for pgadmin.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief GET /health — liveness probe.
class HealthCtrl
    : public drogon::HttpController<HealthCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        HealthCtrl::check, "/health", drogon::Get);
    METHOD_LIST_END

    /// @brief Return health status.
    void check(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
