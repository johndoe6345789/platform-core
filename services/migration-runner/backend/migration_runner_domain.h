#pragma once
/**
 * @file migration_runner_domain.h
 * @brief Applies all pending migrations for one domain.
 *
 * For each pending file: reads the UP section, runs it
 * inside a transaction, and inserts a (domain, filename)
 * row into schema_migrations.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <functional>
#include <string>
#include <vector>

namespace services::migrations
{

/**
 * @class MigrationRunnerDomain
 * @brief Sequential per-domain migration applier.
 */
class MigrationRunnerDomain
{
  public:
    /**
     * @brief Apply @p pending files for @p domain in order.
     *
     * @param servicesRoot Path to the `services/` directory.
     * @param domain       Domain name.
     * @param pending      Sorted filenames to apply.
     * @param onDone       Callback when all are applied.
     * @param onError      Error callback.
     */
    static void applyAll(
        const std::string& servicesRoot,
        const std::string& domain,
        std::vector<std::string> pending,
        std::function<void()> onDone,
        services::ErrCallback onError);
};

} // namespace services::migrations
