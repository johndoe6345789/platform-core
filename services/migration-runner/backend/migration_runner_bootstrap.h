#pragma once
/**
 * @file migration_runner_bootstrap.h
 * @brief One-time bootstrap for domain-sliced migrations.
 *
 * Applies 001_schema_migrations_domain_column.sql when the
 * `domain` column is absent, then retro-stamps legacy rows.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <string>

namespace services::migrations
{

/**
 * @class MigrationRunnerBootstrap
 * @brief Ensures the schema_migrations table has the
 *        domain column before domain-sliced runs begin.
 */
class MigrationRunnerBootstrap
{
  public:
    /**
     * @brief Run bootstrap if needed, then call @p then.
     *
     * Checks for the `domain` column in information_schema.
     * If absent: applies the bootstrap SQL file and retro-
     * stamps pre-existing rows with domain='legacy'.
     *
     * @param bootstrapSql Full SQL for the bootstrap script.
     * @param then         Continuation after bootstrap.
     * @param onError      Error callback.
     */
    static void run(
        const std::string& bootstrapSql,
        std::function<void()> then,
        services::ErrCallback onError);
};

} // namespace services::migrations
