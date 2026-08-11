#pragma once
/**
 * @file migration_runner_scan.h
 * @brief Computes pending migrations for one domain.
 *
 * Scans `services/<domain>/migrations/*.sql` lexically,
 * then subtracts rows already in schema_migrations for
 * that domain.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <functional>
#include <string>
#include <vector>

namespace services::migrations
{

/**
 * @class MigrationRunnerScan
 * @brief Determines which SQL files are pending for a domain.
 */
class MigrationRunnerScan
{
  public:
    /**
     * @brief Load pending filenames for @p domain.
     *
     * @param servicesRoot Path to the `services/` directory.
     * @param domain       Domain name (maps to subdirectory).
     * @param onReady      Callback with ordered pending list.
     * @param onError      Error callback.
     */
    static void pending(
        const std::string& servicesRoot,
        const std::string& domain,
        std::function<void(
            std::vector<std::string>)> onReady,
        services::ErrCallback onError);
};

} // namespace services::migrations
