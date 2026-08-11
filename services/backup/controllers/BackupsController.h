#pragma once

/**
 * @file BackupsController.h
 * @brief REST surface for the backup-manager daemon.
 *
 * Mounted at /api/backups — operator dashboards plus the
 * "trigger backup" and "request restore" hooks that the
 * /backups Next.js tool uses.  Writes are audited through
 * the universal audit filter; DB access goes via the
 * BackupRunner service and raw read-only SQL.
 *
 * Endpoints:
 *   GET  /api/backups/runs        List recent backup_runs
 *   GET  /api/backups/policy      List backup_policy rows
 *   POST /api/backups/trigger     Start a backup now
 *   POST /api/backups/restore     Queue a restore from S3
 */

#include <drogon/HttpController.h>

namespace nextra::backup
{

class BackupsController
    : public drogon::HttpController<BackupsController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BackupsController::listRuns,
                  "/api/backups/runs", drogon::Get, "JwtFilter");
    ADD_METHOD_TO(BackupsController::listPolicy,
                  "/api/backups/policy", drogon::Get, "JwtFilter");
    ADD_METHOD_TO(BackupsController::trigger,
                  "/api/backups/trigger", drogon::Post, "JwtFilter");
    ADD_METHOD_TO(BackupsController::restore,
                  "/api/backups/restore", drogon::Post, "JwtFilter");
    METHOD_LIST_END

    void listRuns(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void listPolicy(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void trigger(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void restore(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

}  // namespace nextra::backup
