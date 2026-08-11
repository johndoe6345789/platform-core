/**
 * @file MigrationRollback.cpp
 * @brief MigrationRollback implementation.
 */

#include "migration-runner/backend/MigrationRollback.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void MigrationRollback::rollbackLast(const std::string& migrationsDir,
                                     Callback onSuccess, ErrCallback onError)
{
    auto dbClient = MigrationStateStore::db();
    const std::string sql = "SELECT filename FROM schema_migrations "
                            "ORDER BY applied_at DESC LIMIT 1";

    *dbClient << sql >> [migrationsDir, onSuccess,
                         onError](const Result& result) {
        if (result.empty()) {
            onError(k404NotFound, "No migrations to roll back");
            return;
        }
        auto fn = result[0]["filename"].as<std::string>();
        auto path = fmt::format("{}/{}", migrationsDir, fn);
        std::string downSql;
        try {
            downSql = MigrationFileUtils::extractDown(
                MigrationFileUtils::readFile(path));
        } catch (const std::exception& e) {
            onError(k500InternalServerError,
                    fmt::format("Cannot read {}: {}", fn, e.what()));
            return;
        }
        if (downSql.empty()) {
            onError(k400BadRequest, fmt::format("{} has no DOWN section", fn));
            return;
        }
        auto trans = MigrationStateStore::db()->newTransaction();
        const std::string del = "DELETE FROM schema_migrations "
                                "WHERE filename = $1";
        *trans << downSql >> [trans, fn, del, onSuccess,
                              onError](const Result&) {
            *trans << del << fn >> [fn, onSuccess](const Result&) {
                spdlog::info("Rolled back: {}", fn);
                onSuccess({{"filename", fn}});
            } >> [onError](const DrogonDbException& e) {
                onError(k500InternalServerError, e.base().what());
            };
        } >> [fn, onError](const DrogonDbException& e) {
            spdlog::error("Rollback {} failed: {}", fn, e.base().what());
            onError(k500InternalServerError,
                    fmt::format("Rollback {} failed: {}", fn, e.base().what()));
        };
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("rollbackLast query error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

} // namespace services
