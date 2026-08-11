#pragma once

/**
 * @file JobRegistry.h
 * @brief Name -> handler lookup for the job scheduler.
 *
 * Every daemon that dispatches background work registers its handler
 * functions at startup via @ref JobRegistry::reg.  The worker loop
 * then resolves the `handler` column of a job_queue row to the
 * registered callable.  Handlers are pure C++ functions — they do
 * not cross process boundaries, so the scheduler and the workers
 * run in the same backend binary (in the `job-scheduler` subcommand
 * of `nextra-api`).
 */

#include "JobTypes.h"

#include <mutex>
#include <string>
#include <unordered_map>

namespace nextra::jobs
{

/**
 * @class JobRegistry
 * @brief Thread-safe global registry of @ref JobHandler by name.
 *
 * Use @ref reg from daemon startup code and @ref resolve from the
 * worker loop.  Unknown handlers cause the job to be marked failed
 * with a clear error rather than crashing the worker.
 */
class JobRegistry
{
public:
    /// Access the process-wide singleton.
    static JobRegistry& instance();

    /**
     * @brief Register a handler under the given name.
     * @param name The handler identifier (e.g. "audit::prune").
     * @param fn   The callable to invoke when a job with this handler fires.
     * @throws std::logic_error if @p name is already registered.
     */
    void reg(const std::string& name, JobHandler fn);

    /**
     * @brief Look up a handler by name.
     * @param name The handler identifier from the job_queue row.
     * @return The registered handler, or std::nullopt if unknown.
     */
    std::optional<JobHandler> resolve(const std::string& name) const;

    /// Number of currently-registered handlers (for /healthz and tests).
    std::size_t size() const;

    /// Remove every handler.  Used only by the test harness.
    void clearForTesting();

private:
    JobRegistry() = default;
    mutable std::mutex mtx_;
    std::unordered_map<std::string, JobHandler> handlers_;
};

}  // namespace nextra::jobs
