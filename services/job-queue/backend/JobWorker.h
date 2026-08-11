#pragma once

/**
 * @file JobWorker.h
 * @brief One polling thread that drains the job_queue.
 *
 * Each worker owns a @c std::thread, a worker id, and a reference to
 * the shared JobQueue.  The run loop:
 *   1. claimBatch(workerId, batchSize)
 *   2. for each claimed job: resolve handler via JobRegistry, run it,
 *      then markSucceeded or markFailed
 *   3. if batch was empty, sleep pollInterval; otherwise loop
 * Handlers run on the worker thread, so they should NOT block for
 * long — spawn a sub-task and enqueue a follow-up job if needed.
 */

#include "JobQueue.h"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>

namespace nextra::jobs
{

/**
 * @class JobWorker
 * @brief Polling worker thread for the durable job queue.
 *
 * Workers are owned by JobScheduler, which constructs them with a
 * unique id, starts them via start(), and asks them to exit via
 * requestStop() before join().
 */
class JobWorker
{
public:
    JobWorker(std::string id,
              std::shared_ptr<JobQueue> queue,
              std::chrono::milliseconds pollInterval,
              std::size_t batchSize = 4);
    ~JobWorker();

    JobWorker(const JobWorker&) = delete;
    JobWorker& operator=(const JobWorker&) = delete;

    /// Spawn the worker thread.  Idempotent — second call is a no-op.
    void start();

    /// Ask the loop to exit after the current job.
    void requestStop();

    /// Block until the thread has exited.  Safe to call after requestStop.
    void join();

    /// Worker id used to stamp claimed rows and audit events.
    const std::string& id() const { return id_; }

    /// True between start() and the thread returning.
    bool running() const { return running_.load(); }

private:
    void runLoop();
    void runOne(QueuedJob job);

    std::string id_;
    std::shared_ptr<JobQueue> queue_;
    std::chrono::milliseconds pollInterval_;
    std::size_t batchSize_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stop_{false};
    std::thread thread_;
};

}  // namespace nextra::jobs
