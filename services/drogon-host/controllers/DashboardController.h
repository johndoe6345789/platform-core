#pragma once
/**
 * @file DashboardController.h
 * @brief Dashboard aggregation endpoint.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class DashboardController
    : public drogon::HttpController<
          DashboardController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        DashboardController::stats,
        "/api/dashboard/stats",
        drogon::Get,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Get aggregated dashboard stats.
     * @param req  HTTP request.
     * @param cb   Response callback.
     */
    void stats(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
