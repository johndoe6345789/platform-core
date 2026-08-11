/**
 * @file JobQueue.cpp
 * @brief Constructor, enqueue, and writeRun helper for JobQueue.
 *
 * The remaining JobQueue methods live in sibling translation units
 * (job_queue_claim.cpp, job_queue_mark.cpp, job_queue_recover.cpp)
 * so each file stays under the 100-line project cap.
 */

#include "JobQueue.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::jobs
{

using drogon::orm::Result;

JobQueue::JobQueue(std::shared_ptr<drogon::orm::DbClient> db)
    : db_(std::move(db))
{
}

std::int64_t JobQueue::enqueue(const QueuedJob& job,
                               std::chrono::system_clock::time_point runAt)
{
    const std::string sql =
        "INSERT INTO job_queue "
        "(name, handler, payload, priority, run_at, max_attempts, "
        " backoff_strategy, scheduled_job_id) "
        "VALUES ($1,$2,$3::jsonb,$4,to_timestamp($5),$6,$7,$8) "
        "RETURNING id";

    const auto runAtSecs = std::chrono::duration_cast<std::chrono::seconds>(
        runAt.time_since_epoch()).count();

    try
    {
        auto result = db_->execSqlSync(
            sql,
            job.name,
            job.handler,
            job.payload.dump(),
            job.priority,
            static_cast<double>(runAtSecs),
            job.maxAttempts,
            toString(job.backoff),
            job.scheduledJobId.has_value()
                ? std::to_string(*job.scheduledJobId)
                : std::string{});
        return result[0]["id"].as<std::int64_t>();
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("JobQueue::enqueue failed for {}: {}",
                      job.name, e.base().what());
        throw;
    }
}

void JobQueue::writeRun(std::int64_t queueId,
                        const std::string& name,
                        const std::string& handler,
                        int attempt,
                        JobStatus status,
                        std::chrono::system_clock::time_point startedAt,
                        const std::optional<std::string>& error,
                        const nlohmann::json& result,
                        const std::string& workerId)
{
    const auto startSecs = std::chrono::duration_cast<std::chrono::seconds>(
        startedAt.time_since_epoch()).count();
    const auto now = std::chrono::system_clock::now();
    const auto durationMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - startedAt)
            .count();

    db_->execSqlSync(
        "INSERT INTO job_runs "
        "(job_queue_id, name, handler, attempt, status, started_at, "
        " finished_at, duration_ms, error, result, worker_id) "
        "VALUES ($1,$2,$3,$4,$5,to_timestamp($6),now(),$7,$8,$9::jsonb,$10)",
        queueId,
        name,
        handler,
        attempt,
        toString(status),
        static_cast<double>(startSecs),
        static_cast<int>(durationMs),
        error.value_or(std::string{}),
        result.dump(),
        workerId);
}

}  // namespace nextra::jobs
