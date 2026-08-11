/**
 * @file cron_manager_tick.cpp
 * @brief CronManager::runTick — scan scheduled_jobs and enqueue due rows.
 *
 * One DB transaction per tick locks the due rows with SKIP LOCKED
 * so multiple cron-manager instances can run HA against the same
 * database without double-firing a schedule.  Each row's next
 * fire time is recomputed from its expression before the update
 * so clock drift or paused daemons can't lose a schedule.
 */

#include "CronExpression.h"
#include "CronManager.h"

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::cron
{

TickStats CronManager::runTick()
{
    TickStats stats;
    try
    {
        auto trans = db_->newTransaction();
        auto rows = trans->execSqlSync(
            "SELECT id, name, cron, handler, payload "
            "FROM scheduled_jobs "
            "WHERE enabled = TRUE "
            "  AND (next_run_at IS NULL OR next_run_at <= now()) "
            "FOR UPDATE SKIP LOCKED");

        stats.inspected = rows.size();
        for (const auto& row : rows)
        {
            const auto id       = row["id"].as<std::int32_t>();
            const auto name     = row["name"].as<std::string>();
            const auto cronExpr = row["cron"].as<std::string>();
            const auto handler  = row["handler"].as<std::string>();
            const auto payload  = row["payload"].as<std::string>();

            CronExpression cx;
            try { cx = parseCron(cronExpr); }
            catch (const std::exception& e)
            {
                spdlog::error("cron '{}' for {} is invalid: {}",
                              cronExpr, name, e.what());
                ++stats.skipped;
                continue;
            }

            try
            {
                trans->execSqlSync(
                    "INSERT INTO job_queue "
                    "(name, handler, payload, scheduled_job_id) "
                    "VALUES ($1,$2,$3::jsonb,$4)",
                    name, handler, payload, id);

                const auto nextAt = nextFireTime(
                    cx, std::chrono::system_clock::now());
                const auto nextSecs =
                    std::chrono::duration_cast<std::chrono::seconds>(
                        nextAt.time_since_epoch()).count();
                trans->execSqlSync(
                    "UPDATE scheduled_jobs "
                    "SET last_run_at = now(), "
                    "    next_run_at = to_timestamp($1) "
                    "WHERE id = $2",
                    static_cast<double>(nextSecs), id);
                ++stats.enqueued;
            }
            catch (const std::exception& e)
            {
                spdlog::error("cron enqueue {} failed: {}", name, e.what());
                ++stats.errors;
            }
        }
    }
    catch (const drogon::orm::DrogonDbException& e)
    {
        spdlog::error("CronManager tick DB error: {}", e.base().what());
        throw;
    }
    return stats;
}

}  // namespace nextra::cron
