/**
 * @file job_queue_recover.cpp
 * @brief JobQueue::recoverAbandonedJobs — rescue crashed workers' jobs.
 *
 * The recovery loop runs on a timer inside JobScheduler.  Any row in
 * status='running' whose locked_at is older than @c lockTimeout is
 * assumed to belong to a worker that crashed or was killed, so the
 * lock is released and the row becomes eligible for another worker.
 * The attempt counter is NOT bumped here — the job gets one more
 * real try before the backoff logic in markFailed kicks in.
 */

#include "JobQueue.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::jobs
{

void JobQueue::recoverAbandonedJobs(std::chrono::seconds lockTimeout)
{
    try
    {
        auto rows = db_->execSqlSync(
            "UPDATE job_queue "
            "SET status = 'retrying', locked_by = NULL, locked_at = NULL "
            "WHERE status = 'running' "
            "  AND locked_at IS NOT NULL "
            "  AND locked_at < now() - ($1 || ' seconds')::interval "
            "RETURNING id, name, locked_by",
            std::to_string(lockTimeout.count()));

        for (const auto& row : rows)
        {
            spdlog::warn(
                "Recovered abandoned job {} ({}) from worker {}",
                row["id"].as<std::int64_t>(),
                row["name"].as<std::string>(),
                row["locked_by"].isNull()
                    ? std::string{"<null>"}
                    : row["locked_by"].as<std::string>());
        }
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("JobQueue::recoverAbandonedJobs failed: {}",
                      e.base().what());
        throw;
    }
}

}  // namespace nextra::jobs
