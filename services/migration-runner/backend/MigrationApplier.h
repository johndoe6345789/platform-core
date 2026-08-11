#pragma once
/**
 * @file MigrationApplier.h
 * @brief Sequential migration-apply loop.
 *
 * Applies a list of pending migration filenames one by one,
 * each inside its own Drogon ORM transaction, recording each
 * successful apply in the `schema_migrations` table.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>
#include <vector>

namespace services
{

/**
 * @class MigrationApplier
 * @brief Runs a list of pending SQL files in order.
 */
class MigrationApplier
{
  public:
    /**
     * @brief Apply every file in @p pending sequentially.
     *
     * For each file: read its UP section, execute it in a
     * transaction, and insert its name into
     * `schema_migrations`.
     *
     * @param migrationsDir Filesystem path to the SQL files.
     * @param pending       Sorted list of filenames to apply.
     * @param onSuccess     Callback with JSON array of applied
     *                      filenames.
     * @param onError       Callback on failure.
     */
    static void applyAll(const std::string& migrationsDir,
                         std::vector<std::string> pending, Callback onSuccess,
                         ErrCallback onError);
};

} // namespace services
