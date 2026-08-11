#pragma once

/**
 * @file JobBackoff.h
 * @brief Pure functions for computing retry delays.
 *
 * Isolated from JobQueue so the strategy can be unit-tested without
 * a database, and so adding a new strategy is a one-file change.
 * All delays are clamped to the configured backoffMaxMs ceiling to
 * prevent pathological job lifetimes.
 */

#include "JobTypes.h"

#include <chrono>

namespace nextra::jobs
{

/// Parameters driving the retry schedule.  Loaded from
/// constants/job-scheduler.json at daemon startup.
struct BackoffConfig
{
    std::chrono::milliseconds baseDelay{1000};
    std::chrono::milliseconds maxDelay{std::chrono::hours{1}};
};

/**
 * @brief Compute the delay before the next retry of a failed job.
 *
 * @param strategy The strategy the job was enqueued with.
 * @param attempt  The attempt number that just failed (1-based).
 * @param cfg      Base and ceiling parameters.
 * @return A positive duration, never larger than @c cfg.maxDelay.
 *
 * Strategies:
 *  - Exponential: @c base * 2^(attempt-1), jittered ±15 percent.
 *  - Linear:      @c base * attempt.
 *  - Fixed:       always @c base (still clamped to maxDelay for safety).
 *
 * The jitter on Exponential prevents thundering-herd retries when
 * many jobs fail at the same time (e.g. audit daemon momentarily
 * unavailable).
 */
std::chrono::milliseconds computeBackoff(BackoffStrategy strategy,
                                         int attempt,
                                         const BackoffConfig& cfg);

}  // namespace nextra::jobs
