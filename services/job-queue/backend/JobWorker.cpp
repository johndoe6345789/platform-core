/**
 * @file JobWorker.cpp
 * @brief JobWorker lifecycle + polling run loop.
 *
 * The per-job handler dispatch lives in job_worker_run_one.cpp so
 * both files stay under the 100-line project cap.
 */

#include "JobWorker.h"

#include <spdlog/spdlog.h>

namespace nextra::jobs
{

JobWorker::JobWorker(std::string id,
                     std::shared_ptr<JobQueue> queue,
                     std::chrono::milliseconds pollInterval,
                     std::size_t batchSize)
    : id_(std::move(id))
    , queue_(std::move(queue))
    , pollInterval_(pollInterval)
    , batchSize_(batchSize)
{
}

JobWorker::~JobWorker()
{
    requestStop();
    join();
}

void JobWorker::start()
{
    if (running_.exchange(true)) return;
    stop_.store(false);
    thread_ = std::thread([this] { runLoop(); });
}

void JobWorker::requestStop()
{
    stop_.store(true);
}

void JobWorker::join()
{
    if (thread_.joinable()) thread_.join();
    running_.store(false);
}

void JobWorker::runLoop()
{
    spdlog::info("JobWorker {} started", id_);
    while (!stop_.load())
    {
        std::vector<QueuedJob> batch;
        try
        {
            batch = queue_->claimBatch(id_, batchSize_);
        }
        catch (const std::exception& e)
        {
            spdlog::error("JobWorker {} claim error: {}", id_, e.what());
            std::this_thread::sleep_for(pollInterval_);
            continue;
        }

        if (batch.empty())
        {
            std::this_thread::sleep_for(pollInterval_);
            continue;
        }

        for (auto& job : batch)
        {
            if (stop_.load()) break;
            runOne(std::move(job));
        }
    }
    spdlog::info("JobWorker {} exiting", id_);
}

}  // namespace nextra::jobs
