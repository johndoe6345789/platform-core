#pragma once

/**
 * @file CronManager.h
 * @brief Facade for the cron-manager daemon.
 *
 * Owns a single ticker thread.  On each tick:
 *   1. loadDueSchedules() — SELECT FOR UPDATE SKIP LOCKED
 *   2. for each due row: enqueueOne() writes a job_queue row
 *   3. rescheduleOne() updates scheduled_jobs.next_run_at
 *
 * The SKIP LOCKED serialization makes it safe to run multiple
 * cron-manager instances against the same database for HA — two
 * tickers never materialise the same schedule twice, and a
 * crashed ticker's unfinished work is picked up on the next wake.
 */

#include "CronTypes.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace drogon::orm { class DbClient; }

namespace nextra::cron
{

/// Configuration loaded from constants/cron-manager.json.
struct CronConfig
{
    std::chrono::seconds tickInterval{30};
    std::chrono::seconds gracefulShutdown{10};
    /// Maximum clock drift we'll ignore when deciding if a row is "due".
    std::chrono::seconds dueSlack{1};
};

/**
 * @class CronManager
 * @brief Owns the ticker thread that materialises scheduled_jobs.
 */
class CronManager
{
public:
    CronManager(std::shared_ptr<drogon::orm::DbClient> db, CronConfig cfg);
    ~CronManager();

    CronManager(const CronManager&) = delete;
    CronManager& operator=(const CronManager&) = delete;

    /// Load seed schedules from cron-manager.json into scheduled_jobs.
    void upsertSeedSchedules(const nlohmann::json& seeds);

    /// Start the ticker thread.  Idempotent — second call is a no-op.
    void start();

    /// Signal shutdown and wait up to @c cfg.gracefulShutdown.
    void stop();

    /// Run a single tick synchronously.  Exposed for tests and for
    /// the REST endpoint POST /api/cron/tick which operators call to
    /// force-advance the scheduler during debugging.
    TickStats runTick();

    bool running() const { return running_.load(); }

private:
    void tickerLoop();

    std::shared_ptr<drogon::orm::DbClient> db_;
    CronConfig cfg_;
    std::atomic<bool> running_{false};
    std::thread tickerThread_;
};

}  // namespace nextra::cron
