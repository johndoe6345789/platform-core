/**
 * @file cron_manager.h
 * @brief CLI subcommand that runs the cron-manager daemon.
 */

#pragma once

#include <string>

namespace commands
{

/**
 * @brief Run the cron manager (scheduled_jobs materialiser).
 *
 * Loads @c constants/cron-manager.json, upserts the seed schedules
 * into @c scheduled_jobs, starts the ticker thread, and mounts the
 * /api/cron controller so the T-sibling "cron" Next.js tool can
 * CRUD schedules live.  The daemon is a separate process from
 * job-scheduler so the control plane and the worker pool can be
 * scaled and restarted independently.
 *
 * @param config  Path to a Drogon JSON config with DB credentials.
 * @throws std::runtime_error if the config or constants file is
 *         missing or malformed.
 */
void cmdCronManager(const std::string& config);

}  // namespace commands
