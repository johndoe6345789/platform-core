/**
 * @file migration_runner_domain.cpp
 * @brief Per-domain migration applier implementation.
 */

#include "migration-runner/backend/migration_runner_domain.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"
#include "migration-runner/backend/migration_runner_stmt.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <memory>

namespace services::migrations
{

using namespace drogon;
using namespace drogon::orm;

static void applyOne(
    const std::string& servicesRoot,
    const std::string& domain,
    std::vector<std::string> pending,
    std::size_t idx,
    std::function<void()> onDone,
    services::ErrCallback onError)
{
    if (idx >= pending.size()) {
        onDone();
        return;
    }
    const auto& fn = pending[idx];
    auto path = fmt::format(
        "{}/{}/migrations/{}", servicesRoot, domain, fn);
    std::string sql;
    try {
        sql = services::MigrationFileUtils::extractUp(
            services::MigrationFileUtils::readFile(path));
    } catch (const std::exception& e) {
        onError(k500InternalServerError,
                fmt::format("Read {}: {}", fn, e.what()));
        return;
    }

    // Split into individual statements so each execSqlAsync
    // call gets exactly one statement — required because the
    // extended-query protocol rejects multi-statement SQL.
    auto stmts =
        services::MigrationFileUtils::splitStatements(sql);
    auto db = services::MigrationStateStore::db();

    applyStmts(
        db, std::move(stmts), 0,
        [db, domain, fn, pending, idx,
         servicesRoot, onDone, onError]() {
            const std::string ins =
                "INSERT INTO schema_migrations"
                "(domain,filename) VALUES($1,$2) "
                "ON CONFLICT DO NOTHING";
            db->execSqlAsync(
                ins,
                [pending, idx, servicesRoot,
                 domain, fn, onDone, onError](
                    const Result&) {
                    spdlog::info("Applied {}/{}",
                                 domain, fn);
                    applyOne(servicesRoot, domain,
                             pending, idx + 1,
                             onDone, onError);
                },
                [fn, onError](
                    const DrogonDbException& e) {
                    spdlog::error("Record {}: {}",
                                  fn, e.base().what());
                    onError(k500InternalServerError,
                            fmt::format(
                                "Record {}", fn));
                },
                domain, fn);
        },
        onError);
}

void MigrationRunnerDomain::applyAll(
    const std::string& servicesRoot,
    const std::string& domain,
    std::vector<std::string> pending,
    std::function<void()> onDone,
    services::ErrCallback onError)
{
    applyOne(servicesRoot, domain,
             std::move(pending), 0,
             std::move(onDone), std::move(onError));
}

} // namespace services::migrations
