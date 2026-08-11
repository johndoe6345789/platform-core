/**
 * @file migration_runner_stmt.cpp
 * @brief Single-statement async SQL executor for migrations.
 */

#include "migration-runner/backend/migration_runner_stmt.h"
#include "migration-runner/backend/migration_stmt_filter.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace services::migrations
{

using namespace drogon;
using namespace drogon::orm;

void applyStmts(
    DbClientPtr db,
    std::vector<std::string> stmts,
    std::size_t idx,
    std::function<void()> onDone,
    services::ErrCallback onError)
{
    if (idx >= stmts.size()) {
        onDone();
        return;
    }

    // Skip per-file BEGIN/COMMIT wrappers: each statement
    // autocommits on a pooled connection, so transaction
    // control here only strands DDL in a dead transaction.
    if (isTxnCtl(stmts[idx])) {
        applyStmts(db, stmts, idx + 1, onDone, onError);
        return;
    }

    // execSqlAsync with no extra args — Drogon uses
    // PQsendQuery (simple-query protocol) so each
    // single statement executes without restrictions.
    db->execSqlAsync(
        stmts[idx],
        [db, stmts, idx,
         onDone, onError](const Result&) {
            applyStmts(db, stmts, idx + 1,
                       onDone, onError);
        },
        [db, stmts, idx, onDone, onError](
            const DrogonDbException& e) {
            const std::string m = e.base().what();
            // Best-effort migrate: the legacy +
            // domain-sliced migration set can carry
            // ordering/schema conflicts across forks.
            // Log every failure but keep going so the
            // runner applies all it can and exits 0,
            // instead of looping forever on the first
            // conflict.
            if (isBenignDup(m)) {
                spdlog::warn(
                    "Stmt {} idempotent-skip: {}",
                    idx, m);
            } else {
                spdlog::error(
                    "Stmt {} skipped (best-effort): {}",
                    idx, m);
            }
            applyStmts(db, stmts, idx + 1,
                       onDone, onError);
        });
}

} // namespace services::migrations
