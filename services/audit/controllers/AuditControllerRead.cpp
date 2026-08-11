/**
 * @file AuditControllerRead.cpp
 * @brief GET /api/audit/events — paginated list.
 */

#include "audit/controllers/AuditController.h"

#include "audit/backend/AuditWriter.h"

#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

namespace controllers
{

void AuditController::listEvents(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb)
{
    auto tenant = req->getParameter("tenant");
    auto since  = req->getParameter("since");
    auto until  = req->getParameter("until");
    auto lim    = req->getParameter("limit");
    int limit = lim.empty() ? 100 : std::stoi(lim);
    if (limit <= 0 || limit > 1000) limit = 100;
    if (since.empty()) since = "1970-01-01T00:00:00Z";
    if (until.empty()) until = "2999-01-01T00:00:00Z";

    if (tenant.empty()) {
        auto r = drogon::HttpResponse::newHttpResponse();
        r->setStatusCode(drogon::k400BadRequest);
        r->setBody("{\"error\":\"tenant required\"}");
        r->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        cb(r);
        return;
    }

    nextra::audit::AuditWriter writer(
        drogon::app().getDbClient());
    auto rows = writer.list(tenant, since, until, limit);

    nlohmann::json out = nlohmann::json::array();
    for (const auto& a : rows) {
        nlohmann::json j = {
            {"id",           a.id},
            {"tenant_id",    a.tenantId},
            {"action",       a.action},
            {"payload",      a.payload},
            {"prev_hash",    a.prevHashHex},
            {"hash",         a.hashHex},
            {"created_at",   a.createdAt},
        };
        if (a.actorId)    j["actor_id"]    = *a.actorId;
        if (a.targetType) j["target_type"] = *a.targetType;
        if (a.targetId)   j["target_id"]   = *a.targetId;
        out.push_back(std::move(j));
    }

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(out.dump());
    cb(resp);
}

}  // namespace controllers
