#pragma once
/**
 * @file MigrationStatusQuery.h
 * @brief Reports the apply-status of every known migration.
 *
 * Compares the `.sql` files on disk against the rows in
 * `schema_migrations` and returns a combined status array.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>

namespace services
{

/**
 * @class MigrationStatusQuery
 * @brief Builds the full migration status report.
 */
class MigrationStatusQuery
{
  public:
    /**
     * @brief Get the status of every migration file.
     *
     * For each `.sql` file in @p migrationsDir, reports
     * whether it has been applied and, if so, when.
     *
     * @param migrationsDir Filesystem path to the SQL files.
     * @param onSuccess     Callback with a JSON status array.
     * @param onError       Callback on failure.
     */
    static void getStatus(const std::string& migrationsDir, Callback onSuccess,
                          ErrCallback onError);
};

} // namespace services
