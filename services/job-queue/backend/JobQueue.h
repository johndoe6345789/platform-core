#pragma once

/**
 * @file JobQueue.h
 * @brief Durable priority queue backed by Postgres job_queue table.
 *
 * Provides the four primitive operations the worker loop needs:
 * enqueue, claim (with SKIP LOCKED for multi-worker safety), complete,
 * and fail/retry.  Also exposes a moveToDeadLetter helper for jobs
 * that have exhausted their retry budget.  All methods are blocking
 * and use a Drogon DbClient obtained at construction.
 */

#include "JobTypes.h"

#include <drogon/orm/DbClient.h>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace nextra::jobs
{

/**
 * @class JobQueue
 * @brief Thin repository over the job_queue / job_runs / job_dead_letter tables.
 *
 * Instances are cheap — hold one per worker and share the underlying
 * Drogon DbClient, which is thread-safe.
 */
class JobQueue
{
public:
    explicit JobQueue(std::shared_ptr<drogon::orm::DbClient> db);

    /**
     * @brief Insert a new job into the queue.
     * @param job  Populated fields; @c id is ignored and reassigned.
     * @param runAt When the job becomes eligible (defaults to now).
     * @return The newly-assigned job id.
     */
    std::int64_t enqueue(const QueuedJob& job,
                         std::chrono::system_clock::time_point runAt = std::chrono::system_clock::now());

    /**
     * @brief Atomically claim up to @p limit ready jobs for this worker.
     *
     * Uses SELECT ... FOR UPDATE SKIP LOCKED inside a transaction so
     * concurrent workers never claim the same row.  Claimed rows are
     * moved to status='running' and stamped with @p workerId.
     */
    std::vector<QueuedJob> claimBatch(const std::string& workerId, std::size_t limit);

    /// Mark a running job as succeeded and write a job_runs row.
    void markSucceeded(std::int64_t jobId,
                       const std::string& workerId,
                       std::chrono::system_clock::time_point startedAt,
                       const JobResult& result);

    /// Mark a running job as failed; either retry or dead-letter.
    void markFailed(const QueuedJob& job,
                    const std::string& workerId,
                    std::chrono::system_clock::time_point startedAt,
                    const std::string& error);

    /// Release claims older than lockTimeout so a crashed worker's
    /// jobs re-enter the ready set.
    void recoverAbandonedJobs(std::chrono::seconds lockTimeout);

private:
    void writeRun(std::int64_t queueId,
                  const std::string& name,
                  const std::string& handler,
                  int attempt,
                  JobStatus status,
                  std::chrono::system_clock::time_point startedAt,
                  const std::optional<std::string>& error,
                  const nlohmann::json& result,
                  const std::string& workerId);

    std::shared_ptr<drogon::orm::DbClient> db_;
};

}  // namespace nextra::jobs
