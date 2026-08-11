#pragma once
/**
 * @file MigrationRollback.h
 * @brief Rolls back the most recently applied SQL migration.
 *
 * Queries `schema_migrations` for the last applied file,
 * executes its `-- DOWN` section inside a transaction, and
 * removes the record from the tracking table.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>

namespace services
{

/**
 * @class MigrationRollback
 * @brief Executes the rollback for the latest migration.
 */
class MigrationRollback
{
  public:
    /**
     * @brief Roll back the most recently applied migration.
     *
     * Locates the last applied filename, reads its `-- DOWN`
     * section, executes it in a transaction, and deletes the
     * row from `schema_migrations`.
     *
     * @param migrationsDir Filesystem path to the SQL files.
     * @param onSuccess     Callback with `{"filename": ...}`.
     * @param onError       Callback on failure.
     */
    static void rollbackLast(const std::string& migrationsDir,
                             Callback onSuccess, ErrCallback onError);
};

} // namespace services
