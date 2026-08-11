#pragma once
/**
 * @file MigrationRunner.h
 * @brief DAG-driven migration orchestrator.
 *
 * Bootstraps the schema_migrations table, loads the
 * domain dependency graph, topo-sorts it, then applies
 * each domain's pending SQL files in dependency order.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>

namespace services
{

/**
 * @class MigrationRunner
 * @brief Applies all pending migrations in DAG order.
 */
class MigrationRunner
{
  public:
    /**
     * @brief Construct with paths needed to locate files.
     *
     * @param servicesRoot Path to the `services/` directory.
     * @param graphPath    Path to migration-graph.json.
     * @param bootstrapSql Path to the bootstrap SQL file.
     */
    explicit MigrationRunner(
        std::string servicesRoot,
        std::string graphPath,
        std::string bootstrapSql);

    /**
     * @brief Bootstrap, load DAG, apply all domains.
     *
     * @param onSuccess Callback with JSON array of applied
     *                  domain/file pairs.
     * @param onError   Callback on failure.
     */
    void runMigrations(Callback onSuccess,
                       ErrCallback onError);

  private:
    std::string servicesRoot_;
    std::string graphPath_;
    std::string bootstrapSql_;
};

} // namespace services
