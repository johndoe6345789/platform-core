#pragma once

/**
 * @file JobController.h
 * @brief REST surface for the job-scheduler daemon.
 *
 * Mounted at /api/jobs — read-only dashboards plus the "enqueue
 * arbitrary job" operator hook that the T3 Next.js tool uses to
 * kick off maintenance tasks by hand.  Writes are audited through
 * the universal audit filter; no direct DB access lives here.
 *
 * Endpoints:
 *   GET  /api/jobs/queue            List pending + running jobs
 *   GET  /api/jobs/runs             Recent job_runs history
 *   GET  /api/jobs/dead_letter      Dead-letter rows (paginated)
 *   POST /api/jobs/enqueue          Enqueue an ad-hoc job (admin)
 *   POST /api/jobs/retry_dead/:id   Requeue a dead-letter row
 *   DELETE /api/jobs/queue/:id      Cancel a not-yet-running job
 */

#include <drogon/HttpController.h>

namespace nextra::jobs
{

class JobController : public drogon::HttpController<JobController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(JobController::listQueue,
                  "/api/jobs/queue", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(JobController::listRuns,
                  "/api/jobs/runs", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(JobController::listDeadLetter,
                  "/api/jobs/dead_letter", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(JobController::enqueue,
                  "/api/jobs/enqueue", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(JobController::retryDead,
                  "/api/jobs/retry_dead/{id}", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(JobController::cancelQueued,
                  "/api/jobs/queue/{id}", drogon::Delete,
                  "filters::JwtAuthFilter");
    METHOD_LIST_END

    void listQueue(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void listRuns(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void listDeadLetter(const drogon::HttpRequestPtr& req,
                        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void enqueue(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb);
    void retryDead(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                   std::int64_t id);
    void cancelQueued(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                      std::int64_t id);
};

}  // namespace nextra::jobs
