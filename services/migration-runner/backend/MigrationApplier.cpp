/**
 * @file MigrationApplier.cpp
 * @brief Sequential migration-apply loop.
 * Uses execSqlAsync (0 params → PQsendQuery) for the
 * migration SQL so multi-statement files work.
 */

#include "migration-runner/backend/MigrationApplier.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <functional>
#include <memory>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void MigrationApplier::applyAll(
    const std::string& migrationsDir,
    std::vector<std::string> pending,
    Callback onSuccess,
    ErrCallback onError)
{
    auto idx  = std::make_shared<std::size_t>(0);
    auto done = std::make_shared<json>(json::array());
    auto next = std::make_shared<std::function<void()>>();

    *next = [migrationsDir, pending, idx, done,
             onSuccess, onError, next]() {
        if (*idx >= pending.size()) {
            onSuccess(*done);
            return;
        }
        const auto fn   = pending[*idx];
        const auto path =
            fmt::format("{}/{}", migrationsDir, fn);

        std::string upSql;
        try {
            upSql = MigrationFileUtils::extractUp(
                MigrationFileUtils::readFile(path));
        } catch (const std::exception& e) {
            onError(k500InternalServerError,
                fmt::format("Cannot read {}: {}",
                            fn, e.what()));
            return;
        }

        auto db = MigrationStateStore::db();

        db->execSqlAsync(           // simple query: multi-statement OK
            upSql,
            [fn, db, idx, done,
             onSuccess, onError, next](const Result&) {
                const std::string ins =
                    "INSERT INTO schema_migrations"
                    "(filename) VALUES ($1) "
                    "ON CONFLICT DO NOTHING";
                db->execSqlAsync(
                    ins,
                    [fn, idx, done, onSuccess,
                     onError, next](const Result&) {
                        spdlog::info("Applied: {}", fn);
                        done->push_back(fn);
                        ++(*idx);
                        (*next)();
                    },
                    [fn, onError](
                        const DrogonDbException& e) {
                        spdlog::error(
                            "Record {} error: {}",
                            fn, e.base().what());
                        onError(k500InternalServerError,
                            fmt::format(
                                "Failed to record {}",
                                fn));
                    },
                    fn);
            },
            [fn, onError](const DrogonDbException& e) {
                spdlog::error("Migration {} failed: {}",
                              fn, e.base().what());
                onError(k500InternalServerError,
                    fmt::format(
                        "Migration {} failed: {}",
                        fn, e.base().what()));
            });
    };

    (*next)();
}

} // namespace services
