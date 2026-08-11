/**
 * @file AuditControllerVerify.cpp
 * @brief GET /api/audit/verify — hash-chain integrity walker.
 */

#include "audit/controllers/AuditController.h"

#include "audit/backend/HashVerifier.h"

#include <drogon/HttpResponse.h>
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

namespace controllers
{

void AuditController::verifyChain(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb)
{
    auto tenant = req->getParameter("tenant");
    if (tenant.empty()) {
        auto r = drogon::HttpResponse::newHttpResponse();
        r->setStatusCode(drogon::k400BadRequest);
        r->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        r->setBody("{\"error\":\"tenant required\"}");
        cb(r);
        return;
    }

    nextra::audit::HashVerifier verifier(
        drogon::app().getDbClient(), 1000);
    auto report = verifier.verify(tenant);

    nlohmann::json out = {
        {"ok",           report.ok},
        {"rows_checked", report.rowsChecked},
        {"message",      report.message},
    };
    if (report.firstBadId)
        out["first_bad_id"] = *report.firstBadId;

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(out.dump());
    cb(resp);
}

}  // namespace controllers
