/**
 * @file job_worker_run_one.cpp
 * @brief JobWorker::runOne — resolve handler, execute, record outcome.
 *
 * Handlers run synchronously on the worker thread.  Exceptions are
 * caught here so one bad handler cannot bring the worker down: a
 * throw becomes a JobResult::fail which then flows through the
 * normal retry/dead-letter path in JobQueue::markFailed.
 */

#include "JobRegistry.h"
#include "JobWorker.h"

#include <spdlog/spdlog.h>

namespace nextra::jobs
{

void JobWorker::runOne(QueuedJob job)
{
    const auto startedAt = std::chrono::system_clock::now();
    auto handler = JobRegistry::instance().resolve(job.handler);
    if (!handler)
    {
        const std::string err =
            "No handler registered for '" + job.handler + "'";
        spdlog::error("Worker {}: {}", id_, err);
        queue_->markFailed(job, id_, startedAt, err);
        return;
    }

    JobResult result;
    try
    {
        result = (*handler)(job);
    }
    catch (const std::exception& e)
    {
        result = JobResult::fail(std::string{"handler threw: "} + e.what());
    }
    catch (...)
    {
        result = JobResult::fail("handler threw unknown exception");
    }

    if (result.success)
    {
        queue_->markSucceeded(job.id, id_, startedAt, result);
    }
    else
    {
        queue_->markFailed(job, id_, startedAt,
                           result.error.value_or("unknown error"));
    }
}

}  // namespace nextra::jobs
