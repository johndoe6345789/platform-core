/**
 * @file backups_controller_restore.cpp
 * @brief POST /api/backups/restore — enqueue a restore job.
 *
 * Restore is destructive, so this endpoint only records the
 * intent: it inserts a row into backup_runs with status
 * 'restore-requested' referencing the chosen s3_key.  The
 * actual pg_restore is performed by an operator after a
 * human-in-the-loop confirmation in the /backups tool.
 */

#include "BackupsController.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace nextra::backup
{

using namespace drogon;

void BackupsController::restore(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    const auto j = req->getJsonObject();
    if (!j || !j->isMember("s3_key"))
    {
        Json::Value err;
        err["error"] = "missing s3_key";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k400BadRequest);
        cb(resp);
        return;
    }

    const std::string key = (*j)["s3_key"].asString();
    auto db = app().getDbClient();
    auto rows = db->execSqlSync(
        "INSERT INTO backup_runs (status, kind, s3_key) "
        "VALUES ('restore-requested', 'full', $1) RETURNING id",
        key);

    Json::Value body;
    body["id"]     = rows[0]["id"].as<Json::Int64>();
    body["status"] = "restore-requested";
    body["s3_key"] = key;
    spdlog::info("backup: restore requested for {}", key);
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::backup
