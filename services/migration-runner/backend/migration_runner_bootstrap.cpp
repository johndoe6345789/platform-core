/**
 * @file migration_runner_bootstrap.cpp
 * @brief Bootstrap implementation for domain-sliced migrations.
 */

#include "migration-runner/backend/migration_runner_bootstrap.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"
#include "migration-runner/backend/migration_runner_stmt.h"

#include <spdlog/spdlog.h>

#include <utility>

namespace services::migrations
{

using namespace drogon;
using namespace drogon::orm;

static void retroStamp(
    std::function<void()> then,
    services::ErrCallback onError)
{
    auto db = services::MigrationStateStore::db();
    const std::string sql =
        "UPDATE schema_migrations "
        "SET domain='legacy' "
        "WHERE domain IS NULL";
    *db << sql
        >> [then](const Result&) { then(); }
        >> [onError](const DrogonDbException& e) {
            spdlog::error("retroStamp: {}",
                          e.base().what());
            onError(k500InternalServerError,
                    "Failed to retro-stamp legacy rows");
        };
}

static void applyBootstrap(
    const std::string& bootstrapSql,
    std::function<void()> then,
    services::ErrCallback onError)
{
    // Drogon 1.9.3 execSqlAsync always uses the
    // extended-query protocol (prepared), which
    // rejects multi-command strings. Split the
    // bootstrap into single statements (the splitter
    // is $$-aware so the DO block stays intact) and
    // apply them one at a time, mirroring the domain
    // path.
    auto db = services::MigrationStateStore::db();
    auto stmts =
        services::MigrationFileUtils::splitStatements(
            bootstrapSql);
    applyStmts(
        db, std::move(stmts), 0,
        [then, onError]() { retroStamp(then, onError); },
        onError);
}

void MigrationRunnerBootstrap::run(
    const std::string& bootstrapSql,
    std::function<void()> then,
    services::ErrCallback onError)
{
    auto db = services::MigrationStateStore::db();
    const std::string check =
        "SELECT column_name "
        "FROM information_schema.columns "
        "WHERE table_name='schema_migrations' "
        "  AND column_name='domain'";
    *db << check
        >> [bootstrapSql, then, onError](
               const Result& r) {
            if (!r.empty()) {
                spdlog::debug(
                    "Bootstrap already applied");
                then();
                return;
            }
            applyBootstrap(bootstrapSql, then, onError);
        }
        >> [onError](const DrogonDbException& e) {
            spdlog::error("bootstrap check: {}",
                          e.base().what());
            onError(k500InternalServerError,
                    "Failed to check bootstrap state");
        };
}

} // namespace services::migrations
