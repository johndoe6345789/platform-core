#pragma once

/**
 * @file JobTypes.h
 * @brief Shared types for the durable job scheduler (migration 012).
 *
 * Every daemon in the template dispatches its background work through
 * the job scheduler.  This header defines the plain-old-data types
 * shared by the queue, the workers, the cron tick, and the REST API
 * controller.  Implementations live in JobQueue / JobWorker / JobCron.
 */

#include <chrono>
#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace nextra::jobs
{

/// Lifecycle state of a row in job_queue.
enum class JobStatus
{
    Queued,
    Running,
    Retrying,
    Succeeded,
    Failed
};

/// Strategy for computing the delay before the next retry attempt.
enum class BackoffStrategy
{
    Exponential,
    Linear,
    Fixed
};

/// A row of job_queue that a worker has claimed for execution.
struct QueuedJob
{
    std::int64_t id{0};
    std::string name;
    std::string handler;
    nlohmann::json payload;
    int priority{100};
    int attempts{0};
    int maxAttempts{5};
    BackoffStrategy backoff{BackoffStrategy::Exponential};
    std::optional<std::int32_t> scheduledJobId;
};

/// Outcome a handler returns to the worker loop.
struct JobResult
{
    bool success{false};
    std::optional<std::string> error;
    nlohmann::json data;

    static JobResult ok(nlohmann::json data = {}) { return {true, std::nullopt, std::move(data)}; }
    static JobResult fail(std::string msg) { return {false, std::move(msg), {}}; }
};

/// Handler signature registered in JobRegistry by name.
using JobHandler = std::function<JobResult(const QueuedJob&)>;

/// Human-readable conversions used by the REST API + audit events.
std::string toString(JobStatus s);
std::string toString(BackoffStrategy s);
JobStatus parseJobStatus(const std::string& s);
BackoffStrategy parseBackoff(const std::string& s);

}  // namespace nextra::jobs
