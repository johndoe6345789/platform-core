/**
 * @file cron_controller_preview.cpp
 * @brief /api/cron/preview + /api/cron/tick endpoints.
 *
 * preview is read-only — it never touches the database.  The UI
 * uses it to show operators "if I save this expression, it will
 * fire at ..." before committing.  forceTick is the operator hook
 * for nudging the scheduler out of a wedge during an incident;
 * ordinarily the ticker thread handles this on its own cadence.
 */

#include "CronController.h"
#include "cron/backend/CronExpression.h"
#include "cron/backend/CronManager.h"

#include <drogon/drogon.h>

namespace nextra::cron
{

using namespace drogon;

extern CronManager* g_cronManager;  // set by main() when daemon starts

void CronController::preview(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j || !j->isMember("cron"))
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k400BadRequest);
        r->setBody("{\"error\":\"cron required\"}");
        r->setContentTypeCode(CT_APPLICATION_JSON);
        cb(r); return;
    }

    const auto expr = (*j)["cron"].asString();
    const int count = std::min(j->get("count", 5).asInt(), 10);

    CronExpression cx;
    try { cx = parseCron(expr); }
    catch (const std::exception& e)
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k400BadRequest);
        r->setBody(std::string{"{\"error\":\""} + e.what() + "\"}");
        r->setContentTypeCode(CT_APPLICATION_JSON);
        cb(r); return;
    }

    Json::Value fires(Json::arrayValue);
    auto t = std::chrono::system_clock::now();
    for (int i = 0; i < count; ++i)
    {
        t = nextFireTime(cx, t);
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(
            t.time_since_epoch()).count();
        fires.append(static_cast<Json::Int64>(secs));
    }
    Json::Value body;
    body["next"] = fires;
    cb(HttpResponse::newHttpJsonResponse(body));
}

void CronController::forceTick(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    if (!g_cronManager)
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k503ServiceUnavailable);
        r->setBody("{\"error\":\"cron manager not running in this process\"}");
        r->setContentTypeCode(CT_APPLICATION_JSON);
        cb(r); return;
    }
    const auto stats = g_cronManager->runTick();
    Json::Value body;
    body["inspected"] = static_cast<Json::UInt>(stats.inspected);
    body["enqueued"]  = static_cast<Json::UInt>(stats.enqueued);
    body["skipped"]   = static_cast<Json::UInt>(stats.skipped);
    body["errors"]    = static_cast<Json::UInt>(stats.errors);
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::cron
