#pragma once
/**
 * @file HealthzController.h
 * @brief /healthz infrastructure readiness endpoint.
 *
 * Reports availability of each backing dependency:
 * Postgres, Redis, Kafka, Elasticsearch.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @class HealthzController
 * @brief Aggregates live probes for all infra dependencies.
 */
class HealthzController
    : public drogon::HttpController<HealthzController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        HealthzController::healthz,
        "/healthz", drogon::Get);
    ADD_METHOD_TO(
        HealthzController::healthz,
        "/api/healthz", drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Return per-dependency status map.
     * @param req HTTP request (unused).
     * @param cb  Completion callback.
     */
    void healthz(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
