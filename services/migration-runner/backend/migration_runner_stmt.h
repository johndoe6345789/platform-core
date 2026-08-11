#pragma once
/**
 * @file migration_runner_stmt.h
 * @brief Single-statement async SQL executor for migrations.
 *
 * applyStmts() drives a vector of pre-split SQL statements through
 * the Drogon DB client one at a time via execSqlAsync with no bound
 * parameters. Drogon uses PQsendQuery (simple-query protocol) when
 * no parameters are bound, which supports multi-statement SQL.
 * Splitting the SQL before this call ensures each invocation gets
 * exactly one statement, which is safe under either protocol.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <drogon/orm/DbClient.h>

#include <functional>
#include <string>
#include <vector>

namespace services::migrations
{

/**
 * @brief Execute pre-split SQL statements sequentially.
 *
 * Iterates through @p stmts starting at @p idx and invokes
 * execSqlAsync for each one. Calls @p onDone when the last
 * statement succeeds. Calls @p onError on the first failure.
 *
 * @param db      Drogon DB client handle.
 * @param stmts   Individual SQL statements (no trailing `;`).
 * @param idx     Index of the next statement to execute.
 * @param onDone  Callback invoked when all statements succeed.
 * @param onError Callback invoked with k500InternalServerError
 *                on the first statement failure.
 */
void applyStmts(
    drogon::orm::DbClientPtr db,
    std::vector<std::string> stmts,
    std::size_t idx,
    std::function<void()> onDone,
    services::ErrCallback onError);

} // namespace services::migrations
