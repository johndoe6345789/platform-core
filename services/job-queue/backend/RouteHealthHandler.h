#pragma once

/**
 * @file RouteHealthHandler.h
 * @brief Job handler that smoke-tests every app route
 *        and emits an alert for any 404 or 5xx response.
 *
 * Triggered nightly by the cron-manager via the
 * `routes.health_check` seed schedule.  The job payload
 * carries the portal base URL and the list of routes.
 */

#include "job-queue/backend/JobTypes.h"

namespace nextra::health
{

/**
 * @class RouteHealthHandler
 * @brief JobRegistry handler for `routes.health_check`.
 */
class RouteHealthHandler
{
public:
    /// Invoke synchronously; called by the worker pool.
    nextra::jobs::JobResult run(
        const nextra::jobs::QueuedJob& job) const;

    /// Convenience factory for registerHandler lambdas.
    static nextra::jobs::JobHandler makeHandler();
};

} // namespace nextra::health
