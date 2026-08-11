/**
 * @file JobScheduler.cpp
 * @brief JobScheduler facade implementation.
 *
 * Owns the worker pool and the recovery loop.  Cron-style
 * scheduling was moved to the separate cron-manager daemon so
 * this process can be scaled horizontally without leader election.
 */

#include "JobScheduler.h"

#include "JobRegistry.h"
#include "JobWorker.h"

#include <spdlog/spdlog.h>

namespace nextra::jobs
{

JobScheduler::JobScheduler(std::shared_ptr<drogon::orm::DbClient> db,
                           SchedulerConfig cfg)
    : db_(std::move(db))
    , cfg_(std::move(cfg))
{
}

JobScheduler::~JobScheduler()
{
    stop();
}

void JobScheduler::registerHandler(const std::string& name, JobHandler fn)
{
    JobRegistry::instance().reg(name, std::move(fn));
}

void JobScheduler::start()
{
    if (running_.exchange(true)) return;

    auto queue = std::make_shared<JobQueue>(db_);
    workers_.reserve(cfg_.workers);
    for (int i = 0; i < cfg_.workers; ++i)
    {
        auto id = cfg_.workerIdPrefix + "-" + std::to_string(i);
        auto w = std::make_unique<JobWorker>(
            std::move(id), queue, cfg_.pollInterval);
        w->start();
        workers_.push_back(std::move(w));
    }

    recoveryThread_ = std::thread([this] { recoveryLoop(); });
    spdlog::info("JobScheduler started with {} workers", cfg_.workers);
}

void JobScheduler::stop()
{
    if (!running_.exchange(false)) return;

    for (auto& w : workers_) w->requestStop();
    for (auto& w : workers_) w->join();
    workers_.clear();

    if (recoveryThread_.joinable()) recoveryThread_.join();
    spdlog::info("JobScheduler stopped");
}

std::size_t JobScheduler::runningWorkers() const
{
    std::size_t n = 0;
    for (const auto& w : workers_)
        if (w->running()) ++n;
    return n;
}

void JobScheduler::recoveryLoop()
{
    JobQueue queue(db_);
    while (running_.load())
    {
        try
        {
            queue.recoverAbandonedJobs(cfg_.lockTimeout);
        }
        catch (const std::exception& e)
        {
            spdlog::error("recoveryLoop: {}", e.what());
        }
        for (int i = 0; i < cfg_.recoveryTick.count() && running_.load(); ++i)
            std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}

}  // namespace nextra::jobs
