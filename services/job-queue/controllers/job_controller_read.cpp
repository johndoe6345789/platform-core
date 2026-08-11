/**
 * @file job_controller_read.cpp
 * @brief Read endpoints for /api/jobs (queue / runs / dead_letter).
 */

#include "JobController.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

namespace nextra::jobs
{

using namespace drogon;

namespace
{
Json::Value rowsToArray(const orm::Result& rows,
                        const std::vector<std::string>& cols)
{
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows)
    {
        Json::Value o(Json::objectValue);
        for (const auto& c : cols)
            o[c] = r[c].isNull() ? Json::Value(Json::nullValue)
                                 : Json::Value(r[c].as<std::string>());
        arr.append(o);
    }
    return arr;
}
}  // namespace

void JobController::listQueue(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, name, handler, priority, status, attempts, max_attempts, "
        "       run_at, locked_by FROM job_queue "
        "WHERE status IN ('queued','retrying','running') "
        "ORDER BY priority ASC, run_at ASC LIMIT 200");
    Json::Value body;
    body["items"] = rowsToArray(rows, {"id","name","handler","priority",
        "status","attempts","max_attempts","run_at","locked_by"});
    cb(HttpResponse::newHttpJsonResponse(body));
}

void JobController::listRuns(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, job_queue_id, name, handler, attempt, status, "
        "       started_at, finished_at, duration_ms, worker_id "
        "FROM job_runs ORDER BY started_at DESC LIMIT 200");
    Json::Value body;
    body["items"] = rowsToArray(rows, {"id","job_queue_id","name","handler",
        "attempt","status","started_at","finished_at","duration_ms","worker_id"});
    cb(HttpResponse::newHttpJsonResponse(body));
}

void JobController::listDeadLetter(
    const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, original_id, name, handler, attempts, last_error, "
        "       failed_at FROM job_dead_letter "
        "ORDER BY failed_at DESC LIMIT 200");
    Json::Value body;
    body["items"] = rowsToArray(rows, {"id","original_id","name","handler",
        "attempts","last_error","failed_at"});
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::jobs
