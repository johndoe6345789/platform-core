/**
 * @file backups_controller_list.cpp
 * @brief Read endpoints for /api/backups (runs + policy).
 */

#include "BackupsController.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

namespace nextra::backup
{

using namespace drogon;

namespace
{
Json::Value rowsToArray(
    const orm::Result& rows,
    const std::vector<std::string>& cols)
{
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows)
    {
        Json::Value o(Json::objectValue);
        for (const auto& c : cols)
            o[c] = r[c].isNull()
                ? Json::Value(Json::nullValue)
                : Json::Value(r[c].as<std::string>());
        arr.append(o);
    }
    return arr;
}
}  // namespace

void BackupsController::listRuns(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db   = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, started_at, finished_at, status, bytes, "
        "       s3_key, error, kind FROM backup_runs "
        "ORDER BY started_at DESC LIMIT 200");
    Json::Value body;
    body["items"] = rowsToArray(rows, {
        "id","started_at","finished_at","status","bytes",
        "s3_key","error","kind"});
    cb(HttpResponse::newHttpJsonResponse(body));
}

void BackupsController::listPolicy(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db   = app().getDbClient();
    auto rows = db->execSqlSync(
        "SELECT id, name, kind, retain_days, schedule_cron "
        "FROM backup_policy ORDER BY name ASC");
    Json::Value body;
    body["items"] = rowsToArray(rows, {
        "id","name","kind","retain_days","schedule_cron"});
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::backup
