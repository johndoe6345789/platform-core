#pragma once

/**
 * @file JobScheduler.h
 * @brief Facade that starts the worker pool and the recovery loop.
 *
 * A single JobScheduler instance is constructed from the
 * `./nextra-api job-scheduler` subcommand in main.cpp.  It owns:
 *   - N JobWorker threads that drain job_queue.
 *   - A recovery thread that unlocks jobs stranded by dead workers.
 *
 * Cron-style scheduling (the scheduled_jobs table) lives in the
 * separate cron-manager daemon under services/cron/.  This daemon
 * is intentionally ad-hoc-queue-only so it can scale workers
 * horizontally without electing a leader.
 *
 * Daemons (audit, notifications, webhooks, blog, etc.) register
 * their handlers via @ref registerHandler before @ref start is
 * called.  After @ref stop, in-flight jobs are given a grace period
 * to finish and then the worker threads are joined.
 */

#include "JobBackoff.h"
#include "JobTypes.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace drogon::orm { class DbClient; }

namespace nextra::jobs
{

class JobWorker;

/// Configuration bag populated from constants/job-scheduler.json.
struct SchedulerConfig
{
    int workers{4};
    std::chrono::milliseconds pollInterval{1000};
    std::chrono::seconds lockTimeout{300};
    std::chrono::seconds gracefulShutdown{30};
    std::chrono::seconds recoveryTick{60};
    BackoffConfig backoff;
    std::string workerIdPrefix{"nextra-worker"};
};

/**
 * @class JobScheduler
 * @brief Owns the worker pool, cron ticker, and recovery loop.
 */
class JobScheduler
{
public:
    JobScheduler(std::shared_ptr<drogon::orm::DbClient> db, SchedulerConfig cfg);
    ~JobScheduler();

    JobScheduler(const JobScheduler&) = delete;
    JobScheduler& operator=(const JobScheduler&) = delete;

    /// Register a handler under the given name (thin wrapper around JobRegistry).
    void registerHandler(const std::string& name, JobHandler fn);

    /// Start the worker pool and the recovery loop.
    void start();

    /// Signal shutdown; wait up to @c cfg_.gracefulShutdown for jobs to finish.
    void stop();

    /// For /healthz: number of running workers + last heartbeat.
    std::size_t runningWorkers() const;

private:
    void recoveryLoop();

    std::shared_ptr<drogon::orm::DbClient> db_;
    SchedulerConfig cfg_;
    std::atomic<bool> running_{false};
    std::vector<std::unique_ptr<JobWorker>> workers_;
    std::thread recoveryThread_;
};

}  // namespace nextra::jobs
