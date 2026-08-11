#pragma once
/**
 * @file StatusController.h
 * @brief Public status-page endpoints.
 *
 * Two endpoints, both anonymous (no auth filter):
 *
 *   GET /api/status/incidents  — open/in-progress incidents
 *   GET /api/status/history    — recent resolved history
 *
 * The status tool (`tools/status/`) also talks directly to
 * the Phase-0 `/api/healthz` endpoint for live service
 * reachability; this controller only serves incident rows.
 */

#include <drogon/HttpController.h>

namespace nextra::controllers
{

/**
 * @class StatusController
 * @brief Public incident-list endpoints for the status tool.
 */
class StatusController
    : public drogon::HttpController<StatusController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        StatusController::incidents,
        "/api/status/incidents", drogon::Get);
    ADD_METHOD_TO(
        StatusController::history,
        "/api/status/history", drogon::Get);
    METHOD_LIST_END

    /**
     * @brief List active (non-resolved) incidents.
     * @param req Drogon request.
     * @param cb  Response callback.
     */
    void incidents(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /**
     * @brief List recent resolved incidents (latest first).
     * @param req Drogon request.
     * @param cb  Response callback.
     */
    void history(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace nextra::controllers
