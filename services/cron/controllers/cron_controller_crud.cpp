/**
 * @file cron_controller_crud.cpp
 * @brief CRUD endpoints for /api/cron/schedules.
 *
 * The five-field cron expression is validated at write time by
 * parseCron — an invalid expression returns 400 before anything
 * touches the database, so the scheduled_jobs table never holds a
 * row the ticker could choke on.
 */

#include "CronController.h"
#include "cron/backend/CronExpression.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

namespace nextra::cron
{

using namespace drogon;

static HttpResponsePtr badReq(const std::string& msg)
{
    auto r = HttpResponse::newHttpResponse();
    r->setStatusCode(k400BadRequest);
    r->setContentTypeCode(CT_APPLICATION_JSON);
    r->setBody("{\"error\":\"" + msg + "\"}");
    return r;
}

void CronController::list(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, name, cron, handler, enabled, next_run_at, last_run_at, "
        "       description FROM scheduled_jobs ORDER BY name");
    Json::Value items(Json::arrayValue);
    for (const auto& r : rows)
    {
        Json::Value o;
        o["id"]          = r["id"].as<std::int32_t>();
        o["name"]        = r["name"].as<std::string>();
        o["cron"]        = r["cron"].as<std::string>();
        o["handler"]     = r["handler"].as<std::string>();
        o["enabled"]     = r["enabled"].as<bool>();
        o["next_run_at"] = r["next_run_at"].isNull()
            ? std::string{} : r["next_run_at"].as<std::string>();
        o["last_run_at"] = r["last_run_at"].isNull()
            ? std::string{} : r["last_run_at"].as<std::string>();
        o["description"] = r["description"].isNull()
            ? std::string{} : r["description"].as<std::string>();
        items.append(o);
    }
    Json::Value body;
    body["items"] = items;
    cb(HttpResponse::newHttpJsonResponse(body));
}

void CronController::create(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) { cb(badReq("body required")); return; }
    const auto name = (*j)["name"].asString();
    const auto expr = (*j)["cron"].asString();
    const auto handler = (*j)["handler"].asString();
    if (name.empty() || expr.empty() || handler.empty())
    { cb(badReq("name, cron, handler required")); return; }

    try { parseCron(expr); }
    catch (const std::exception& e) { cb(badReq(e.what())); return; }

    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "INSERT INTO scheduled_jobs (name, cron, handler, payload, "
        " description, enabled) VALUES ($1,$2,$3,$4::jsonb,$5,$6) "
        "RETURNING id",
        name, expr, handler,
        j->get("payload", Json::Value(Json::objectValue)).toStyledString(),
        j->get("description", "").asString(),
        j->get("enabled", true).asBool());

    Json::Value body;
    body["id"] = rows[0]["id"].as<std::int32_t>();
    auto r = HttpResponse::newHttpJsonResponse(body);
    r->setStatusCode(k201Created);
    cb(r);
}

}  // namespace nextra::cron
