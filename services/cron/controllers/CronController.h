#pragma once

/**
 * @file CronController.h
 * @brief REST surface for the cron-manager daemon.
 *
 * Mounted at /api/cron — CRUD over the scheduled_jobs table plus a
 * "tick now" hook operators can use to force-advance a schedule
 * during debugging.  The T3-sibling "cron" Next.js tool uses this
 * to show a table of schedules, toggle enabled, edit the cron
 * expression with live next-fire-time preview, and one-click run.
 *
 * Endpoints:
 *   GET    /api/cron/schedules       List schedules
 *   POST   /api/cron/schedules       Create a schedule
 *   PUT    /api/cron/schedules/:id   Update enabled/cron/payload
 *   DELETE /api/cron/schedules/:id   Remove a schedule
 *   POST   /api/cron/schedules/:id/fire   Enqueue one run immediately
 *   POST   /api/cron/tick            Force a tick cycle (admin-only)
 *   POST   /api/cron/preview         Evaluate an expression (next N fires)
 */

#include <drogon/HttpController.h>

namespace nextra::cron
{

class CronController : public drogon::HttpController<CronController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CronController::list,
                  "/api/cron/schedules", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::create,
                  "/api/cron/schedules", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::update,
                  "/api/cron/schedules/{id}", drogon::Put,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::remove,
                  "/api/cron/schedules/{id}", drogon::Delete,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::fireNow,
                  "/api/cron/schedules/{id}/fire", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::forceTick,
                  "/api/cron/tick", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(CronController::preview,
                  "/api/cron/preview", drogon::Post,
                  "filters::JwtAuthFilter");
    METHOD_LIST_END

    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void create(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void update(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                std::int32_t id);
    void remove(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                std::int32_t id);
    void fireNow(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                 std::int32_t id);
    void forceTick(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void preview(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

}  // namespace nextra::cron
