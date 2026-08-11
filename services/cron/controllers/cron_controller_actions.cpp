/**
 * @file cron_controller_actions.cpp
 * @brief Update/remove + action endpoints for /api/cron/schedules.
 *
 * fireNow inserts a job_queue row for the schedule WITHOUT touching
 * next_run_at, so an operator "run now" does not disrupt the normal
 * cadence.  preview evaluates an expression for up to 10 upcoming
 * fire times so the UI can show operators what they're about to
 * commit to before they save a change.
 */

#include "CronController.h"
#include "cron/backend/CronExpression.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

namespace nextra::cron
{

using namespace drogon;

void CronController::update(
    const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb,
    std::int32_t id)
{
    auto j = req->getJsonObject();
    if (!j) { cb(HttpResponse::newHttpResponse()); return; }

    if (j->isMember("cron"))
    {
        try { parseCron((*j)["cron"].asString()); }
        catch (const std::exception& e)
        {
            auto r = HttpResponse::newHttpResponse();
            r->setStatusCode(k400BadRequest);
            r->setBody(std::string{"{\"error\":\""} + e.what() + "\"}");
            r->setContentTypeCode(CT_APPLICATION_JSON);
            cb(r); return;
        }
    }

    auto db = app().getDbClient();
    db->execSqlSync(
        "UPDATE scheduled_jobs SET "
        "  cron     = COALESCE($2, cron), "
        "  payload  = COALESCE($3::jsonb, payload), "
        "  enabled  = COALESCE($4, enabled), "
        "  updated_at = now() "
        "WHERE id = $1",
        id,
        j->isMember("cron")    ? (*j)["cron"].asString()       : std::string{},
        j->isMember("payload") ? (*j)["payload"].toStyledString(): std::string{},
        j->isMember("enabled") ? ((*j)["enabled"].asBool() ? "true" : "false")
                               : std::string{});
    cb(HttpResponse::newHttpResponse());
}

void CronController::remove(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb,
    std::int32_t id)
{
    app().getDbClient()->execSqlSync(
        "DELETE FROM scheduled_jobs WHERE id=$1", id);
    auto r = HttpResponse::newHttpResponse();
    r->setStatusCode(k204NoContent);
    cb(r);
}

void CronController::fireNow(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb,
    std::int32_t id)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "INSERT INTO job_queue (name, handler, payload, scheduled_job_id) "
        "SELECT name, handler, payload, id FROM scheduled_jobs WHERE id=$1 "
        "RETURNING id", id);
    if (rows.empty())
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k404NotFound);
        cb(r); return;
    }
    Json::Value body;
    body["queued_id"] = rows[0]["id"].as<std::int64_t>();
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::cron
