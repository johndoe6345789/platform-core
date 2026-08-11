/**
 * @file job_queue_claim.cpp
 * @brief JobQueue::claimBatch — atomic multi-worker claim.
 *
 * Uses SELECT ... FOR UPDATE SKIP LOCKED inside a transaction so
 * two workers polling simultaneously never return the same row.
 * Rows are transitioned to status='running' and stamped with the
 * worker id and lock timestamp in the same transaction.
 */

#include "JobQueue.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::jobs
{

using drogon::orm::Result;

std::vector<QueuedJob> JobQueue::claimBatch(const std::string& workerId,
                                            std::size_t limit)
{
    std::vector<QueuedJob> claimed;
    if (limit == 0)
    {
        return claimed;
    }

    // Inline the limit to avoid parameterised-query issues on
    // transaction execSqlSync (Drogon extended-protocol bug).
    // limit is our own integer — no injection risk.
    const std::string selectSql =
        "SELECT id, name, handler, payload, priority, attempts, "
        "       max_attempts, backoff_strategy, scheduled_job_id "
        "FROM job_queue "
        "WHERE status IN ('queued','retrying') AND run_at <= now() "
        "ORDER BY priority ASC, run_at ASC "
        "LIMIT " + std::to_string(limit) + " "
        "FOR UPDATE SKIP LOCKED";

    const std::string updateSql =
        "UPDATE job_queue "
        "SET status = 'running', locked_by = $1, locked_at = now(), "
        "    attempts = attempts + 1 "
        "WHERE id = ANY($2::bigint[])";

    try
    {
        auto trans = db_->newTransaction();
        auto rows = trans->execSqlSync(selectSql);

        if (rows.empty())
        {
            return claimed;
        }

        std::string idArray = "{";
        bool first = true;
        claimed.reserve(rows.size());
        for (const auto& row : rows)
        {
            QueuedJob j;
            j.id           = row["id"].as<std::int64_t>();
            j.name         = row["name"].as<std::string>();
            j.handler      = row["handler"].as<std::string>();
            j.payload      = nlohmann::json::parse(
                row["payload"].as<std::string>());
            j.priority     = row["priority"].as<int>();
            j.attempts     = row["attempts"].as<int>() + 1;
            j.maxAttempts  = row["max_attempts"].as<int>();
            j.backoff      = parseBackoff(
                row["backoff_strategy"].as<std::string>());
            if (!row["scheduled_job_id"].isNull())
            {
                j.scheduledJobId = row["scheduled_job_id"].as<std::int32_t>();
            }
            claimed.push_back(std::move(j));

            if (!first) idArray += ",";
            idArray += std::to_string(claimed.back().id);
            first = false;
        }
        idArray += "}";

        trans->execSqlSync(updateSql, workerId, idArray);
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("JobQueue::claimBatch failed: {}", e.base().what());
        throw;
    }

    return claimed;
}

}  // namespace nextra::jobs
