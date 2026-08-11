/**
 * @file MigrationService.cpp
 * @brief MigrationService facade implementation.
 *
 * Delegates each operation to the appropriate sub-module.
 */

#include "migration-runner/backend/MigrationService.h"
#include "migration-runner/backend/MigrationRollback.h"
#include "migration-runner/backend/MigrationRunner.h"
#include "migration-runner/backend/MigrationStateStore.h"
#include "migration-runner/backend/MigrationStatusQuery.h"

namespace services
{

MigrationService::MigrationService(std::string migrationsDir)
    : migrationsDir_(std::move(migrationsDir))
{
}

void MigrationService::runMigrations(Callback onSuccess, ErrCallback onError)
{
    MigrationRunner runner(
        migrationsDir_,
        migrationsDir_ + "/migration-graph.json",
        migrationsDir_ +
            "/migration-runner/migrations/"
            "001_schema_migrations_domain_column.sql");
    runner.runMigrations(onSuccess, onError);
}

void MigrationService::rollbackLast(Callback onSuccess, ErrCallback onError)
{
    MigrationStateStore::ensureTable(
        [this, onSuccess, onError]() {
            MigrationRollback::rollbackLast(migrationsDir_, onSuccess, onError);
        },
        onError);
}

void MigrationService::getMigrationStatus(Callback onSuccess,
                                          ErrCallback onError)
{
    MigrationStateStore::ensureTable(
        [this, onSuccess, onError]() {
            MigrationStatusQuery::getStatus(migrationsDir_, onSuccess, onError);
        },
        onError);
}

} // namespace services
