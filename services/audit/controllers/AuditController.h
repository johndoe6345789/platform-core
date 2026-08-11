#pragma once

/**
 * @file AuditController.h
 * @brief REST surface for the audit-manager daemon.
 *
 * Mounted at /api/audit — JWT-gated admin endpoints:
 *   GET /api/audit/events   List recent events for a tenant
 *                           (query params: tenant, since, until,
 *                            limit)
 *   GET /api/audit/verify   Walk the hash chain for a tenant and
 *                           return {ok, rows_checked, first_bad_id}
 */

#include <drogon/HttpController.h>

namespace controllers
{

class AuditController
    : public drogon::HttpController<AuditController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuditController::listEvents,
                  "/api/audit/events",
                  drogon::Get, "JwtFilter");
    ADD_METHOD_TO(AuditController::verifyChain,
                  "/api/audit/verify",
                  drogon::Get, "JwtFilter");
    METHOD_LIST_END

    /**
     * @brief GET /api/audit/events — paginated event list.
     * @param req Incoming request.
     * @param cb  Response callback.
     */
    void listEvents(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /**
     * @brief GET /api/audit/verify — hash-chain integrity check.
     * @param req Incoming request.
     * @param cb  Response callback.
     */
    void verifyChain(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

}  // namespace controllers
