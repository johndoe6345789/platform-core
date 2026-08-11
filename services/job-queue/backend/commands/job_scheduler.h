/**
 * @file job_scheduler.h
 * @brief CLI subcommand that runs the job-scheduler daemon.
 */

#pragma once

#include <string>

namespace commands
{

/**
 * @brief Run the durable job scheduler (workers + recovery).
 *
 * Loads @c constants/job-scheduler.json, spins up the worker pool,
 * the recovery loop, and a small Drogon HTTP listener that exposes
 * the /api/jobs controller for the T3 operator tool.  Blocks until
 * SIGINT/SIGTERM, then drains in-flight work within the configured
 * graceful shutdown window and joins every worker thread.
 *
 * @param config  Path to a Drogon JSON config with DB credentials.
 * @throws std::runtime_error if the config or constants file is
 *         missing or malformed.
 */
void cmdJobScheduler(const std::string& config);

}  // namespace commands
