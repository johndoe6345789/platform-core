#pragma once
/**
 * @file MigrationService.h
 * @brief Facade for SQL database migration operations.
 *
 * Delegates all work to MigrationRunner, MigrationRollback,
 * and MigrationStatusQuery.  Callers only need this header.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>

namespace services
{

/**
 * @class MigrationService
 * @brief Public API for applying and rolling back migrations.
 */
class MigrationService
{
  public:
    /**
     * @brief Construct with a custom migrations directory.
     *
     * @param migrationsDir Filesystem path to the folder
     *                      containing `.sql` files.
     *                      Defaults to `"migrations"`.
     */
    explicit MigrationService(std::string migrationsDir = "migrations");

    /**
     * @brief Apply all pending migrations in order.
     *
     * @param onSuccess Callback with array of applied filenames.
     * @param onError   Callback on failure.
     */
    void runMigrations(Callback onSuccess, ErrCallback onError);

    /**
     * @brief Roll back the most recently applied migration.
     *
     * @param onSuccess Callback with `{"filename": ...}`.
     * @param onError   Callback on failure.
     */
    void rollbackLast(Callback onSuccess, ErrCallback onError);

    /**
     * @brief Get the apply-status of every known migration.
     *
     * @param onSuccess Callback with a status array.
     * @param onError   Callback on failure.
     */
    void getMigrationStatus(Callback onSuccess, ErrCallback onError);

  private:
    std::string migrationsDir_;
};

} // namespace services
