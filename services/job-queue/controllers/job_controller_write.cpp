/**
 * @file job_controller_write.cpp
 * @brief Write endpoints for /api/jobs (enqueue / retry / cancel).
 *
 * All mutations go through the universal audit filter so operator
 * actions leave a trail in the audit log — no side-door enqueues.
 */

#include "JobController.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace nextra::jobs
{

using namespace drogon;

void JobController::enqueue(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name") || !json->isMember("handler"))
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k400BadRequest);
        r->setBody("{\"error\":\"name and handler required\"}");
        r->setContentTypeCode(CT_APPLICATION_JSON);
        cb(r);
        return;
    }

    const auto name    = (*json)["name"].asString();
    const auto handler = (*json)["handler"].asString();
    const auto payload = json->isMember("payload")
        ? (*json)["payload"].toStyledString()
        : std::string{"{}"};
    const int priority = json->get("priority", 100).asInt();
    const int maxAttempts = json->get("max_attempts", 5).asInt();
    const auto backoff = json->get("backoff", "exponential").asString();

    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "INSERT INTO job_queue "
        "(name, handler, payload, priority, max_attempts, backoff_strategy) "
        "VALUES ($1,$2,$3::jsonb,$4,$5,$6) RETURNING id",
        name, handler, payload, priority, maxAttempts, backoff);

    Json::Value body;
    body["id"] = rows[0]["id"].as<std::int64_t>();
    spdlog::info("Operator enqueued job {} ({}) as id {}",
                 name, handler, body["id"].asInt64());
    cb(HttpResponse::newHttpJsonResponse(body));
}

void JobController::retryDead(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb,
    std::int64_t id)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "INSERT INTO job_queue "
        "(name, handler, payload, priority, max_attempts, backoff_strategy) "
        "SELECT name, handler, payload, 50, 5, 'exponential' "
        "FROM job_dead_letter WHERE id = $1 "
        "RETURNING id",
        id);
    if (rows.empty())
    {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k404NotFound);
        cb(r);
        return;
    }
    db->execSqlSync("DELETE FROM job_dead_letter WHERE id=$1", id);
    Json::Value body;
    body["requeued_as"] = rows[0]["id"].as<std::int64_t>();
    cb(HttpResponse::newHttpJsonResponse(body));
}

void JobController::cancelQueued(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb,
    std::int64_t id)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "DELETE FROM job_queue "
        "WHERE id = $1 AND status IN ('queued','retrying') "
        "RETURNING id", id);
    auto r = HttpResponse::newHttpResponse();
    r->setStatusCode(rows.empty() ? k409Conflict : k204NoContent);
    cb(r);
}

}  // namespace nextra::jobs
