/**
 * @file job_queue_mark.cpp
 * @brief JobQueue::markSucceeded / markFailed — terminal transitions.
 *
 * markFailed chooses between a retry (reschedule with backoff) and a
 * dead-letter copy depending on whether @c attempts has reached
 * @c max_attempts.  Both paths write a job_runs history row so the
 * REST controller can show the full trail.
 */

#include "JobBackoff.h"
#include "JobQueue.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::jobs
{

void JobQueue::markSucceeded(std::int64_t jobId,
                             const std::string& workerId,
                             std::chrono::system_clock::time_point startedAt,
                             const JobResult& result)
{
    try
    {
        auto rows = db_->execSqlSync(
            "UPDATE job_queue SET status='succeeded', locked_by=NULL, "
            "locked_at=NULL WHERE id=$1 "
            "RETURNING name, handler, attempts",
            jobId);
        if (rows.empty()) return;

        writeRun(jobId,
                 rows[0]["name"].as<std::string>(),
                 rows[0]["handler"].as<std::string>(),
                 rows[0]["attempts"].as<int>(),
                 JobStatus::Succeeded,
                 startedAt,
                 std::nullopt,
                 result.data,
                 workerId);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("JobQueue::markSucceeded({}) failed: {}",
                      jobId, e.base().what());
        throw;
    }
}

void JobQueue::markFailed(const QueuedJob& job,
                          const std::string& workerId,
                          std::chrono::system_clock::time_point startedAt,
                          const std::string& error)
{
    const bool exhausted = job.attempts >= job.maxAttempts;

    try
    {
        if (exhausted)
        {
            db_->execSqlSync(
                "INSERT INTO job_dead_letter "
                "(original_id, name, handler, payload, attempts, last_error) "
                "VALUES ($1,$2,$3,$4::jsonb,$5,$6)",
                job.id, job.name, job.handler, job.payload.dump(),
                job.attempts, error);
            db_->execSqlSync(
                "UPDATE job_queue SET status='failed', locked_by=NULL, "
                "locked_at=NULL WHERE id=$1",
                job.id);
            spdlog::warn("Job {} ({}) dead-lettered after {} attempts: {}",
                         job.id, job.name, job.attempts, error);
        }
        else
        {
            BackoffConfig cfg;
            const auto delay = computeBackoff(job.backoff, job.attempts, cfg);
            db_->execSqlSync(
                "UPDATE job_queue SET status='retrying', locked_by=NULL, "
                "locked_at=NULL, "
                "run_at = now() + ($2 || ' milliseconds')::interval "
                "WHERE id=$1",
                job.id, std::to_string(delay.count()));
        }

        writeRun(job.id, job.name, job.handler, job.attempts,
                 exhausted ? JobStatus::Failed : JobStatus::Retrying,
                 startedAt, error, {}, workerId);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("JobQueue::markFailed({}) failed: {}",
                      job.id, e.base().what());
        throw;
    }
}

}  // namespace nextra::jobs
