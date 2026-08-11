#pragma once

/**
 * @file backup_manager.h
 * @brief CLI subcommand that runs the backup-manager daemon.
 */

#include <string>

namespace commands
{

/**
 * @brief Run the backup-manager daemon.
 *
 * Registers the `backup.run` job-scheduler handler so that
 * the existing `backup.daily` cron seed (see
 * constants/cron-manager.json) dispatches backup runs to
 * this daemon.  Also exposes the /api/backups HTTP surface
 * used by the /backups operator tool for manual triggers
 * and restore flows.
 *
 * @param config Path to a Drogon JSON config with DB creds.
 * @throws std::runtime_error on configuration errors.
 */
void cmdBackupManager(const std::string& config);

}  // namespace commands
